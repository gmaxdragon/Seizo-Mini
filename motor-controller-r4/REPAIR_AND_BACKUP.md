# Repairing R4 and keeping a backup

R4 can be repaired in many ordinary cases. It is not solder-error-proof. A solder bridge that connects 12 V to a logic pin can permanently damage a driver, the RP2040 module or a connected computer before the 4 A fuse opens. A fuse is not protection against every wrong connection.

## Repair plan

Disconnect both USB and motor power before any soldering, wiring, module changes or inspection by touch. Measure VMOT below 1 V before handling the board. Follow `ASSEMBLY_AND_FIRST_POWER.md`; do not skip to plugging in motors.

| Problem | Practical response |
|---|---|
| Unwanted solder bridge | With all power removed, use flux and solder wick to remove the excess. Inspect under magnification and check continuity to the intended nets. Do not power it repeatedly to see whether the bridge disappears. |
| Open or poorly wetted joint | Rework that joint with suitable flux and inspect the component lead and pad. A matte lead-free joint alone does not prove a fault. |
| Wrong resistor, reversed diode or capacitor | Correct it before power. If a polarized part was powered backwards, replace the affected part rather than assuming its appearance proves it survived. |
| U7 solder wicking into its escape vias | Before fitting U7, fill and wick flat the holes at pads **7, 9, 16, 18 and 19**. Recheck for bridges. This manual operation remains necessary in the current low-cost fabrication option. |
| Blown F1 or F2 | Find the short or overload first, then replace with the exact specified fuse. Never replace a fuse with wire or a larger rating. |
| One defective driver module | Check the carrier rails and socket first. After both supplies are off and discharged, replace the socketed module. Five modules are being bought for five channels, so a sixth spare module is not included. |
| Lifted pad, scorched board or damaged power copper | Stop. A verified small-signal jumper repair may be possible, but a beginner should use a spare bare PCB for damaged motor-power copper rather than guess at a wire repair. Rebuilding the same design does not cure an underlying design error; diagnose that first. |
| Damaged RP2040-Zero | The current BOM solders its 23 supplied header pins directly into the carrier. It is **not** a plug-in controller module. Removing it needs careful desoldering; do not pull against attached pads. Female sockets would improve replacement access but are not currently purchased or mechanically qualified. |

The initial tests deliberately leave the expensive driver modules out. Check the bare carrier, its rails, reset harness and fan first. Then fit the drivers with no motors, check UART/configuration, and only later test one supported unloaded motor. The supplied firmware remains movement-locked during these checks.

## Reduce damage from an assembly mistake

1. Leave U6 and all five driver modules out while checking the carrier's soldering. With all power disconnected, compare the actual pad/net map to each suspect joint and check for a persistent supply-to-ground short. A continuity beep alone is not a complete test.
2. Before fitting U6, perform a no-module motor-supply check with a current-limited supply if available. Confirm the barrel polarity and protected motor/fan rails. Check that motor voltage has not appeared on the U6 3.3 V or GPIO pads through a bridge. U7/U8 can be present, but their logic supply is unpowered at this stage. Stop on unexpected voltage; do not insert a controller to see whether it survives.
3. Program the bare Zero separately. Once U6 is fitted, use a suitable standalone **5 V USB source** for initial logic-power checks, rather than connecting the Pi immediately. Use current limiting where available; a wall adapter or USB port is not proof of a low fault-current limit. Never put the 12 V adapter into USB.
4. Before connecting the Pi, complete the no-motor checks in the assembly guide with both supplies present. Confirm the Zero's USB supply remains nominally 5 V and its logic rail nominally 3.3 V. Unexpected voltage, excessive current, heat or odor means disconnect and investigate. These static checks reduce risk; they cannot prove absence of every intermittent fault or transient.
5. Power off, discharge, then fit the five driver modules. Qualify UART and each unloaded motor as described in the first-power guide. Keep the arm mechanically supported throughout later tests.

