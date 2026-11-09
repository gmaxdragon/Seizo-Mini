# Seizo Mini — R4.1 manual build

This is a three-driver motor-controller design: U1/U2 for suitably rated NEMA23 motors, U3 fitted as an unused-motor/spare channel in this plan, and U4/U5 empty future slots. The separate existing CNC shield stays unchanged. One USB-C data cable connects to a Pi USB-A port; a reused regulated 12 V adapter supplies the motor rail. Frame size alone does not prove motor compatibility.

The native PCB, schematic, manufacturing exports and locked firmware are unchanged from the reviewed R4.1 freeze. ERC and DRC report zero errors, warnings, unconnected nets and schematic mismatches. Firmware has 6,987 regression checks plus 651 three-driver checks; the new isolated Python host has 50 passing tests, ten against the actual firmware core. No real board or motor was tested.

## Current build option

**100USD maximum allocation; final landed charges unverified.** Use [Final_BOM.csv](cost/Final_BOM.csv), [Budget.csv](cost/Budget.csv), the [21-line LCSC importer](cost/LCSC_import.csv) and [cost explanation](cost/READ_FIRST.md). No coupon is required for the planning total. Actual final duties, taxes, accessory contents and checkout charges remain unconfirmed; $18.62 remains for unquoted charges after a5USD fallback adaptation allowance; it is not a liability ceiling. Motors/tools are excluded and the owned adapter is reused.

**All carrier parts are hand soldered**, including U7/U8. The genuine bare Waveshare Zero gets 23 added header pins. J8's plastic header is not purchased: the specified SUNON blower wires solder to protected pads1/2 and need a retained bracket/duct. Start with [BUY_AND_SOLDER_MANUAL.md](BUY_AND_SOLDER_MANUAL.md), then [ASSEMBLY_AND_FIRST_POWER.md](ASSEMBLY_AND_FIRST_POWER.md). The earlier factory option is preserved under cost/factory-alternative; it is not the active purchase plan.

The [Python host](host/) implements the USB protocol, but it is not deployed to the Pi or integrated into the existing GRBL/Uno web application. Firmware remains motion-locked. Exact motor current, gearing, direction and joint mapping are not measured.

## Files

- [Native project](project/big%20motor%20controller.kicad_pro), [schematic](project/big%20motor%20controller.kicad_sch), [PCB](project/big%20motor%20controller.kicad_pcb), and [Gerbers/drills](fabrication/).
- [Orientation/pin map](assembly/CONNECTIONS_AND_ORIENTATION.md), [recovery/fallback](RECOVERY_AND_FALLBACK_R41.md), [status](release-status.json), [submission file guide](SUBMISSION_FILES.md).
- [Cart evidence](cart-proof/): actual unpaid screenshots, cropped to omit personal details. They are price evidence rather than paid orders or final landed quotes.

The owned adapter is allocated to this carrier; combined power with the CNC controller has not been qualified. HOLD is not an independent emergency stop or brake. A gravity-loaded arm may fall on power loss. Physical soldering, protection, cooling, supply, USB/UART and regeneration tests remain before loaded-arm operation. This is a documented design prototype, not a production-qualified product.

Current intended arm: two qualifiedNEMA23 motors onU1/U2 plus threeNEMA17 motors on the separate ownedCNC. See [power and conditional NEMA23-only fallback](POWER_AND_FALLBACK.md). The ownedA4988s are not interchangeable with this carrier'sTMC sockets. Selected cooling is now in the detailed and purchase BOMs.
