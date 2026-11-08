# R4.1 USB host adapter — isolated review candidate

This Python module implements the actual R4.1 USB-CDC session protocol. It is a new transport adapter, not a GRBL emulator, a deployed Pi service, hardware commissioning or a complete arm-control application. Nothing is opened or started on import. It has not been run against a physical controller, flashed or deployed.

The no-motion CLI requires an explicit dedicated port and an explicit isolated/unloaded-board acknowledgement. It verifies the RP2040 SDK CDC USB VID/PID from metadata before opening and then checks the exact R4.1 response revision and populated mask7. It does not open candidate ports to discover devices. Opening/closing a serial port can alter USB control signals or reset hardware; support the mechanism and use a dedicated electrically checked controller with no loaded arm attached. Missing USB while motor power is off can be intentional because of the required RUN supervisor harness.

The default client sends STATUS, HELLO, PING and optional explicit CHECK. HELLO stops pulses and changes the session, so diagnostic takeover of an already-enabled controller is refused before HELLO. CHECK configures the fitted drivers and can clear their historical fault state; it is performed only when explicitly requested and with all outputs disabled. It is not a passive electrical test.

The client has bounded reads/writes, strict response validation, increasing sequence numbers, a100ms-or-faster heartbeat and exclusive serial access where pySerial supports it. Session changes, timeouts, malformed responses and driver faults invalidate the connection without replaying commands or reconnecting automatically. After a verified-identity failure, or when closing an established session normally, it attempts HOLD and closes the transport; that attempt is not a guaranteed physical stop. HOLD preserves healthy holding-current requests. DISARM is never automatically sent because it can release torque and drop this arm.

## Safe default use

Running `python r41_host.py` only prints help and does not import pySerial or touch hardware. The optional pySerial dependency is needed only for a later physical diagnostic session. The CLI offers no ARM, MOVE, DISARM or unlock flag. It requires the delivered movement-locked firmware. No installation or physical diagnostic command was executed while creating this candidate.

`client.report()` supplies bounded structured application status with driver masks, configured nominal currents, lock/fault state and commanded pulse counters. A disconnected or more-than500ms-old packet is moved into a labelled cached report and is not presented as the live controller. The report always sets `production_motion_ready=false`, `physical_motion_verified=false` and no measured joint position. Currents are the firmware's configured calculations, not measured winding currents. Pulse counts are requests, not encoder data or proof of motor motion.

`format_move()` is an inert serializer that accepts exactly three relative integer pulse counts. It emits the firmware's five-axis wire format with the final two counts always zero, duration50–30000ms, per-command count±2000 and average rate≤500pulses/s. These firmware bounds do not establish a safe joint speed, distance, torque or collision clearance.

An explicitly constructed `allow_unloaded_bench_motion=True` client provides ARM/MOVE/DISARM API support for a **future separately commissioned unloaded bench release**. That is not exposed by the CLI or connected to the live Seizo application; it cannot unlock firmware. It requires exact firmware identity, fresh session/PING, explicit successful CHECK, populated-axis masks, no HOLD/fault, correct enabled axes and firmware `locked=false`. Default clients reject all motion locally. DISARM additionally requires explicit mechanically-supported acknowledgement. The repository's active-product motion quarantine remains unchanged. Do not enable this policy on the real arm merely because software tests pass.

## Integration boundary

The existing Seizo application expects GRBL0.9j/Uno commands. Its XYZ millimetre/G-code interface must **not** be routed into this adapter. The seven-axis Torque host tester is a different protocol and is incompatible.

The new adapter closes the USB transport/session implementation gap. A reviewed application integration still needs: explicit RP2040 controller selection, deliberate mapping of physical joints to M1/M2/M3, measured motor/gearing/direction/step calibration, coordination with the separate Uno/CNC controller, bounded host scheduling, and preservation of all existing physical-control locks. No step count is automatically treated as a joint angle or a measured pose. No host-only claim certifies a brake, independent emergency stop, reliable lifting or power/cooling behavior.

For future integration, construct one `R41Client` per dedicated transport, call `connect()`, consume `client.report()` for telemetry, and keep existing UI motion unavailable until a separate physical commissioning release. The emergency software-stop mapping is `hold()`; it is not a physical stop. Never silently replace Seizo's existing ARM/STOP or calibration rules with the optional bench API.

## Hardware-free verification

`python -m unittest discover -s tests -v` runs strict-status, fragmentation, malformed/duplicate/oversize reply, short-write, timeout, sequence/session, mask, no-motion and CLI-default tests. Native-core integration tests require `R41_CORE_SIM` pointing to the compiled test harness. The harness includes the actual unmodified firmware `core.h`; it has no GPIO, USB-device, UART or physical serial access. Simulator-only `@` commands inject faults/time changes and are never controller protocol commands.

The integration harness models CHECK's reported UART result; it does **not** test real UART wiring or TMC registers. Its optional unlocked mode exists only in a native desktop simulation; the shipped RP2040 UF2 remains locked and unchanged.

References: [pySerial API](https://pyserial.readthedocs.io/en/latest/pyserial_api.html), [USB metadata](https://pyserial.readthedocs.io/en/latest/tools.html), and the reviewed firmware source `main.cpp`/`core.h`. Hardware qualification and supplier acceptance remain separate.
