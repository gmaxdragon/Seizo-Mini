# R4.1 TOP2 factory quotation inputs

Quotation only, not assembly/production clearance. Native PCB SHA256 `6f870caf0e68e484de63a360227a2154618d24368472897633d0d671f4e0d3b9`.

The BOM/CPL select **U7 SN74HC541DWR, C6757** and **U8 TPS3808G01DBVR, C19653** only: two front-side ICs, 26 solder joints per assembled board. Two carriers receive those parts from a five-PCB batch. The matching ZIP has9 fresh Gerber layers including F/B paste,2 drills and1 job file. Native coordinates are retained, without an extra bottom-side mirror.

U7 land/via drilling overlaps found in R4 were rerouted in this revision; the independent nominal-geometry audit found no SMT-land/drill overlap and zero vias inside U7 pads. That does not replace factory soldermask/CAM tolerance, orientation or DFM approval. Every IC match, pin 1, JLC rotation and solder-paste placement must be reviewed before accepting the quote.

**Everything else is excluded from this partial assembly**, including F1/F2fuses, rear D2TVS, all through-hole parts, directly soldered23-pin RP2040-Zero-M,10 female driver socket rows,3 plug-in drivers, heatsinks, fan, USB cable, RUN/GND harness, HOLD loop and supply. Never interpret these two BOM rows as the full purchase BOM.

The observed matching two-IC quotation was$26.76 plus$16.03 freight minus a conditional$10 new-user coupon=$32.79, expiring2026-11-10. The final labeledR4.1 files still need supplier review, current freight/tax/import confirmation and coupon validation. The complete project plan exceeds100 USD.

## Supplier-library rotations

The active **JLC_R41_TOP2_QUOTE_CPL.csv** contains the supplier-library-specific rotations **U7=0° and U8=0°**. X/Y positions and native CAD/Gerbers are unchanged. **JLC_R41_TOP2_NATIVE_POSITION_AUDIT.csv** preserves KiCad's native90°/180° values for audit only; do not upload that audit file as the factory placement file. The override record includes exact native pad coordinates and observed pin 1/body alignment.

Root re-imported the exact corrected CPL from scratch and visually verified both parts' pin 1 and all pads aligned in the JLC quote viewer. This verifies the saved quotation placement only, not manufacturer CAM/production acceptance.

The current saved unpaid cart was verified at **$32.79**: $26.76 merchandise + $16.03 selected freight − one conditional $10 coupon. See [the current quotation review](../../cost/QUOTE_RECHECK.md). Final destination tax/imports and manufacturer CAM/DFM acceptance remain unconfirmed.
