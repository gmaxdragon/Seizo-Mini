"""Hardware-free transport fault tests and actual firmware-core integration."""
import contextlib
import io
import json
import os
from pathlib import Path
import queue
import subprocess
import sys
import threading
import time
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import r41_host as h


def packet(**changes):
    values = dict(result="OK", revision=h.REVISION, populated_mask=7, locked=True,
                  session=92, seq=1, armed=False, moving=False, hold_open=False,
                  verified_mask=0, enabled_mask=0, fault_mask=0, fault_latched=False,
                  position_known=False, commanded_pulses=[0]*5, rms_mA=[704,704,337,0,0])
    values.update(changes)
    return json.dumps(values, separators=(",", ":")).encode()+b"\n"


class ScriptedTransport:
    def __init__(self, replies, *, fragment=256, short_write=False):
        self.replies = list(replies)
        self.buffer = bytearray()
        self.writes = []
        self.fragment = fragment
        self.short_write = short_write
        self.closed = False

    def write(self, data):
        self.writes.append(data)
        if self.short_write:
            return len(data)-1
        if self.replies:
            self.buffer.extend(self.replies.pop(0))
        return len(data)

    def read(self, size):
        count = min(size, self.fragment, len(self.buffer))
        result = bytes(self.buffer[:count])
        del self.buffer[:count]
        return result

    def close(self):
        self.closed = True


def connected_script(extra=(), **kwargs):
    return ScriptedTransport([packet(session=0,seq=0), packet(seq=0), packet(seq=1), *extra], **kwargs)


class StatusTests(unittest.TestCase):
    def test_exact_status_and_report(self):
        s = h.Snapshot.parse(packet())
        self.assertEqual(s.rms_mA, (704,704,337,0,0))
        report = s.report()
        self.assertFalse(report["physical_motion_verified"])
        self.assertIsNone(report["measured_joint_position"])
        self.assertNotIn("session", report)

    def test_rejects_old_revision(self):
        with self.assertRaises(h.ProtocolError): h.Snapshot.parse(packet(revision="R4 bench"))

    def test_rejects_old_five_driver_mask(self):
        with self.assertRaises(h.ProtocolError): h.Snapshot.parse(packet(populated_mask=31))

    def test_rejects_missing_fields(self):
        with self.assertRaises(h.ProtocolError): h.Snapshot.parse(b'{"result":"OK"}')

    def test_rejects_duplicate_keys(self):
        with self.assertRaises(h.ProtocolError): h.Snapshot.parse(packet()[:-2]+b',"seq":1}\n')

    def test_rejects_non_boolean_lock(self):
        with self.assertRaises(h.ProtocolError): h.Snapshot.parse(packet(locked=1))

    def test_rejects_bool_as_sequence(self):
        with self.assertRaises(h.ProtocolError): h.Snapshot.parse(packet(seq=True))

    def test_rejects_inactive_axis_enable(self):
        with self.assertRaises(h.ProtocolError): h.Snapshot.parse(packet(enabled_mask=8))

    def test_rejects_fake_measured_position(self):
        with self.assertRaises(h.ProtocolError): h.Snapshot.parse(packet(position_known=True))

    def test_rejects_future_pulses(self):
        with self.assertRaises(h.ProtocolError): h.Snapshot.parse(packet(commanded_pulses=[0,0,0,1,0]))

    def test_rejects_future_current(self):
        with self.assertRaises(h.ProtocolError): h.Snapshot.parse(packet(rms_mA=[704,704,337,0,1]))

    def test_rejects_non_ascii(self):
        with self.assertRaises(h.ProtocolError): h.Snapshot.parse(b'\xff')

    def test_rejects_inconsistent_motion(self):
        with self.assertRaises(h.ProtocolError): h.Snapshot.parse(packet(moving=True))


class MoveFormatTests(unittest.TestCase):
    def test_three_axes_produce_five_wire_values(self):
        self.assertEqual(h.format_move(42, 9, 100, [20,-10,5]), b"MOVE 42 9 100 20 -10 5 0 0\n")

    def test_rate_boundary(self):
        self.assertEqual(h.format_move(1,1,50,[25,0,0]), b"MOVE 1 1 50 25 0 0 0 0\n")
        with self.assertRaises(h.ProtocolError): h.format_move(1,1,50,[26,0,0])

    def test_never_accepts_five_input_channels(self):
        with self.assertRaises(h.ProtocolError): h.format_move(1,1,100,[1,0,0,0,0])

    def test_integer_and_range_validation(self):
        bad = [(0,1,100,[1,0,0]),(1,0,100,[1,0,0]),(1,1,49,[1,0,0]),
               (1,1,30001,[1,0,0]),(1,1,5000,[2001,0,0]),(1,1,100,[1.0,0,0]),
               (1,1,100,[True,0,0]),(1,1,100,[0,0,0])]
        for args in bad:
            with self.subTest(args=args), self.assertRaises(h.ProtocolError): h.format_move(*args)


