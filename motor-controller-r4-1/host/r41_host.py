"""Bounded USB session transport for Seizo R4.1, isolated from the live app.

No port is discovered/opened and no thread is started by importing this module.
The diagnostic CLI never exposes ARM, MOVE or DISARM. The optional bench API
cannot unlock firmware and is not physical commissioning or a GRBL adapter.
"""
from __future__ import annotations

import argparse
from dataclasses import asdict, dataclass
import json
import math
import re
import sys
import threading
import time
from typing import Protocol

POPULATED_MASK = 7
MAX_SEQUENCE = 2_147_483_647
MAX_REPLY_BYTES = 2048
REVISION = "R4.1 three-driver bench"


class R41Error(RuntimeError):
    pass


class ProtocolError(R41Error):
    pass


class TransportError(R41Error):
    pass


class SessionLost(R41Error):
    pass


class ControllerFault(R41Error):
    pass


class MotionUnavailable(R41Error):
    pass


class CommissioningLocked(MotionUnavailable):
    pass


class CommandRejected(R41Error):
    def __init__(self, result: str):
        self.result = result
        super().__init__(f"Controller rejected command: {result}")


class ByteTransport(Protocol):
    """read must return within 20 ms; write must be bounded by 100 ms."""
    def read(self, size: int) -> bytes: ...
    def write(self, data: bytes) -> int: ...
    def close(self) -> None: ...


def _integer(value, name: str, lo: int, hi: int) -> int:
    if type(value) is not int or not lo <= value <= hi:
        raise ProtocolError(f"Invalid {name}")
    return value


@dataclass(frozen=True)
class Snapshot:
    result: str
    revision: str
    populated_mask: int
    locked: bool
    session: int
    seq: int
    armed: bool
    moving: bool
    hold_open: bool
    verified_mask: int
    enabled_mask: int
    fault_mask: int
    fault_latched: bool
    position_known: bool
    commanded_pulses: tuple[int, ...]
    rms_mA: tuple[int, ...]

    @classmethod
    def parse(cls, raw: bytes) -> "Snapshot":
        def unique_object(pairs):
            obj = {}
            for key, value in pairs:
                if key in obj:
                    raise ProtocolError("Duplicate JSON field")
                obj[key] = value
            return obj
        try:
            obj = json.loads(raw.decode("ascii"), object_pairs_hook=unique_object)
        except (UnicodeError, ValueError) as exc:
            raise ProtocolError("Malformed controller response") from exc
        if not isinstance(obj, dict) or set(obj) != set(cls.__dataclass_fields__):
            raise ProtocolError("Controller response schema mismatch")
        if obj["revision"] != REVISION or obj["populated_mask"] != POPULATED_MASK:
            raise ProtocolError("Not the reviewed R4.1 three-driver protocol")
        if not isinstance(obj["result"], str) or not re.fullmatch(r"(?:OK|[a-z_]{1,48})", obj["result"]):
            raise ProtocolError("Invalid result")
        for name in ("locked", "armed", "moving", "hold_open", "fault_latched", "position_known"):
            if type(obj[name]) is not bool:
                raise ProtocolError(f"Invalid {name}")
        _integer(obj["populated_mask"], "populated_mask", 7, 7)
        _integer(obj["session"], "session", 0, MAX_SEQUENCE)
        _integer(obj["seq"], "seq", 0, MAX_SEQUENCE)
        for name in ("verified_mask", "enabled_mask", "fault_mask"):
            _integer(obj[name], name, 0, POPULATED_MASK)
        if obj["position_known"]:
            raise ProtocolError("This firmware does not measure physical position")
        for name, lo, hi in (("commanded_pulses", -(2**63), 2**63-1), ("rms_mA", 0, 979)):
            values = obj[name]
            if not isinstance(values, list) or len(values) != 5:
                raise ProtocolError(f"Invalid {name}")
            obj[name] = tuple(_integer(v, name, lo, hi) for v in values)
            if any(obj[name][3:]):
                raise ProtocolError("Empty future channels have unexpected data")
        if obj["moving"] and not obj["armed"]:
            raise ProtocolError("Inconsistent motion state")
        if obj["armed"] and not obj["enabled_mask"]:
            raise ProtocolError("Inconsistent enable state")
        return cls(**obj)

    def report(self) -> dict:
        """UI/report shape omits the transient wire session and any port identity."""
        obj = asdict(self)
        obj.pop("session")
        obj.pop("seq")
        obj["physical_motion_verified"] = False
        obj["measured_joint_position"] = None
        return obj


