# R4.1: exact manual purchase and assembly option

Use the R4.1 schematic, PCB and front/back assembly maps. Do not use the original R4 instructions to fill vias below U7: **R4.1 moved those vias away from the solder pads**. There is no factory fitting in this plan.

## Buy

1. Import `cost/LCSC_import.csv` into LCSC. Map **LCSC Part Number** to part number and **Quantity** to purchase quantity. It contains exactly 20 unique part numbers. Check every result, quantity and stock indicator before checkout. The November 8 unpaid cart showed $27.97 goods and $13.85 freight, with a $0.28 automatic discount. The planning budget ignores the discount.
2. Buy **three** West3D BTT0529 driver modules. The carrier requires the reviewed **BIGTREETECH TMC2209 V1.3 with R110 sense resistors** and the documented shunt configuration. Confirm the delivered modules against their board markings before installation. A similarly named module is not automatically a compatible substitute. The current listing does not explicitly promise all revision, heatsink and adhesive details; the BOM retains a $2 cooling-accessory reserve.
3. Order **one batch of five bare R4.1 PCBs** from the matching Gerber archive. Specification: 100×100mm, two layers, 1.6mm FR-4, 1oz copper, green solder mask, white silkscreen, LeadFree HASL, tented vias and electrical test. The November 8 saved unpaid quote shows $5.30 boards + $9.20 Global Standard shipping − a $5.30 automatic PCB coupon = $9.20 displayed. The budget ignores the coupon and reserves $14.50. No address was entered and final destination charges remain unconfirmed. Assemble one; four are bare spares. No assembly or stencil is included.
4. Reuse the approved owned regulated 12V/4A adapter, owned insulated jumper wires and printing material. Verify the adapter label, center-positive polarity and nominal 5.5/2.1mm plug before connecting it. USB does not power the motor rail.

The estimated budget is $98.94 with reserves; see `cost/READ_FIRST.md`. The actual supplier checkout totals and any owed import/tax charges govern purchasing. No order or payment has been placed by this plan.

## Assemble with all power disconnected

Keep both USB and motor power disconnected throughout soldering. Use the references in `cost/Solder_List.csv`; spare parts from the purchase packs are not all fitted.

1. Fit **U7 SN74HC541DWR**, matching pin1 on the front assembly map. Solder all 20 leads. Fit **U8 TPS3808G01DBVR**, matching pin1, and solder all six leads, including its NC land. Inspect under magnification for bridges, lifted leads and incomplete joints. R4.1 needs no old R4 via-filling step.
2. Fit **F1 0454004.MR (4A)** and **F2 0454.500MR (0.5A)**. They are not polarized. Do not swap them or replace either with a wire.
3. Fit back-side **D2 SMCJ15A**. Its cathode band goes to pad1/VMOT. Follow the back assembly map, accounting for the flipped view.
4. Fit the 18 actual **10kΩ** resistors and four **1kΩ** resistors listed in the solder CSV. Keep **R4=1kΩ**. Fit **R5 as a clipped tinned resistor-lead wire link** across its holes; do not fit a 1kΩ resistor there.
5. Fit **D1 SB560-E3/54**, band to pad1/VMOT. Form its leads to the reviewed 25.4mm spacing and keep the body roughly 3–5mm above the board. Fit smaller **D3 1N5819-E3/54**, band to pad1/FAN_VMOT.
6. Fit **C2/C3**, two nonpolarized 100nF ceramic capacitors. Fit **C11–C15**, five 100µF/35V capacitors, positive lead to pad1/VMOT and striped negative side to pad2/GND. Fit **C1**, 470µF/50V, with the same positive/negative rule. Keep C1 clear of warm D1.
7. Fit **ten straight female 1×8 socket rows** at U1–U5, two rows per slot. Keep them upright, aligned and fully seated. The five physical slots all receive sockets, but **only U1/U2/U3 receive driver modules**. Do not solder the driver modules themselves to the carrier.
8. Fit the five exact DORABO four-position motor screw terminals at **J1–J5**, matching the wire-entry direction on the front assembly map. Fit **J6 PJ-102AH**. Fit vertical two-pin headers at **J7 and J9**. Do not substitute ordinary Dupont pins for the motor terminals.
9. **Do not fit J8's plastic fan header.** The BOM excludes C240840. Leave J8 pads3/4 unused.

## Fit the bare MCU

The purchased MCU is **genuine Waveshare RP2040-Zero, bare board (20187), LCSC C5350143**. The old preheadered item was 20335/Zero-M. This is the same reviewed Zero board geometry and signal map; headers are added manually.

