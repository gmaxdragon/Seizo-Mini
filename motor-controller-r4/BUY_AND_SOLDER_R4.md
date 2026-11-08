# Seizo motor controller R4: buy and solder

This is the five-driver prototype carrier. It uses one USB-C data cable to a Pi USB-A port and a separate owned 12 V / 4 A motor supply. The old CNC shield is unchanged.

**Ordering status:** the current planning total is **$98.84 before unquoted import or brokerage fees**, including estimated shipping/tax and a $2 heatsink reserve. It is not a guaranteed delivered price. Do not place orders whose combined final charges exceed $100. Manufacturer engineering approval and physical qualification remain outstanding; this document does not call the controller production-ready.

## Buy these exact parts

For West3D, buy **5 x BIGTREETECH TMC2209 driver, SKU BTT0529** at the observed $3.99 each ($19.95 total). The design requires the reviewed **V1.3 / R110** board. The product page does not explicitly guarantee the shipped board revision or five included heatsinks, so confirm those before relying on the order. Do not choose a different driver just because its pins appear similar.

For Waveshare, buy these three items:

| Quantity | Exact item | SKU | Observed item total |
|---:|---|---|---:|
| 1 | RP2040-Zero-M, with male headers already soldered | 20335 | $4.99 |
| 1 | Fan-4010-PWM-12V, with its screw pack | 19193 | $2.69 |
| 1 | USB-A-TO-USB-C-Cable-1M data cable | 35944 | $1.19 |

The three project items total **$8.87**. Leave any unrelated item already in the shared cart out of this project total. All quoted shared shipping has been allocated to this project.

For LCSC, import `cost/R4-LCSC-import.csv` or enter all 17 lines below. The buy quantity includes required minimum packs; **do not solder all the spares**.

| LCSC number | Buy quantity | Exact manufacturer part | Used on this controller |
|---|---:|---|---|
| C6757 | 1 | SN74HC541DWR | U7 |
| C19653 | 1 | TPS3808G01DBVR | U8 |
| C1519255 | 50 | MFR-25FBF52-10K | R11 R12 R13 R14 R15 R6 R7 R9 R21 R31 R22 R32 R23 R33 R24 R34 R25 R35 |
| C172978 | 50 | MFR-25FTE52-1K | R2 R3 R4 R8 |
| C524651 | 5 | RDER71H104K0S1H03A | C2 C3 |
| C109392 | 10 | 35ZLH100MEFC6.3X11 | C11 C12 C13 C14 C15 |
| C178708 | 1 | EEU-FR1H471 | C1 |
| C2915641 | 5 | DB128V-5.08-4P-GN-S | J1 J2 J3 J4 J5 |
| C3096093 | 1 | PJ-102AH | J6 |
| C124375 | 20 | B-2100S02P-A110 | J7 J9 |
| C240840 | 5 | 470531000 | J8 |
| C179006 | 5 | 0454004.MR | F1 |
| C179003 | 5 | 0454.500MR | F2 |
| C3719503 | 1 | SMCJ15A | D2 |
| C506560 | 1 | SB560-E3/54 | D1 |
| C145202 | 5 | 1N5819-E3/54 | D3 |
| C27438 | 10 | 2.54-1*8P | Ten driver socket rows at U1-U5 |

The current LCSC merchandise quote is **$15.59**. Keeping that order above $15 avoids the published small-order handling-fee threshold. Recheck checkout instead of assuming quoted shipping includes import charges.

For JLCPCB, upload **Seizo_R4_Gerbers_Prototype.zip** and order **one batch of five bare boards**: 100 x 100 mm, two layers, 1.6 mm FR-4, green solder mask, white silkscreen, 1 oz copper, LeadFree HASL, tented vias and electrical test. The observed web price is **$5.30**, plus **$9.20 Global Standard Direct Line** shipping. Populate only one board; the other four are bare spares. No factory assembly or stencil is included.

## Reuse rather than buy

