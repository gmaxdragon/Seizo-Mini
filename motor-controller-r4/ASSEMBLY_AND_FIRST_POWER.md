# Seizo motor controller R4 — assembly and first power

This is the five-axis prototype with a motor-supply reset monitor. Its CAD and software checks are recorded separately from the physical tests below. No assembled controller has been powered or tested by this work.

## Connections

| Connection | What belongs there |
|---|---|
| Zero USB-C | One data cable from a Pi USB-A port. Powers logic and carries serial commands. |
| J6 | **Regulated 12 V DC, center positive only.** Intended owned adapter: 12 V/4 A, nominal 5.5 mm outside / 2.1 mm inside plug. Verify its actual label, condition, polarity and fit. Do not use 24 V because the board's protection is selected for 12 V. |
| J1–J5 | One four-wire bipolar stepper per connector: **1 A1, 2 A2, 3 B1, 4 B2**. |
| J7 | Isolated normally closed HOLD contact, or an intentional bench jumper, between pin 1 GND and pin 2 HOLD_CONTACT. No external voltage. |
| J8 | Specified 12 V fan: pin 1 GND, pin 2 protected fan supply, pins 3/4 unused. |
| J9 | **Required internal reset harness:** pin 1 VMOT_RESET_N to Zero RESET/Key2 RUN contact; pin 2 GND to the same switch's GND contact. |

The old CNC shield and its Uno/USB connection are unchanged. The new carrier has five new driver modules and its own single USB connection. It has no servos, encoders, position sensors or automatic homing.

Select the Zero's actual USB serial device by its device identity; do not assume it will always be `/dev/ttyACM0` or select the Uno's port. The new board uses the documented serial protocol, and integration with the existing Seizo application has not been run on hardware.

Slots 1–2 are intended for suitably current-rated NEMA 23 motors; slots 3–5 for suitably current-rated NEMA 17/pancakes. A NEMA frame size is not a winding-current rating or torque guarantee. These TMC2209 modules cannot provide full rated current for every common NEMA 23 motor. The reference firmware starts near 0.7 A RMS on slots 1/2 and 0.35 A RMS on slots 3/4/5; check actual motor ratings before any unlock.

The initial shared input design envelope is **2.5 A / 30 W**. It is not a measured continuous rating or an electronic aggregate current clamp. Do not share the adapter with the old CNC setup without checking combined demand and power distribution. Do not parallel adapter outputs.

## Before soldering

Use `cost/R4-purchase-BOM.csv` for purchasing and `assembly/` for front/back reference drawings and pad coordinates. KiCad **Ctrl+F** also locates a component reference. Keep the complete `project/` folder together so its custom libraries remain available.

- U1–U5: exact BIGTREETECH TMC2209 V1.3 modules with R110 sensing. Fit ten 1×8 female sockets; confirm actual delivered revision. Do not substitute another StepStick by appearance.
- U6: Waveshare RP2040-Zero-M. Solder its supplied 23 male pins through the carrier; VSYS is intentionally unconnected. Do not add a motor-power connection to it.
- U7: TI SN74HC541DWR, wide SOIC-20 with 1.27 mm lead pitch.
- U8: TI TPS3808G01DBVR, SOT-23-6 with exposed 0.95 mm-pitch leads. It requires careful small-part soldering. Match pin 1 and inspect under magnification.
- J1–J5: exact DORABO DB128V-5.08-4P-GN-S terminals, 1.60 mm holes.
- D1: SB560-E3/54, series diode on 25.4 mm lead pitch. Leave its body about 3–5 mm above the board for airflow.
- D2: SMCJ15A on the **back**, beside C1. Follow the back-side cathode mark.
- R5: **fitted wire link**, formed from a clipped resistor lead. It is not optional. R4 stays 1 kΩ; do not put a 1 kΩ resistor at R5.
- R6/R7/R9: 10 kΩ; R8: 1 kΩ. Check values, since an incorrect divider changes the reset threshold.
- C2/C3: the specified 100 nF capacitors, beside U7/U8 respectively. CT/pin 4 of U8 is intentionally unconnected; do not bridge it to a neighboring pin.