class SessionTests(unittest.TestCase):
    def make(self, extra=(), **kwargs):
        transport = connected_script(extra, **kwargs)
        client = h.R41Client(transport, request_timeout=.025)
        self.addCleanup(client.close)
        client.connect(start_heartbeat=False)
        return client, transport

    def test_fragmented_and_crlf_responses(self):
        tr = connected_script([packet(seq=2).replace(b"\n",b"\r\n")], fragment=1)
        c = h.R41Client(tr)
        self.addCleanup(c.close)
        c.connect(start_heartbeat=False)
        self.assertEqual(c.ping().seq,2)
        self.assertEqual(tr.writes[:4], [b"STATUS\n",b"HELLO\n",b"PING 92 1\n",b"PING 92 2\n"])

    def test_no_auto_arm_or_move(self):
        c,tr = self.make()
        with self.assertRaises(h.MotionUnavailable): c.arm()
        with self.assertRaises(h.MotionUnavailable): c.move_steps([1,0,0],100)
        with self.assertRaises(h.MotionUnavailable): c.disarm()
        self.assertFalse(any(x.startswith((b"ARM",b"MOVE",b"DISARM")) for x in tr.writes))

    def test_holds_without_releasing_torque(self):
        c,tr = self.make([packet(seq=1)])
        c.hold()
        self.assertEqual(tr.writes[-1],b"HOLD\n")
        c.close()
        self.assertNotIn(b"DISARM",b"".join(tr.writes))

    def test_normal_close_attempts_hold_but_never_disarm(self):
        c,tr = self.make()
        c.close()
        self.assertEqual(tr.writes[-1],b"HOLD\n")
        self.assertFalse(any(x.startswith(b"DISARM") for x in tr.writes))

    def test_explicit_check_mask7(self):
        c,tr = self.make([packet(seq=2,result="check_complete",verified_mask=7)])
        self.assertEqual(c.check_drivers().verified_mask,7)
        self.assertEqual(tr.writes[-1],b"CHECK 92 2\n")

    def test_check_wrong_mask_is_failure(self):
        c,tr = self.make([packet(seq=2,result="check_complete",verified_mask=3)])
        with self.assertRaises(h.ControllerFault): c.check_drivers()
        self.assertTrue(tr.closed)
        self.assertEqual(tr.writes[-1],b"HOLD\n")

    def test_malformed_reply_trips_and_does_not_retry(self):
        c,tr = self.make([b"not-json\n"])
        with self.assertRaises(h.ProtocolError): c.ping()
        self.assertFalse(c.connected)
        self.assertEqual(tr.writes.count(b"PING 92 2\n"),1)
        self.assertEqual(tr.writes[-1],b"HOLD\n")

    def test_timeout_trips_and_does_not_retry(self):
        c,tr = self.make()
        with self.assertRaises(h.TransportError): c.ping()
        self.assertTrue(tr.closed)
        self.assertEqual(tr.writes.count(b"PING 92 2\n"),1)

    def test_disconnected_status_is_never_presented_as_live(self):
        c,tr = self.make()
        self.assertTrue(c.report()["response_live"])
        c.close()
        report=c.report()
        self.assertFalse(report["response_live"])
        self.assertIsNone(report["controller"])
        self.assertIsNotNone(report["last_controller_report"])
        self.assertTrue(report["last_report_is_cached"])
        self.assertFalse(report["production_motion_ready"])

    def test_old_response_is_cached_even_before_transport_closes(self):
        c,tr = self.make()
        with patch.object(h.time,"monotonic",return_value=c._received_at+.501):
            self.assertTrue(c.report()["connection_open"])
            self.assertFalse(c.report()["response_live"])
            self.assertIsNone(c.report()["controller"])

    def test_no_packet_report_is_unknown(self):
        c=h.R41Client(ScriptedTransport([]))
        self.addCleanup(c.close)
        self.assertFalse(c.report()["response_live"])
        self.assertIsNone(c.report()["controller"])
        self.assertIsNone(c.report()["last_controller_report"])

    def test_stale_sequence_trips(self):
        c,tr = self.make([packet(seq=1)])
        with self.assertRaises(h.SessionLost): c.ping()
        self.assertTrue(tr.closed)

    def test_reset_session_trips(self):
        c,tr = self.make([packet(session=0,seq=0)])
        with self.assertRaises(h.SessionLost): c.ping()
        self.assertTrue(tr.closed)

    def test_fault_on_heartbeat_trips(self):
        c,tr = self.make([packet(seq=2,fault_mask=2,fault_latched=True)])
        with self.assertRaises(h.ControllerFault): c.ping()
        self.assertEqual(tr.writes[-1],b"HOLD\n")

    def test_short_write_never_retries(self):
        tr = ScriptedTransport([],short_write=True)
        c = h.R41Client(tr)
        with self.assertRaises(h.TransportError): c.connect(start_heartbeat=False)
        self.assertEqual(tr.writes,[b"STATUS\n"])
        self.assertTrue(tr.closed)

    def test_oversize_reply_closes(self):
        c,tr = self.make([b"x"*3000+b"\n"])
        with self.assertRaises(h.ProtocolError): c.ping()
        self.assertTrue(tr.closed)

    def test_refuses_takeover_of_enabled_controller(self):
        tr = ScriptedTransport([packet(enabled_mask=1)])
        c = h.R41Client(tr)
        with self.assertRaises(h.MotionUnavailable): c.connect(start_heartbeat=False)
        self.assertEqual(tr.writes,[b"STATUS\n"])

    def test_refuses_unlocked_firmware_by_default(self):
        tr = ScriptedTransport([packet(locked=False)])
        c = h.R41Client(tr)
        with self.assertRaises(h.MotionUnavailable): c.connect(start_heartbeat=False)
        self.assertEqual(tr.writes,[b"STATUS\n"])

    def test_hello_must_change_previous_session(self):
        tr = ScriptedTransport([packet(session=92,seq=5), packet(session=92,seq=0)])
        c = h.R41Client(tr)
        with self.assertRaises(h.SessionLost): c.connect(start_heartbeat=False)
        self.assertTrue(tr.closed)

    def test_sequence_never_wraps(self):
        c,tr = self.make()
        c.sequence=h.MAX_SEQUENCE
        with self.assertRaises(h.SessionLost): c.ping()
        self.assertFalse(any(str(h.MAX_SEQUENCE+1).encode() in x for x in tr.writes))

    def test_cli_without_port_never_opens_hardware(self):
        with patch.object(h,"open_reviewed_serial") as opened, contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(h.main([]),0)
            opened.assert_not_called()

    def test_cli_requires_isolated_board_acknowledgement(self):
        with patch.object(h,"open_reviewed_serial") as opened, contextlib.redirect_stderr(io.StringIO()):
            with self.assertRaises(SystemExit): h.main(["--port","not-a-real-device"])
            opened.assert_not_called()

    def test_cli_does_not_offer_motion_flags(self):
        for flag in ("--arm","--move","--disarm","--unlock"):
            with patch.object(h,"open_reviewed_serial") as opened, contextlib.redirect_stderr(io.StringIO()):
                with self.assertRaises(SystemExit): h.main([flag])
                opened.assert_not_called()


