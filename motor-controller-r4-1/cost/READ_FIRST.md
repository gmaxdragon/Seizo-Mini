# R4.1 manual build and budget

This purchase option keeps three new plug-in drivers in U1/U2/U3, the unchanged R4.1 carrier circuit and the separate existing CNC controller. U4/U5 remain empty. It reuses the approved owned 12V/4A adapter and uses one USB-A-to-USB-C data cable from the Pi to the carrier's MCU. Motor power is separate from USB.

**Estimated project total: $98.94, including tax and import/price-change reserves.** This is a planning total, not a guaranteed landed checkout amount. No coupon is needed for the calculation. All five carrier protection parts remain: U7, U8, F1, F2 and D2; the other diodes and bypass/bulk capacitors remain too.

| Purchase/category | Goods | Freight | Total used in budget |
|---|---:|---:|---:|
| LCSC: 20 lines, including bare MCU, headers, blower and data cable | $27.97 | $13.85 | $41.82 |
| West3D: three new BTT driver modules | $11.97 | $6.89 allowance | $18.86 |
| JLCPCB: five bare carrier boards, no assembly | $5.30 | $9.20 current saved quote | $14.50 |
| Module heatsinks/adhesive reserve | $2.00 | Included in reserve only if no separate parcel | $2.00 |
| Subtotal | | | **$77.18** |
| Conservative tax allowance: 12% of subtotal | | | $9.26 |
| Import and price-change contingency | | | $12.50 |
| Estimated total | | | **$98.94** |

The revised LCSC unpaid cart was checked on November 8, 2026: **20 lines, all in stock, $27.97 goods + $13.85 freight − $0.28 automatic promotion = $41.54 displayed**. The budget deliberately ignores that promotion and uses $41.82. This cart display does not establish final destination duties, brokerage or tax. Shipping for the revised parts cart changed from the original 17-line cart; do not reuse the original $11.85 freight figure.

The JLC bare-board unpaid cart was also saved and checked on November 8: **five bare boards, no PCBA, $5.30 goods + $9.20 Global Standard freight − $5.30 automatic PCB coupon = $9.20 displayed**. The budget deliberately ignores that coupon and uses **$14.50**. No delivery address was entered, so this is a current saved quotation rather than a final destination-bound landed total. West3D's current unpaid cart contains exactly three BTT0529 modules for $11.97; its $6.89 freight remains an allowance. Future modules and the unpopulated J8 header are excluded from purchase costs.

`LCSC_import.csv` is the 20-line supplier importer. `Final_BOM.csv` is the complete purchase list, including spares from minimum packs, the DNP fan header and reused items. `Solder_List.csv` lists what actually gets fitted. Purchase quantities are not fitted quantities.

This lower-cost option is **fully manual carrier assembly**. U7 is a 20-lead wide SOIC; U8 is a six-lead SOT-23. F1/F2 and D2 are also surface-mount. The drivers themselves arrive assembled. There is no factory assembly, stencil or assembly coupon in this budget. Assembly needs suitable tools, flux, magnification and short/continuity checks; header-only soldering experience does not establish that the small-chip work has been completed successfully.

The genuine bare Waveshare RP2040-Zero replaces the preheadered Zero-M. Add its male header rows 9+9+5 manually. The two-wire SUNON blower replaces the old 40mm fan: leave J8's header unpopulated, solder black to J8.1 and red to J8.2, and add a printed blower mount/duct and wire strain relief. F2 and D3 stay fitted. Full instructions are in `BUY_AND_SOLDER_MANUAL.md`.

The $2 cooling-accessory line remains an **unselected reserve** until compatible heatsinks and their thermal interface are confirmed. Exact BTT V1.3/R110 delivery, adapter label/polarity/plug fit, current settings, board assembly, motor loading and temperatures require inspection/testing. Motors and tools are outside this controller budget. No new supply, motor cable or mounting-fastener pack is included. If new mounting hardware is necessary, reprice it before buying. Do not order if actual combined charges exceed $100.

The owned adapter is allocated to this new carrier; the budget does not establish that it can also supply the existing CNC controller at the same time. The starting shared-input qualification envelope is at most 2.5A/30W, not a measured continuous rating or electronic current limiter. NEMA frame size alone does not establish a compatible motor. HOLD is a motion-control input, not an independent emergency stop or brake. Physical motor/arm commissioning and manufacturer fabrication acceptance remain separate from the completed digital design checks.
