# R4.1 connections, orientation and soldering

These instructions apply only to the exact reviewed **BIGTREETECH TMC2209 V1.3 with R110 sense resistors** and **genuine Waveshare RP2040-Zero (20187) with the specified added header pins**. A generic StepStick/TMC2209 or similar-looking board is not automatically pin compatible. Verify the delivered revision, shunts, markings, headers and current-sense resistors against the primary manufacturer documentation before fitting it.

## Front view: module direction

With the native board shown from the front, every driver footprint has pad 1 at its upper left and pad 16 at its upper right. Match the module's **ENN/EN contact to pad 1** and **VMOT/VM contact to pad 16**, and verify the nearby GND contacts. Do not use the adjustment-potentiometer position alone to choose a direction. The two socket rows are12.70 mm apart, with2.54 mm pin pitch. Fit ten upright1×8 female socket rows, two per footprint; keep all rows even though only three driver modules are fitted.

```text
Native socket, FRONT view (not viewed through the PCB):

  1 ENN       16 VMOT
  2 MS1/AD0   15 GND
  3 MS2/AD1   14 A2
  4 UART      13 A1
  5 TX_NC     12 B1
  6 SPREAD    11 B2
  7 STEP      10 VIO (3.3 V)
  8 DIR        9 GND
```

The numbers are the **custom KiCad logical pad numbers**; do not assume they are printed on the purchased module. Use its actual pin names and unpowered continuity. TX_NC is deliberately unconnected. SPREAD is grounded. MS1/MS2 are strapped for the reviewed UART address; do not change them based on a generic microstepping tutorial. The exact per-slot native nets and coordinates are in [R41-pad-map.csv](R41-pad-map.csv).

| Slot | Module fitted now | UART bus/address | MS1 / MS2 | Motor output |
|---|---|---|---|---|
| U1 | Yes | A / 0 | GND / GND | J1, NEMA 23-1 |
| U2 | Yes | A / 1 | 3.3 V / GND | J2, NEMA 23-2 |
| U3 | Yes | A / 2 | GND / 3.3 V | J3, pancake |
| U4 | No | A / 3 | 3.3 V / 3.3 V | J4, future |
| U5 | No | B / 0 | GND / GND | J5, future |

Leave U4/U5 modules absent. The locked R4.1 firmware uses populated mask 7 and keeps those future channels inactive. The existing CNC shield stays separate for the three full-size NEMA 17 axes.

## External connections

| Connector/pin | Connection |
|---|---|
| Zero USB-C | Pi USB-A to USB-C **data** cable; logic power and serial commands |
| J6 | Regulated12 V DC, center positive; exact PJ-102AH jack, nominal5.5 mm OD/2.1 mm ID plug. Verify actual adapter label, polarity and fit |
| J1/J2/J3 pins 1,2,3,4 | A1, A2, B1, B2 respectively. Identify complete coil pairs while disconnected; wire colors are not standardized |
| J4/J5 | Same coil order, but no driver module and no active motor in this variant |
| J7.1 / J7.2 | GND / HOLD_CONTACT. Dry normally closed contact or intentional bench loop only; no external voltage |
| J8 pins 1,2,3,4 | GND, protected12 V fan supply, unused tach, unused PWM |
| J9.1 / J9.2 | VMOT_RESET_N to verified Zero Key2.1 RUN / Key2.2 GND. RUN is not an edge-header pin |

Motor terminals are upright DORABO5.08 mm screw terminals, not ordinary Dupont signal headers. Do not hot-plug motors or modules. Do not parallel power-adapter outputs or share the reused adapter with CNC loads without checking combined demand and distribution.

## Parts that are easy to reverse or confuse

- **U7/U8:** hand fitted in the active manual plan. Match pin1 to the native pad map and inspect all 26 joints. R4.1 has no SMT-land/drill overlaps in the frozen nominal audit. The older TOP2 factory option remains separate.
- **F1/F2:** manually fit the 4 A and 0.5 A fuses in their correct positions. Neither is a wire link. Do not bypass them.
- **R5:** a required fitted tinned wire link, made from a clipped resistor lead. **R4 remains 1 kΩ; R5 is not 1 kΩ.**
- **D1:** SB560-E3/54 band/cathode to pad 1/VMOT. Leave the body about 3–5 mm above the PCB for airflow; formed lead pitch 25.4 mm.
- **D3:** 1N5819-E3/54 band/cathode to pad 1/FAN_VMOT.
- **D2:** SMCJ15A is on the **back**; cathode/band to pad 1/VMOT.
- **C1 and C11–C15:** positive to pad 1/VMOT, negative stripe to pad 2/GND. Keep C1's body and leads clear of warm D1.
- **C2/C3:** 100 nF ceramics, nonpolarized. R6/R7/R9 are 10 kΩ; R8 is 1 kΩ. Wrong divider values change the reset threshold.
- **U6:** program the bare Zero with the supplied locked three-driver UF2 before directly soldering its 23 male pins. VSYS is intentionally unused. Do not connect motor power to it.

Use [Solder_List.csv](../cost/Solder_List.csv) for exact fitted quantities and joints. The active build has 230 carrier joints plus 23 extra MCU header joints and separate reset-harness terminations. J8 is not fitted as a plastic header; solder the blower black wire to J8.1 and red wire to J8.2 with strain relief, leaving pads3/4 unused. Plug-in modules are inserted after inspection with all power disconnected; they are not soldered directly to the carrier.

## Reading the drawings

[Front placement reference](placement-reference-front.pdf) and [back placement reference](placement-reference-back.pdf) are footprint/pad views reused from R4 because the independent R4.1 audit verified all 53 component footprints, positions, rotations, values, pad geometry and nets unchanged. They are **placement references**, not drawings of the changed R4.1 copper/silkscreen. Use the current native PCB and [native plot](../preview/final-board.svg) for the new routing and labels. Back views are mirrored; do not transfer left/right positions between views.

The 100 × 100 mm board has asymmetric 3.2 mm mounting holes at (4,4), (96,4), (96,96), (4,49.1) mm from the front top-left outline corner. Leave clearance under rear parts and solder joints. Neither drawing proves real socket, module, heatsink, cable or fan fit.

Primary references: [BTT TMC2209 V1.3 source documentation](https://github.com/bigtreetech/BIGTREETECH-Stepper-Motor-Driver/tree/master/TMC2209/V1.3), [Waveshare RP2040-Zero documentation](https://www.waveshare.com/wiki/RP2040-Zero). Use the actual delivered revision rather than an unrelated module diagram.