def format_move(session: int, seq: int, duration_ms: int, steps) -> bytes:
    """Pure serializer: three *relative pulse counts*, never angles or GRBL mm."""
    _integer(session, "session", 1, MAX_SEQUENCE)
    _integer(seq, "sequence", 1, MAX_SEQUENCE)
    _integer(duration_ms, "duration_ms", 50, 30_000)
    if not isinstance(steps, (tuple, list)) or len(steps) != 3:
        raise ProtocolError("Exactly three populated-axis step counts required")
    clean = [_integer(v, "step count", -2000, 2000) for v in steps]
    if not any(clean):
        raise ProtocolError("An all-zero move is invalid")
    if any(abs(v)*1000 > duration_ms*500 for v in clean):
        raise ProtocolError("Average pulse rate exceeds firmware limit")
    return f"MOVE {session} {seq} {duration_ms} {' '.join(map(str, clean))} 0 0\n".encode("ascii")


class R41Client:
    def __init__(self, transport: ByteTransport, *, request_timeout: float = .25,
                 heartbeat_interval: float = .10, allow_unloaded_bench_motion: bool = False):
        if not math.isfinite(request_timeout) or not .02 <= request_timeout <= .25:
            raise ValueError("request_timeout must be 0.02–0.25 seconds")
        if not math.isfinite(heartbeat_interval) or not .02 <= heartbeat_interval <= .10:
            raise ValueError("heartbeat_interval must be 0.02–0.10 seconds")
        if type(allow_unloaded_bench_motion) is not bool:
            raise ValueError("bench policy must be explicit boolean")
        self.transport = transport
        self.request_timeout = request_timeout
        self.heartbeat_interval = heartbeat_interval
        self.allow_unloaded_bench_motion = allow_unloaded_bench_motion
        self.session = 0
        self.sequence = 0
        self.snapshot: Snapshot | None = None
        self.connected = False
        self.closed = False
        self.heartbeat_error: str | None = None
        self._identity_verified = False
        self._received_at: float | None = None
        self._rx = bytearray()
        self._lock = threading.RLock()
        self._stop = threading.Event()
        self._thread: threading.Thread | None = None

    def _invalidate(self, *, attempt_hold: bool = True) -> None:
        # HOLD stops future pulses but deliberately does not release healthy
        # holding torque. Never auto-DISARM: the known arm can drop on release.
        if attempt_hold and not self.closed and self._identity_verified:
            try:
                self.transport.write(b"HOLD\n")
            except Exception:
                pass
        self.connected = False
        self.closed = True
        self.session = 0
        self.sequence = 0
        self._stop.set()
        try:
            self.transport.close()
        except Exception:
            pass

    def _read_line(self) -> bytes:
        deadline = time.monotonic() + self.request_timeout
        while time.monotonic() < deadline:
            if b"\n" in self._rx:
                line, _, tail = self._rx.partition(b"\n")
                self._rx[:] = tail
                if line.strip():
                    if len(line) > MAX_REPLY_BYTES:
                        raise ProtocolError("Controller response too long")
                    return bytes(line).rstrip(b"\r")
            if len(self._rx) > MAX_REPLY_BYTES:
                raise ProtocolError("Controller response too long")
            chunk = self.transport.read(min(256, MAX_REPLY_BYTES+1-len(self._rx)))
            if not isinstance(chunk, bytes):
                raise TransportError("Transport did not return bytes")
            if chunk:
                self._rx.extend(chunk)
            else:
                self._stop.wait(.002)
        raise TransportError("Controller response timed out; command is not retried")

    def _exchange(self, wire: bytes, *, expected_seq: int | None,
                  hello: bool = False, permit_fault: bool = False) -> Snapshot:
        with self._lock:
            if self.closed:
                raise SessionLost("Transport closed; explicitly reconnect with a new client")
            try:
                if self.transport.write(wire) != len(wire):
                    raise TransportError("Short serial write; command is not retried")
                status = Snapshot.parse(self._read_line())
                if hello:
                    if not status.session or status.seq or status.session == self.session:
                        raise SessionLost("HELLO did not establish a fresh session")
                elif expected_seq is not None:
                    if status.session != self.session or status.seq != expected_seq:
                        raise SessionLost("Controller session or sequence changed")
                self.snapshot = status
                self._received_at = time.monotonic()
                self._identity_verified = True
                if status.result == "session_or_sequence":
                    raise SessionLost("Controller rejected session/sequence")
                if status.fault_latched and not permit_fault:
                    raise ControllerFault(f"Driver fault mask {status.fault_mask}; explicit recheck required")
                if status.result == "commissioning_locked":
                    raise CommissioningLocked("Firmware commissioning lock remains active")
                if status.result not in {"OK", "check_complete"}:
                    if status.result == "check_failed":
                        raise ControllerFault("One or more fitted drivers failed CHECK")
                    raise CommandRejected(status.result)
                return status
            except (CommissioningLocked, CommandRejected):
                raise
            except Exception as exc:
                self._invalidate()
                if isinstance(exc, R41Error):
                    raise
                raise TransportError("Serial transport failed; command is not retried") from exc

    def connect(self, *, start_heartbeat: bool = True, allow_fault_recheck: bool = False) -> Snapshot:
        with self._lock:
            if self.connected or self.closed:
                raise SessionLost("Use a fresh client for each connection")
            first = self._exchange(b"STATUS\n", expected_seq=None, permit_fault=allow_fault_recheck)
            # Do not silently take over a controller that might hold a real arm.
            if first.armed or first.moving or first.enabled_mask:
                self._invalidate(attempt_hold=False)
                raise MotionUnavailable("Controller outputs already enabled; diagnostic takeover refused")
            if not first.locked and not self.allow_unloaded_bench_motion:
                self._invalidate(attempt_hold=False)
                raise MotionUnavailable("Diagnostic client requires the delivered locked firmware")
            self.session = first.session
            fresh = self._exchange(b"HELLO\n", expected_seq=None, hello=True,
                                   permit_fault=allow_fault_recheck)
            self.session, self.sequence = fresh.session, 0
            self.connected = True
            fresh = self._authenticated("PING", permit_fault=allow_fault_recheck)
            if start_heartbeat and not fresh.fault_latched:
                self.start_heartbeat()
            return fresh

    def _authenticated(self, command: str, *args: int, permit_fault: bool = False) -> Snapshot:
        with self._lock:
            if not self.connected or self.closed or not self.session:
                raise SessionLost("No verified session")
            if self.sequence >= MAX_SEQUENCE:
                self._invalidate()
                raise SessionLost("Sequence exhausted; explicit new connection required")
            self.sequence += 1
            words = " ".join(map(str, args))
            wire = f"{command} {self.session} {self.sequence}{' '+words if words else ''}\n".encode("ascii")
            return self._exchange(wire, expected_seq=self.sequence, permit_fault=permit_fault)

    def status(self) -> Snapshot:
        with self._lock:
            if not self.connected:
                raise SessionLost("No verified session")
            return self._exchange(b"STATUS\n", expected_seq=self.sequence)

    def report(self) -> dict:
        """Never present an old packet as a currently connected controller."""
        with self._lock:
            age = None if self._received_at is None else max(0.0, time.monotonic()-self._received_at)
            live = bool(self.connected and not self.closed and age is not None and age <= .5)
            cached = self.snapshot.report() if self.snapshot else None
            return {"connection_open": bool(self.connected and not self.closed),
                    "response_live": live, "response_age_seconds": age,
                    "controller": cached if live else None,
                    "last_controller_report": cached,
                    "last_report_is_cached": not live,
                    "heartbeat_error": self.heartbeat_error,
                    "host_bench_policy_enabled": self.allow_unloaded_bench_motion,
                    "production_motion_ready": False,
                    "physical_motion_verified": False,
                    "measured_joint_position": None}

    def ping(self) -> Snapshot:
        return self._authenticated("PING")

    def check_drivers(self) -> Snapshot:
        """Explicit operator recheck: configures UART drivers and clears good faults."""
        with self._lock:
            if not self.snapshot or self.snapshot.enabled_mask or self.snapshot.moving:
                raise MotionUnavailable("All outputs must be disabled before CHECK")
            result = self._authenticated("CHECK", permit_fault=True)
            if result.result != "check_complete" or result.verified_mask != POPULATED_MASK or result.fault_latched:
                self._invalidate()
                raise ControllerFault("CHECK did not verify exactly the three fitted slots")
            return result

    def start_heartbeat(self) -> None:
        with self._lock:
            if not self.connected or self.closed:
                raise SessionLost("No verified session")
            if self._thread and self._thread.is_alive():
                return
            def worker():
                while not self._stop.wait(self.heartbeat_interval):
                    try:
                        self.ping()
                    except R41Error as exc:
                        self.heartbeat_error = type(exc).__name__
                        with self._lock:
                            self._invalidate()
                        return
            self._thread = threading.Thread(target=worker, name="r41-heartbeat", daemon=True)
            self._thread.start()

    def hold(self) -> Snapshot:
        with self._lock:
            if not self.connected:
                raise SessionLost("No verified session")
            return self._exchange(b"HOLD\n", expected_seq=self.sequence)

    def _require_bench_motion(self) -> Snapshot:
        if not self.allow_unloaded_bench_motion:
            raise MotionUnavailable("Motion API disabled by host policy; CLI has no motion controls")
        if not self.snapshot or not self.connected:
            raise SessionLost("No verified session")
        if self.snapshot.locked:
            raise CommissioningLocked("Host cannot unlock the delivered firmware")
        if self.snapshot.hold_open or self.snapshot.fault_latched:
            raise MotionUnavailable("HOLD or driver fault blocks motion")
        if self.snapshot.verified_mask != POPULATED_MASK:
            raise MotionUnavailable("Explicit successful CHECK required")
        return self.snapshot

    def arm(self, mask: int = POPULATED_MASK) -> Snapshot:
        """Future qualified unloaded bench API; unavailable in the diagnostic CLI."""
        _integer(mask, "axis mask", 1, POPULATED_MASK)
        with self._lock:
            self._require_bench_motion()
            self.ping()
            current = self._require_bench_motion()
            if current.moving:
                raise MotionUnavailable("Controller is busy")
            return self._authenticated("ARM", mask)

    def move_steps(self, steps, duration_ms: int) -> Snapshot:
        # Validate before any I/O, including PING. Never replay after failure.
        format_move(1, 1, duration_ms, steps)
        with self._lock:
            before = self._require_bench_motion()
            if not before.armed or before.moving:
                raise MotionUnavailable("Controller must be explicitly armed and idle")
            if any(v and not (before.enabled_mask & (1 << i)) for i, v in enumerate(steps)):
                raise MotionUnavailable("Requested axis is not enabled")
            current = self.ping()
            self._require_bench_motion()
            if not current.armed or current.moving:
                raise MotionUnavailable("Armed/idle state changed before move")
            if any(v and not (current.enabled_mask & (1 << i)) for i, v in enumerate(steps)):
                raise MotionUnavailable("Requested axis is no longer enabled")
            return self._authenticated("MOVE", duration_ms, *steps, 0, 0)

    def disarm(self, *, mechanically_supported: bool = False) -> Snapshot:
        if mechanically_supported is not True:
            raise MotionUnavailable("DISARM releases torque: mechanically support every gravity load first")
        with self._lock:
            self._require_bench_motion()
            return self._authenticated("DISARM")

    def close(self) -> None:
        # Closing does not issue DISARM or claim a hardware stop/brake.
        self._stop.set()
        with self._lock:
            if not self.closed:
                if self.connected and self._identity_verified:
                    try:
                        self.transport.write(b"HOLD\n")
                    except Exception:
                        pass
                self.closed = True
                self.connected = False
                self.session = 0
                self.sequence = 0
                try:
                    self.transport.close()
                except Exception:
                    pass
        if self._thread and self._thread is not threading.current_thread():
            self._thread.join(timeout=.5)


