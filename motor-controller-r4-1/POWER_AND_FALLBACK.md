# Current power and NEMA23-only fallback review

## Main arrangement

Use U1/U2 for two electrically suitable NEMA23 motors. Keep U3 fitted with a TMC2209 but no motor for the supplied mask7 self-check; U4/U5 remain empty. The existing CNC/Uno drives three NEMA17 axes with its owned A4988s and keeps its existing USB connection. One USB cable refers to the new controller, not the entire two-controller arm. Four CNC driver modules do not establish four independent GRBL axes.

Do not put A4988 modules in R4.1: its pin6 is grounded SPREAD for TMC, corresponding to SLEEP on an A4988; the UART/reset/microstep mapping and firmware also differ. It requires the exact reviewed TMC2209 V1.3/R110 arrangement.

## Owned12V4A source

The nameplate ceiling is12V x4A =48W at its output. Buck/boost converters cannot increase the input power available and have losses. Do not raise this12V-only carrier to24V or parallel adapter outputs. Its initial total carrier-input qualification envelope is<=2.5A/30W, which is neither an electronic current clamp nor an established continuous rating. The blower alone uses up to0.179A/~2.15W at nominal12V before diode/driver losses.

A4988/TMC winding current is not the same as adapter input current; neither adding frame sizes nor summing nameplate phase currents proves adapter demand. Motors can consume power while holding position. Actual winding resistance/current, configured current limits, speed, acceleration and load determine consumption and torque. The user's successful prior test of four existing drivers is recorded as a user report; it does not qualify the new board, different NEMA23 motors or a full gravity-loaded arm.

One shared adapter requires independently protected, rated branch wiring and measured combined demand. Do not put the CNC loads through this carrier's jack/copper or use a signal jumper/breadboard for motor power. No new distribution harness or combined six/five-axis load is cleared by this review. If adequate current settings cannot produce required torque without exceeding measured supply/thermal limits, use suitable different drivers/power or reduce the mechanical demand; a converter is not the remedy.

## Budgeted conditional fallback — NEMA23 only

Two owned enclosed generic9-42V drivers have0USD new purchase cost. A **5USD adaptation-material allowance** is included separately in the budget. It is an allowance, not a quote or a promise that all needed wiring can be obtained for that amount.

Possible architecture: Pi USB -> owned Uno -> verified open-collector STEP/DIR interface -> two owned enclosed drivers -> suitable NEMA23 motors. Motor power uses an independent correctly fused/rated harness and bypasses a suspect carrier. In this fallback, take the CNC/NEMA17 loads out of service; it is not a backup for all five axes simultaneously. Reuse only checked, undamaged hardware. Do not remove power from an unsupported arm.

The unidentified drivers'9-42V label and current table resemble documented TB6600-style units, but do not identify their manufacturer/chip or prove their input requirements. Verify input voltage/current, timing, enable polarity/default state and motor-current settings before designing/wiring the interface. The existing motion-locked RP2040 firmware requires TMC UART and is NOT firmware for these generic external drivers. The Uno fallback needs separately reviewed compatible control firmware and unloaded testing. No fallback was wired, powered or tested here.

Use the existing spare bare carriers for a damaged PCB and socketed modules for replaceable drivers, after finding the fault. Solder bridges/opens can often be repaired unpowered, but wrong power or orientation can destroy parts before a fuse opens. Do not jumper over damaged power copper or bypass protection. Obtain experienced soldering help for U7/U8 if required. HOLD is not a brake or independent emergency stop; the arm is known to sag/drop on power loss.

## Concrete limits remaining

Motor models, actual current settings, torque, combined draw, adapter regulation/plug/polarity, startup/regeneration and thermal behavior remain unmeasured. Cooling parts are selected, but delivered fit/adhesion and blower placement still need inspection and testing. Current circuitry and power cannot be guaranteed compatible with arbitrary NEMA23s. No order, physical motion or software unlock was performed.

Primary references: [TMC2209](https://www.analog.com/en/products/tmc2209.html), [BTT reference](https://github.com/bigtreetech/docs/blob/master/docs/TMC2209.md), [A4988 operation/current distinction](https://www.pololu.com/product/1182), [DFRobotDRI0043 example](https://wiki.dfrobot.com/dri0043). The DFRobot manual documents only that model, not the user's unidentified unit. [3M8810 manufacturer data](https://multimedia.3m.com/mws/media/122119O/3m-thermally-conductive-adhesive-transfer-tape-88xx-series-tds.pdf) and [Adafruit1467](https://www.adafruit.com/product/1467) support the chosen thermal adhesive.