- The approved owned regulated 12 V / 4 A adapter, only after its label, center-positive polarity and nominal 5.5/2.1 mm plug are checked.
- Two short owned insulated jumper wires for J9 to the Zero RESET switch contacts.
- One owned insulated jumper for the J7 bench link. This removes the earlier unselected $0.20 item; do not buy it again.
- A clipped tinned resistor lead for R5. Keep it clear of nearby copper.
- Owned printing material for an insulated board support and fan mount/duct. Use at least 5 mm underside clearance and the real asymmetric mounting-hole map.

The **$2 heatsink/adhesive reserve is still not an exact quoted purchase**. Confirm whether all five driver modules include suitable heatsinks and their intended interface. If they do not, a compatible exact part must be selected and the total repriced before buying. Do not silently remove this reserve.

Motors, new motor cables, soldering tools, solder, flux, cleaning supplies, a working external multimeter and any bench supply are not charged to this controller budget. Motor leads must be properly rated cable, not loose Dupont jumpers. Tools are separately funded; borrow a working multimeter if necessary before first power.

## What you actually solder

There are **58 separate items and 232 solder joints on the carrier**: **200 through-hole joints** and **32 SMD joints**. The five SMD parts are U7, U8, F1, F2 and D2. This is more than header-only assembly; get help with the small chips if needed.

Use the actual front/back assembly maps in `assembly/` and search KiCad by the exact references below. All soldering happens with **both USB and motor power disconnected**.

**Before U7:** its pads **7, 9, 16, 18 and 19** contain ordinary escape vias in frozen R4. Fill those holes with solder and wick the lands flat before fitting the chip. Check for bridges. This is not factory filled/capped-via assembly.

| Order | KiCad references | Fit quantity | Exact part | Solder joints | What to do |
|---:|---|---:|---|---:|---|
| 1 | U7 | 1 | SN74HC541DWR | 20 | Place only after the five escape vias below are filled and wicked flat. Match pin 1; solder all 20 leads without bridging them. |
| 2 | U8 | 1 | TPS3808G01DBVR | 6 | Match pin 1. All six leads are soldered, including the NC pin 4 land. Do not bridge neighboring pins. |
| 3 | F1 | 1 | 0454004.MR | 2 | Fit the 4 A fuse here. No polarity; do not replace it with a wire. |
| 4 | F2 | 1 | 0454.500MR | 2 | Fit the 0.5 A fan fuse here. No polarity; do not swap it with F1. |
| 5 | D2 | 1 | SMCJ15A | 2 | Back side of PCB. Cathode/band goes to pad 1, VMOT. Use the back assembly map; avoid reversing it. |
| 6 | R11 R12 R13 R14 R15 R6 R7 R9 R21 R31 R22 R32 R23 R33 R24 R34 R25 R35 | 18 | MFR-25FBF52-10K | 36 | 18 actual 10 kOhm resistors. No polarity; ignore the number of spare resistors in the purchase bag. |
| 7 | R2 R3 R4 R8 | 4 | MFR-25FTE52-1K | 8 | Four actual 1 kOhm resistors. No polarity. R4 is a resistor; R5 is the link below. |
| 8 | R5 | 1 | Assembly link - tinned resistor-lead offcut | 2 | Form a clipped tinned resistor lead across the two holes. It is a required wire link, not a 1 kOhm resistor. |
| 9 | D1 | 1 | SB560-E3/54 | 2 | Cathode/band goes to pad 1, VMOT. Leave body about 3-5 mm above the PCB; 25.4 mm formed lead spacing. |
| 10 | D3 | 1 | 1N5819-E3/54 | 2 | Cathode/band goes to pad 1, FAN_VMOT. This is the smaller fan-branch diode. |
| 11 | C2 C3 | 2 | RDER71H104K0S1H03A | 4 | Two 100 nF ceramic capacitors; no polarity. Do not fit the electrolytics in these positions. |
| 12 | C11 C12 C13 C14 C15 | 5 | 35ZLH100MEFC6.3X11 | 10 | Five 100 uF / 35 V capacitors. Positive lead to pad 1/VMOT; striped negative side to pad 2/GND. |
| 13 | C1 | 1 | EEU-FR1H471 | 2 | 470 uF / 50 V bulk capacitor. Positive to pad 1/VMOT, striped negative to pad 2/GND. Keep body/leads clear of warm D1. |
| 14 | U1 U2 U3 U4 U5 | 10 | 2.54-1*8P | 80 | Solder TEN female 1x8 sockets here: two rows per module. Keep straight and fully seated. Do not solder five driver modules directly to the carrier. |
| 15 | J1 J2 J3 J4 J5 | 5 | DB128V-5.08-4P-GN-S | 20 | Five four-position motor screw terminals. Fit each wire-entry side as shown on the front assembly map. |
| 16 | J6 | 1 | PJ-102AH | 3 | Fit the exact PJ-102AH barrel jack. Its shape/pad pattern determines orientation; do not substitute a similar-looking jack. |
| 17 | J7 J9 | 2 | B-2100S02P-A110 | 4 | Two vertical two-pin male headers. J7 is HOLD. J9 is the mandatory RUN/GND harness. Neither is a motor-power connector. |
| 18 | J8 | 1 | 470531000 | 4 | Keyed four-pin PC fan header. The fan is pin 1 GND, pin 2 protected 12 V; pins 3/4 are unused. |
| 19 | U6 | 1 | RP2040-Zero-M | 23 | Program the bare RP2040-Zero-M first. Then solder its 23 supplied male pins into the carrier. Pin 23 is electrically unused but still mechanically soldered. Do not connect motor power to it. |