If a directly soldered Zero has been permanently damaged, an experienced repairer can sometimes sacrifice its header pins, remove the module, and desolder the remaining carrier pins individually. This can avoid heating all 23 joints at once. Do not try to pull a still-soldered module out, cut near live circuits, or assume the original carrier pads can always be saved. Using a spare bare board may be safer and cheaper.

Most small resistors, the 100 nF capacitors, 100 uF capacitors, fuses and two-pin headers are bought in packs, so some repair spares are already available. The bulk capacitor, ICs, motor terminals, barrel jack and RP2040 do **not** all have spare replacements in the current order. Tools, flux, solder wick and a working external multimeter are separately funded or borrowed.

## The backup already in the PCB order

The fabrication quote is for **five bare PCBs**. Only one is populated, leaving **four spare bare boards** at no additional order cost. This is the first backup if a carrier is damaged during assembly. Move only parts that have been checked; do not transfer a suspected shorted component onto the next board. Some connectors or ICs may need replacement rather than risky removal, so a complete second assembled controller is not included in the $100 plan.

The existing Uno/CNC-shield setup also remains unchanged. It can continue doing the jobs it already supports while R4 is repaired. This is a partial functional fallback, not a replacement for five new independent axes or a guarantee of NEMA 23 torque.

## Should we buy another cheap CNC shield?

**Not as an automatic five-axis backup.** A common Uno CNC Shield V3 has four driver sockets. Standard Uno GRBL normally controls three independent axes; the A socket commonly duplicates another axis. Some shields expose a custom fourth-axis option, but that still does not provide five independent axes. [Makerfabs' shield description](https://www.makerfabs.com/arduino-cnc-shield-v3.html) lists four A4988 sockets and the clone/custom D12-D13 choice. Its observed bare-shield price is **$1.90 before shipping/tax**, not a complete backup-system price.

A generic A4988/DRV8825 shield is not a verified plug-in substitute for R4's exact TMC2209 UART wiring. Review its real schematic, socket pin assignments, microstep/UART jumpers, EN wiring and logic supply before considering a conversion. Do not connect a shield's 5 V logic to RP2040 3.3 V inputs. Do not tie the five RP2040 enable outputs together because a shield shares an enable net.

## A possible commercial-carrier fallback for later

The manufacturer-documented **BTT EXP-MOT V1.0** has **three replaceable driver slots**, 3.3 V or 5 V logic selection, 12 V/24 V motor input, and STEP/DIR/UART modes. Thus **two carriers** have enough physical sockets for five reusable TMC modules. [Official BTT manual](https://raw.githubusercontent.com/bigtreetech/BTT-Expansion-module/master/BTT%20EXP-MOT/BTT%20EXP-MOT%20V1.0User%20Guide.pdf).

That is a candidate, not a purchased or validated drop-in backup. To reuse the Zero, it would need two actual compatible carriers, the correct GPIO/UART connector harness, suitable motor-power distribution/protection, cooling, a reviewed pin mapping and firmware configuration, and new electrical qualification. It would not automatically retain R4's hardware HOLD gate or motor-rail reset monitor. A current complete delivered quote was not established, so **no price is invented and these boards are not included in the final carts**.

For now, keep the spare bare PCBs and existing CNC setup. Do not add an unverified second controller to a tight $100 order merely to call it a backup.

## Limits that apply to every backup

Never hot-plug motors or modules. Even common A4988 carrier documentation warns that powered motor disconnection can destroy a driver, and that wiring-induced supply spikes can exceed component limits at a nominal 12 V. [Pololu carrier guidance](https://www.pololu.com/product/1182). Keep this backup's voltage, current, capacitor and wiring choices specific to its actual driver and carrier; do not copy the A4988 limits onto TMC2209.

No alternative carrier increases a TMC2209's current or the owned adapter's power. Actual motor winding ratings, torque, cooling and regenerated voltage still need checks. Power loss can release a loaded arm, so mechanical support or braking remains necessary independently of the PCB repair plan.
