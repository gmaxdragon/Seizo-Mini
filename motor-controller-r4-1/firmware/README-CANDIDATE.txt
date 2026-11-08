SEIZO R4.1 THREE-DRIVER LOCKED REFERENCE CANDIDATE

This variant is for THREE fitted TMC2209 V1.3 modules in U1/U2/U3 only.
The carrier still physically has five sockets. Leave U4/U5 modules absent.
The old CNC/Uno remains separate and unchanged. The variant is a local
engineering candidate. It has not been flashed or tested on hardware.

The compile-time populated mask is decimal7 / binary00111. Hardware UART
warmup, configuration and polling visit only U1/U2/U3. Missing U4/U5 do not
cause communication faults. An absent U1/U2/U3 still makes CHECK fail.
Every declared fitted channel must pass before any later unlocked ARM.

ARM may request only a nonzero subset of mask7. Any ARM selecting U4/U5,
or MOVE containing a nonzero fourth/fifth-axis step count, is rejected as
axis_unpopulated. Those future STEP/DIR outputs are forced low and ENN
outputs remain high. The five-value MOVE wire format is retained: set the
last two values to zero.

The supplied UF2 is COMMISSIONING LOCKED and refuses ARM/MOVE. Default CMake
SEIZO_R4_COMMISSIONING_UNLOCKED remains OFF. A software test that temporarily
models an unlocked core is not permission to unlock or operate an arm.
Do not substitute the previous five-driver binary: it expects mask31 and
will fail CHECK when U4/U5 are absent. Nothing has been auto-deployed.

No-motor reference check, after the carrier/harness first-power tests:
1. HELLO -> obtain a fresh session S.
2. PING S 1
3. CHECK S 2 -> require check_complete, populated_mask7, verified_mask7,
   enabled_mask0 and fault_latchedfalse. No motor needs to be attached.
4. ARM S 3 7 -> MUST return commissioning_locked for this delivered binary.

Nominal requested currents remain707/707/354mA RMS for U1/U2/U3, rounded
down to704/704/337mA. Actual motor suitability, current, torque, temperature,
USB/UART behavior and supply transients remain unmeasured. NEMA frame size
does not establish compatibility. This is not an encoder or closed-loop
controller and does not supply a certified emergency stop or a motor brake.
Reset/power loss can release torque and drop a gravity-loaded arm.

The R4 supervisor/RUN harness,12V-only input and assembly/commissioning
requirements still apply. Never connect/remove drivers or motors while
USB or motor power is connected. Do not infer ship/product readiness from
successful software checks.

Verification is in checks/candidate-verification.json. The original five-axis
regression is tested with an explicit mask31 override; the separate three-axis
tests run with the actual mask7. Real Pico SDK2.3.1 ARM compile/link and UF2
binary round-trip passed. Both software test sets are hardware-free.