Five U7 pads contain ordinary plated escape vias: **7, 9, 16, 18 and 19**. Before fitting U7, fill these holes with solder and wick the solder lands flat. Inspect them, then solder the chip. Otherwise solder can wick down the holes. This is a hand-assembly process requirement; factory filled/capped vias are not included in the low-cost PCB estimate. The two other former in-pad vias were moved away from their solder lands in R4.

Fit small SMD parts first, followed by resistors/diodes, capacitors, sockets and connectors. Match all polarized parts. Fit D1's band/cathode to VMOT; D3's band faces FAN_VMOT; D2's cathode faces VMOT. Avoid solder bridges or trapped strands. Keep C1 and its leads clear of the warm D1 body.

Use properly rated phase cable, not loose Dupont jumpers, for motors. Short 20–22 AWG phase cable is a practical starting point for the stated low-current bench profiles. Main power wiring, the connector and PCB must tolerate 4 A for a possible fault duration. A 4 A adapter can current-limit without promptly opening a 4 A fuse. Never bypass a fuse to make a fault disappear.

## Fit the reset harness correctly

The supervisor's output must reach the module's actual RUN net. That net is **not** one of the 23 carrier header pads.

1. First program the bare Zero with `firmware/Seizo_R4_LOCKED_BENCH.uf2`: hold BOOT, connect USB, release BOOT and copy the UF2 to its boot drive. This build refuses ARM/MOVE. Disconnect USB afterward.
2. With all power disconnected, identify the Zero's **RESET/Key2** switch. The official schematic connects **Key2.1 to RUN** and **Key2.2 to GND**. Do not use the BOOT/Key1 switch.
3. Use a working multimeter to identify the GND-side switch contact by continuity to module GND. The RUN-side contact connects to GND when RESET is pressed. The physical switch can have multiple solder feet on each electrical contact; do not guess a foot from left/right appearance.
4. Connect J9 pin 1 to the verified RUN-side contact, and J9 pin 2 to the verified GND-side contact, using a short insulated wire pair. Keep the pair together, away from phase wiring, and provide strain relief. Do not put 5 V or 12 V on either wire.
5. Check continuity from each J9 pin to its intended switch net, absence of a solder bridge, and connector orientation. This external harness is part of the circuit; CAD connectivity checks cannot verify that you fitted it.

If the contacts cannot be identified and soldered reliably, keep the controller unpowered and get assembly help. Omitting the harness removes the new reset protection.

With J9 connected and motor power absent, the MCU is intentionally held in reset: **USB serial will not enumerate even though the USB 3.3 V supply exists.** This differs from R3. Motor power must be present for normal serial operation. For later reprogramming, disconnect all motors and use either the documented bare-module/service procedure or checked motor power; refit and recheck the harness before operation.

## Mounting and cooling

Nominal board outline: 100 × 100 mm. The four 3.2 mm mounting holes are **asymmetric**, at `(4,4)`, `(96,4)`, `(96,96)` and `(4,49.1)` mm from the top-left outline corner. Use the actual assembly drawing/native board for the mount; do not assume four corner holes.

Provide insulated supports with at least 5 mm underside clearance, checking actual rear D2 height and all solder joints. Keep conductive surfaces and loose hardware away. Retain the board and cables mechanically; solder joints are not strain relief.

Fit each heatsink with the intended thermal/insulated interface, clear of module pins. The 40 mm fan needs airflow over all five modules and D1; a printed duct may be needed. Confirm the specified fan starts and runs with its PWM lead unconnected before any motor heat test. J8's unused signals must not be connected to USB 3.3 V. Actual fan, socket, heatsink, plug and enclosure fit remain receipt/assembly checks.

## First power — no motors, commissioning locked

Use a clean, nonconductive bench, a working external multimeter and preferably a current-limited bench supply for first electrical checks. An uncommissioned built-in meter or a 4 A adapter label does not establish a safe low commissioning-current limit.