Solder the unused IC leads and unused module/header pads to their own lands too; **unused does not mean bridge them together**. The table includes those mechanical solder joints.

## Two extra reset wires

1. Program the bare Zero with the supplied locked UF2 before fitting the reset harness. No motor is attached.
2. With all power removed, identify the Zero **RESET / Key2** switch using its official schematic and continuity measurements. **Key2.1 is RUN; Key2.2 is GND.** RUN is not on the 23 edge-header pins. Do not use the BOOT / Key1 switch.
3. Connect **J9.1 to the verified RUN contact**, and **J9.2 to the verified GND contact**. Keep the insulated wires together, away from motor-phase cables, with strain relief. Do not guess left/right switch feet or put 5 V / 12 V on either wire.
4. Check continuity to the intended contacts and absence of a bridge. These are four wire terminations in addition to the carrier joints; owned female jumper ends may plug onto J9, so not all four need separate soldering.

With that harness fitted and motor power off, the controller intentionally stays in reset and does not appear as a USB serial port. Motor power must return steadily before it boots. Omitting the harness removes the new protection.

## Parts you plug in after inspection

- Plug the five exact TMC2209 V1.3 modules into their socket pairs **only with both supplies off and VMOT discharged**. Match EN to EN and VM to VM; one-pin offset or reversal can destroy a module.
- Fit five suitable module heatsinks and mount the 12 V fan to blow across all five drivers and D1. Check fan operation before motor heating tests.
- The fan plugs into J8. Use the correct keyed direction: pin 1 GND, pin 2 protected fan power; pins 3/4 are unused.
- Link J7 only as the intentional bench HOLD link, or attach an isolated dry normally-closed contact. Do not apply voltage there. It is not a certified emergency-stop system.
- Attach a motor only after the no-motor tests. The terminal order is **1 A1, 2 A2, 3 B1, 4 B2**. Determine winding pairs with an unpowered multimeter test; motor-wire colors are not universal.

## Before this can operate an arm

Follow `ASSEMBLY_AND_FIRST_POWER.md` for the no-driver, no-motor and later unloaded-motor tests. The supplied firmware refuses ARM/MOVE until commissioning is deliberately unlocked after the required checks. Actual motor winding currents, power stability, temperatures, UART communication and regenerated voltage still need physical tests.

The starting shared input envelope is at most 2.5 A / 30 W; it is not an electronic current clamp or tested continuous rating. NEMA 23 describes physical size, not a guaranteed compatible current or available torque. A power loss or reset can release holding torque, so an unbraked arm can fall. Keep loads supported. Digital checks do not make that mechanical hazard disappear.

**Order only when the exact delivered driver revision, heatsink contents, assembly method and combined final landed charges are resolved. The current package supports prototype fabrication review; it does not certify a ship-ready finished product.**
