# Seizo Mini — Motor Controller R4.1

This separate prototype revises R4 for **three fitted TMC2209 V1.3 drivers** in U1/U2/U3: two suitable NEMA 23 motors and one suitable NEMA 17 pancake motor. Frame size alone does not establish electrical or torque compatibility. All five native sockets and ten socket rows remain; U4/U5 driver modules stay absent. The existing CNC shield handles three full-size NEMA 17 motors separately.

R4.1 moves U7 escape vias away from its solder lands, adds clearer motor/future-slot and USB-C labels, and uses matching three-driver reference firmware. The through-hole motor terminals stand vertically. USB-C connects to a Pi USB-A port for control; a separate reused 12 V supply powers motors. This is not an encoder, universal NEMA 23 driver or closed-loop controller.

The Pi can access the USB serial protocol, but the existing Seizo application has not been integrated or hardware-tested with this controller. The application adapter, joint mapping, heartbeat/fault coordination and calibration remain to be implemented/tested.

The factory quotation fits **U7/U8 only** on two carriers from a five-board batch. Fuses, rear TVS, through-hole components, MCU, ten socket rows, three plug-in drivers, cooling and RUN/GND harness still need assembly. The supplied firmware is movement-locked and has not been flashed or tested on the real hardware.

The conservative four-vendor planning total is **$110.21 before unquoted import/checkout charges**. The $100 cap is **not met**. The cheaper $108.46 variant excludes two spare ICs and crosses LCSC's $15 small-order threshold before unknown handling/import charges. Neither is permission to order.

Use [the native project](project/big%20motor%20controller.kicad_pro), [BOM](cost/Final_BOM.csv), [cost explanation](cost/READ_FIRST.md), [assembly plan](ASSEMBLY_AND_FIRST_POWER.md), [quotation inputs](assembly/quotation-only/QUOTE_SCOPE.md) and [checks](checks/labeled-freeze-review.json). Earlier R4 and teaching-arm/encoder designs remain separate and preserved in the repository.

Digital consistency and software tests passed. Supplier DFM review, actual soldering, supply/current/torque, thermal, USB/UART, protection and regeneration tests remain. HOLD is not an independent emergency stop or brake; a gravity-loaded arm may fall after power/torque loss. This is a reviewed design prototype, not a qualified production product.


## Power budget scope

The reused 12 V/4 A adapter is allocated to the **new carrier plan**. The existing CNC shield also needs motor power. This BOM does **not** establish that one adapter can safely power the combined six-motor arm, and actual motor winding currents are unknown. Concurrent CNC-plus-carrier operation, shared adapter wiring and combined peak/continuous demand are unqualified. A separate appropriately sized supply and rated distribution may be necessary and would cost extra. Do not parallel adapter outputs. This is a carrier-component budget, not a guaranteed complete new-arm motor/power budget.
