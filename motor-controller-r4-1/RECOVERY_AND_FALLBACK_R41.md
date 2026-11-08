# R4.1 repair and fallback

R4.1 uses three fitted driver modules in U1, U2 and U3. U4 and U5 are empty future slots. Its reference firmware expects `populated_mask:7` and `verified_mask:7`; the older five-driver firmware expects 31 and is not interchangeable.

## What can be repaired

Many ordinary assembly mistakes are repairable before power is applied. A solder bridge can often be removed, a poor joint reflowed, or a wrong resistor replaced. That does **not** make the circuit tolerant of arbitrary soldering errors. Applying power with a short, reversed part or misplaced driver can destroy a module, MCU, connector or copper trace before a fuse opens. The 4 A fuse does not guarantee protection from every assembly fault.

The stepper drivers plug into sockets, so a failed driver can be replaced after the cause is found. The RP2040-Zero's 23 header pins are soldered into the carrier; it is not a plug-out module. Replacing it requires desoldering or skilled rework. Small chips U7/U8 and the soldered fuses also require rework rather than simply pulling them out.

## Before touching a fault

1. Mechanically support the arm and any load. Removing power can release its holding torque.
2. Disconnect **both** the motor adapter and USB. A software HOLD or stopped program is not power isolation.
3. Use an external working multimeter to confirm VMOT is below 1 V before handling the board. Never short a capacitor to discharge it.
4. Photograph the assembly and label the motor connections before moving anything. Remove plug-in drivers only after the board is unpowered and discharged.

## Repair a simple soldering mistake

| Fault | Repair while completely unpowered | Check before trying power again |
|---|---|---|
| Solder joining adjacent pads | Add suitable flux, remove the extra solder with braid and clean the area as directed by the flux manufacturer. Use a controlled iron temperature and short contact times. | Inspect with magnification. Check the intended nets against the assembly map and multimeter; some neighboring pads intentionally share a net. |
| Poorly wetted or cracked joint | Apply flux and reflow the joint until both lead and pad are wetted. Let it cool without movement. | Confirm continuity to the intended net and inspect for new bridges. |
| Wrong resistor, diode, capacitor or orientation | Desolder and fit the exact specified part in the documented direction. | Recheck value, polarity and neighboring joints. A previously powered reversed component may need replacement even if it looks normal. |
| Crooked socket or connector | Desolder it properly before repositioning. Do not force a multi-pin part out while its pins remain attached. | Verify straightness, full seating, pad condition and every pin's connection. |
| Lifted pad, torn track, burnt board or uncertain damage | Stop. A confirmed same-net repair may be possible for someone experienced with PCB rework; a fresh spare bare board is often the more reliable option. | Do not improvise thin jumper repairs on motor-current paths. Never bridge a fuse or omit the reset/protection circuit to make the board run. |
| Suspected failed plug-in driver | Check the carrier for wrong voltage, shorts, orientation and damaged sockets before fitting a replacement. | Do not use another good driver as the first test of an unexplained carrier fault. Repeat the no-motor checks with current limiting. |

R4.1 moves the former five U7 escape vias outside the solder lands. Its frozen digital layout audit reports **no drill overlaps with any of the 32 SMT pads**, with a minimum via-annulus-to-U7-land gap of **0.175 mm**. The old R4 instruction to prefill U7 pads 7, 9, 16, 18 and 19 does **not** apply to this R4.1 board. Do not prefill those chip pads.

Under the **TOP2 assembly plan, U7 and U8 are factory fitted**, so you do not need to place or solder those two chips yourself. Inspect their factory joints and orientation using the current assembly drawings. Manufacturer CAM review and placement-rotation approval are still pending; the digital gap/overlap check is not factory acceptance. If a factory chip joint needs repair, get help if you cannot inspect and rework the small leads reliably.

After any repair, repeat the assembly guide's unpowered checks, then its no-driver and no-motor power checks. A meter continuity beep alone does not prove a rail is shorted because capacitors can charge during the measurement. Stop on unexpected current, heat, odor, sound or voltage; do not keep retrying power.

## A controller that does not appear over USB

With the required J9 reset harness fitted and motor power absent, the controller is intentionally held in reset. USB serial not appearing in that state is expected, rather than proof of a failed PCB. Follow the documented programming/service procedure and check the RUN/GND harness nets with all power removed. Do not bypass the supervisor or put 5 V/12 V on the reset wires.

With checked motor power present, the no-motor CHECK should verify only U1/U2/U3. U4/U5 being empty must not be treated as a fault by the R4.1 three-driver build. The supplied binary remains commissioning locked and refuses ARM/MOVE; that is intentional until the physical checks pass.

## Backup if the new carrier does not work

Keep the existing **Arduino Uno and CNC shield** unchanged and available for the current arm's three full-size NEMA 17 motors. That known arrangement is the immediate fallback while the new carrier is diagnosed. Stop and isolate both setups before transferring any motor wiring. A motor must be connected to only one driver at a time.

Another generic Uno CNC shield is not a complete replacement for the new three independently controlled channels without checking its exact revision, firmware and wiring. Some fourth sockets clone an existing axis by default. A second Uno/shield normally adds another USB controller connection and requires software support. A4988-style driver current limits also do not automatically suit the proposed NEMA 23 motors. Do not buy a second shield as a guaranteed universal backup.

If the new NEMA 23/pancake setup needs a different driver, first establish each motor's winding-current rating and the required mechanical torque. The existing black-and-green external drivers may be candidates only after their actual specifications, logic interface and supply needs are checked; they are not a blind pin-for-pin substitute. Never connect motor-coil wires directly to Pi GPIO, RP2040 GPIO or logic-only signal headers.

There is no need to buy another backup PCB immediately: the standard five-board fabrication batch leaves spare **bare** carriers. These are useful if a board is damaged, but they are not spare assembled controllers. Transfer or reuse only components that have been checked and remain undamaged.

## Limits that remain

Repairability, fuses, driver diagnostics and software locks improve commissioning; they do not guarantee that a soldering error is harmless or that the assembled controller is safe for a loaded arm. Actual motor ratings, power stability, cooling, regenerated voltage and mechanical protection still need qualification. Neither the carrier's HOLD input nor a generic CNC shield provides a certified emergency stop or a brake for gravity-loaded joints.