1. Cut **9 + 9 + 5** pins from a ZHOURI C2977586 40-pin strip. One strip is enough; the five-strip purchase is its minimum pack. Keep all rows straight using an unpowered alignment fixture.
2. Fit the short 3mm tails through the Zero, with the plastic below it and the long 6mm pins pointing toward the carrier. Solder the **23 module-side joints**, checking that the USB connector and buttons remain accessible.
3. Program the separate module with the supplied locked R4.1 three-driver UF2 before attaching motor power. The firmware remains locked for commissioning; loading it is not permission to move an arm.
4. Insert its long pins through the carrier in the reviewed orientation and solder **23 carrier joints**. Even the electrically unused edge pin is mechanically soldered to its own land. Do not bridge unused pads together.

## Fit the reset harness and HOLD bench link

1. With all power removed, identify the Zero's **RESET/Key2** contacts from the official schematic and unpowered continuity measurements. **Key2.1 is RUN; Key2.2 is GND.** Do not confuse it with BOOT/Key1. RUN is not one of the 23 edge-header pins.
2. Connect **J9.1 to verified RUN**, and **J9.2 to verified GND**, using two short insulated owned wires with strain relief. Owned female jumper ends may plug onto J9. Solder the other ends only to the verified Key2 contacts. Do not guess contacts from left/right appearance or apply 5V/12V to either wire.
3. Use the owned insulated J7 bench link only when intentionally performing the documented bench checks. An external J7 contact must be a dry normally-closed contact with no applied voltage. HOLD is not an independent emergency-stop circuit.

With the harness fitted, motor power off deliberately keeps the MCU in reset, so it may not enumerate as a USB serial port until steady motor power returns. Omitting the harness removes that protection.

## Mount and connect the blower

Use **SUNON MF50151VX-1B00C-A99 / C49308057**. It is a two-wire 12V blower with a nominal **51.7×51.6×15mm** envelope and two **4.5mm** mounting holes. Its side outlet and mounting pattern differ from a 40mm square fan. Make a printed bracket/duct from owned material that retains it, keeps the inlet clear and directs the outlet across the fitted driver heatsinks and D1. Use the manufacturer's dimension drawing for the hole positions. Do not assume a screw pack is included; any new mounting hardware must be added to the budget.

1. Trim only the stripped ends as necessary. Solder **black to J8.1/GND**, **red to J8.2/FAN_VMOT**. Keep insulation near the board and inspect for bridges to pads3/4.
2. Add a separate printed wire clamp/strain relief so a cable pull does not load the solder joints. Protect the underside joints with insulated mounting clearance.
3. Keep **F2 and D3** fitted. The fan is rated 12V, operates over 4.5–13.8V and draws up to **0.179A** according to its manufacturer specification. Include that current in the shared supply budget.
4. Supply continuous DC. **Do not PWM its supply or ground.** This model has no PWM or tach connection to fit.

## Before any motor moves

There are **230 carrier solder joints**, plus **23 added MCU header joints** and the separate required reset-harness terminations. The small-chip work is part of the actual build; obtain experienced hands-on help if the joints cannot be inspected or made reliably.

Use the package's full first-power/commissioning procedure: unpowered polarity and short checks, no-driver power checks, fan and reset/HOLD checks, then one unloaded supported motor at a time. Plug or remove drivers and motor wires only with both supplies off and the motor rail discharged. Match the exact driver orientation: ENN to logical socket pad1, VMOT to pad16; avoid a one-pin offset. Fit verified compatible heatsinks/thermal interface before loaded thermal tests. Motor terminals are **1=A1, 2=A2, 3=B1, 4=B2**; identify winding pairs with an unpowered multimeter test rather than wire colors. Use rated motor cable, not loose signal jumpers.

Adapter regulation, polarity, plug fit, exact motor winding current, driver current setting, fan startup, rail transients and temperatures still need physical validation. The initial total-input qualification envelope is at most **2.5A/30W**, not an electronic current clamp or a certified continuous board rating. Support the mechanism because power loss/reset can remove motor holding torque. This purchase plan does not establish that the same adapter can simultaneously power the separate CNC controller or that arbitrary NEMA23 motors are compatible.

Primary part/drawing references: [genuine MCU](https://www.lcsc.com/product-detail/C5350143.html), [headers](https://www.lcsc.com/product-detail/C2977586.html), [USB data cable](https://www.lcsc.com/product-detail/C430908.html), [blower](https://www.lcsc.com/product-detail/C49308057.html), [SUNON drawing](https://datasheet.lcsc.com/datasheet/pdf/ee5d2a9d0beb092f4b58af8906878919.pdf?productCode=C49308057).