1. **Unpowered, driver sockets empty:** check values/polarity, fuses, R5 link, J9 harness, solder joints and absence of a persistent rail-to-GND short. Capacitor charging can briefly change a resistance reading; do not decide from one continuity beep.
2. **USB only:** verify the 3.3 V rail. With VMOT discharged, U8/J9 RESET must be low and serial unavailable. If the MCU runs normally with the harness fitted and no motor power, the guard is not working as intended. Stop and inspect. ENN pins must be inactive/high; compare to the actual VIO rail.
3. **Add checked 12 V, still no drivers/motors:** confirm correct VMOT and fan voltage/polarity. VMOT should be below the input by D1's forward drop; the fan has its separate protected branch. Confirm the fan spins. U8 should release RESET after VMOT recovers above its threshold and the delay; serial should enumerate and STATUS report `locked:true`.
4. **Remove motor power while leaving USB:** RESET should assert when VMOT falls through the threshold, and USB serial should disappear. Restore motor power: the module must boot fresh, with no enable or movement request restored. Static meter checks cannot establish the full timing; capture RESET/ENN/VMOT waveforms before loaded use.
5. **Remove both supplies:** measure VMOT below 1 V before handling parts. Do not short capacitors to discharge them. Insert all five exact TMC modules, matching EN to EN and VM to VM on both rows. A reversed or one-pin-shifted module can be destroyed.
6. **Power without motors:** use HELLO, PING and CHECK as described in `firmware/BENCH-FIRMWARE.txt`. Require `check_complete`, `verified_mask:31`, `enabled_mask:0` and `fault_latched:false`. The supplied UF2 must refuse ARM and MOVE. Test J7 open/closed status as well.
7. Record actual voltages, idle current, fan behavior, UART checks and power-sequence results. Stop on unexpected current, heat, odor, sound or voltage. Do not proceed to movement on a failed check.

Never insert/remove modules, plug/unplug motors, move wires or alter jumpers while either supply is connected. A powered motor unplug can damage a driver even when no steps are being sent.

## Later unloaded motor qualification

Identify each motor's two winding pairs with a multimeter while disconnected and unpowered. One complete pair goes to A1/A2 and the other to B1/B2. Wire colors are not standardized. Check a six/eight-wire motor's datasheet rather than guessing a connection.

Before an intentionally unlocked build, establish actual motor ratings, correct module/shunt revision, current scale and cooling. Current settings are calculated nominally, not measured. Start with one mechanically secured, unloaded motor at low speed; then check each slot and coordinated timing. Keep the initial measured shared input within 2.5 A. No claim is made that all five motors can run at their maximum currents simultaneously.

Qualification includes sustained temperature of drivers/D1/capacitors/sockets, fan failure/obstruction, ripple, load steps, regenerated voltage under deceleration, USB/session loss, HOLD bounce/noise, UART faults/watchdog and motor-supply drop/recovery. Sweep dip duration/depth and measure actual RESET/ENN behavior, including individual module resets. These tests have not been performed here.

## What this protection does and does not establish

- U8's nominal trip is 8.505 V. The calculated 8.18–8.84 V band includes IC and initial resistor tolerances; resistor temperature drift and physical noise/measurement remain additional considerations. With CT open, the specified release interval is 12–28 ms. Its specified sense-to-reset delay is up to 20 µs under the datasheet test conditions. Very brief dips or individual module resets are not guaranteed to be caught.
- The qualifying common-bus drop resets the MCU, discards old enable state and requires a new software session/CHECK/ARM. Randomized sessions reduce stale-command reuse. The monitor is not a certified safe-torque-off or independent safety controller.
- A reset, driver fault, DISARM, USB/VIO loss or power loss **can release holding torque and drop a gravity-loaded arm**. Even a low-voltage reset with USB still present can release torque. Mechanical support/braking is required for such loads; software cannot prevent a powerless joint from falling.
- HOLD suppresses STEP and requires manual rearm; it is not a safety-rated emergency stop. Healthy holding current is retained during an ordinary software HOLD, so temperature still matters.
- The SMCJ15A is transient suppression, not a continuous braking resistor. The 970 µF nominal motor-bus capacitance and TVS do not establish safe regenerated-energy handling for an unknown arm/load.
- Pulse counts are commanded positions, not measured joint positions. There is no encoder/homing/collision guarantee.
- Delivered module/supply fit, motor torque, power sequencing, soldering, thermal limits and final landed cost remain unverified. Digital checks cannot replace them.

Sources and exact verification records are in `checks/` and `firmware/checks/`. The cost audit is `cost/R4-native-budget-status.txt`. Those records support prototype fabrication review; they do not certify a production or loaded-arm system.