class CoreTransport:
    """Native actual-core subprocess only. No pyserial import or device access."""
    def __init__(self, unlocked=False):
        binary = os.environ.get("R41_CORE_SIM")
        if not binary:
            raise unittest.SkipTest("Set R41_CORE_SIM to the compiled native core harness")
        args=[binary]+(["--unlocked-simulation"] if unlocked else [])
        self.proc=subprocess.Popen(args,stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        self.writes=[]; self.buffer=bytearray(); self.lines=queue.Queue(); self.closed=False
        def reader():
            while True:
                line=self.proc.stdout.readline()
                if not line: break
                self.lines.put(line)
        self.reader=threading.Thread(target=reader,daemon=True)
        self.reader.start()

    def write(self,data):
        self.writes.append(data)
        count=self.proc.stdin.write(data)
        self.proc.stdin.flush()
        return count

    def inject(self,line):
        self.proc.stdin.write((line+"\n").encode())
        self.proc.stdin.flush()

    def read(self,size):
        if not self.buffer:
            try:self.buffer.extend(self.lines.get(timeout=.01))
            except queue.Empty:return b""
        result=bytes(self.buffer[:size]); del self.buffer[:size]; return result

    def close(self):
        if self.closed:return
        self.closed=True
        self.proc.stdin.close()
        try:self.proc.wait(timeout=.5)
        except subprocess.TimeoutExpired:self.proc.kill();self.proc.wait(timeout=.5)
        self.reader.join(timeout=.2)
        self.proc.stdout.close();self.proc.stderr.close()


class ActualCoreTests(unittest.TestCase):
    def make(self,unlocked=False,heartbeat=False):
        tr=CoreTransport(unlocked)
        c=h.R41Client(tr,allow_unloaded_bench_motion=unlocked,heartbeat_interval=.02)
        self.addCleanup(c.close)
        c.connect(start_heartbeat=heartbeat)
        return c,tr

    def test_locked_core_diagnostics(self):
        c,tr=self.make()
        self.assertTrue(c.snapshot.locked)
        self.assertEqual(c.check_drivers().verified_mask,7)
        with self.assertRaises(h.MotionUnavailable):c.arm()
        self.assertFalse(any(x.startswith((b"ARM",b"MOVE")) for x in tr.writes))

    def test_actual_core_check_failure(self):
        c,tr=self.make();tr.inject("@CHECK_MASK 3")
        with self.assertRaises(h.ControllerFault):c.check_drivers()
        self.assertFalse(c.connected)

    def test_actual_core_three_axis_wire_format_and_pulse_counts(self):
        c,tr=self.make(unlocked=True)
        c.check_drivers();c.arm()
        self.assertTrue(c.move_steps([20,-10,5],100).moving)
        tr.inject("@TICKS 2001")
        result=c.status()
        self.assertFalse(result.moving)
        self.assertEqual(result.commanded_pulses,(20,-10,5,0,0))
        self.assertTrue(any(x.endswith(b" 100 20 -10 5 0 0\n") for x in tr.writes))
        self.assertFalse(result.position_known)

    def test_actual_core_heartbeat_expiry_invalidates_session(self):
        c,tr=self.make(unlocked=True);c.check_drivers();c.arm()
        tr.inject("@ADVANCE 500001")
        with self.assertRaises(h.SessionLost):c.status()
        self.assertFalse(c.connected)

    def test_actual_core_hold_stops_pulses_preserving_enabled_channels(self):
        c,tr=self.make(unlocked=True);c.check_drivers();c.arm();c.move_steps([20,0,0],100)
        held=c.hold()
        self.assertFalse(held.moving)
        self.assertFalse(held.armed)
        self.assertEqual(held.enabled_mask,7)

    def test_actual_core_fault_invalidates_host(self):
        c,tr=self.make(unlocked=True);c.check_drivers();c.arm();tr.inject("@FAULT 2")
        with self.assertRaises(h.ControllerFault):c.ping()
        self.assertEqual(tr.writes[-1],b"HOLD\n")
        self.assertFalse(any(x.startswith(b"DISARM") for x in tr.writes))

    def test_actual_core_lock_change_rejects_motion(self):
        c,tr=self.make(unlocked=True);c.check_drivers();c.arm();tr.inject("@LOCK 1")
        with self.assertRaises(h.CommissioningLocked):c.move_steps([1,0,0],100)
        self.assertFalse(any(x.startswith(b"MOVE") for x in tr.writes))

    def test_heartbeat_thread_serializes_sequences(self):
        c,tr=self.make(heartbeat=True)
        deadline=time.monotonic()+.4
        while len([x for x in tr.writes if x.startswith(b"PING")])<4 and time.monotonic()<deadline:time.sleep(.005)
        c.status();c.close()
        seqs=[int(x.split()[2]) for x in tr.writes if x.startswith(b"PING")]
        self.assertGreaterEqual(len(seqs),4)
        self.assertEqual(seqs,list(range(1,len(seqs)+1)))
        self.assertIsNone(c.heartbeat_error)

    def test_disconnect_is_not_reconnected_or_replayed(self):
        c,tr=self.make();tr.inject("@USB_LOST")
        with self.assertRaises(h.SessionLost):c.ping()
        count=len(tr.writes)
        with self.assertRaises(h.SessionLost):c.ping()
        self.assertEqual(len(tr.writes),count)

    def test_explicit_supported_disarm_only(self):
        c,tr=self.make(unlocked=True);c.check_drivers();c.arm()
        with self.assertRaises(h.MotionUnavailable):c.disarm()
        self.assertEqual(c.disarm(mechanically_supported=True).enabled_mask,0)


if __name__=="__main__":unittest.main(verbosity=2)