def open_reviewed_serial(port: str) -> ByteTransport:
    """Explicit metadata allowlist; never scans by opening candidate ports."""
    import serial
    from serial.tools import list_ports
    matches = [p for p in list_ports.comports() if p.device == port]
    if len(matches) != 1 or (matches[0].vid, matches[0].pid) != (0x2E8A, 0x000A):
        raise MotionUnavailable("Explicit port is not the reviewed RP2040 SDK CDC USB identity")
    # This alone is not device authentication; STATUS must also match exactly.
    options = dict(baudrate=115200, timeout=.02, write_timeout=.10,
                   xonxoff=False, rtscts=False, dsrdtr=False)
    if sys.platform != "win32":
        options["exclusive"] = True
    return serial.Serial(port=port, **options)


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description="R4.1 no-motion diagnostics; never flash, ARM, MOVE or DISARM")
    parser.add_argument("--port", help="Explicit dedicated USB serial port; no auto discovery/opening")
    parser.add_argument("--isolated-unloaded-controller", action="store_true",
                        help="Use only after electrical first-power checks, with no loaded mechanism attached")
    parser.add_argument("--check", action="store_true", help="Explicit UART configuration/recheck of U1/U2/U3")
    parser.add_argument("--monitor-seconds", type=float, default=0)
    args = parser.parse_args(argv)
    if not args.port:
        parser.print_help()
        return 0
    if not args.isolated_unloaded_controller:
        parser.error("Refusing to open a port without --isolated-unloaded-controller")
    if not math.isfinite(args.monitor_seconds) or not 0 <= args.monitor_seconds <= 60:
        parser.error("--monitor-seconds must be 0–60")
    client = None
    try:
        client = R41Client(open_reviewed_serial(args.port))
        client.connect(start_heartbeat=False, allow_fault_recheck=args.check)
        if args.check:
            client.check_drivers()
        client.start_heartbeat()
        client.status()
        print(json.dumps(client.report(), sort_keys=True))
        deadline = time.monotonic() + args.monitor_seconds
        while time.monotonic() < deadline:
            time.sleep(min(.2, max(0, deadline-time.monotonic())))
            client.status()
            print(json.dumps(client.report(), sort_keys=True))
        return 0
    except (R41Error, ImportError, OSError) as exc:
        print(f"R4.1 diagnostics stopped: {type(exc).__name__}: {exc}", file=sys.stderr)
        return 2
    finally:
        if client:
            client.close()


if __name__ == "__main__":
    raise SystemExit(main())
