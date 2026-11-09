# Current purchase prices and100USD budget allocation

The root BOM.csv and HalfLife_import.csv now contain **24 selected purchase lines** with numeric prices and working source links. Merchandise total is **46.69USD**: LCSC28.47, West3D11.97, JLC5.30, Adafruit0.95. Do not add the detailed Real_BOM.csv as a second purchase list: it includes ownership/DNP records.

| Website | Current displayed amount | Scope |
|---|---:|---|
| LCSC |42.07|21lines, including five small heatsinks;28.47goods +13.88shipping -0.28promotion |
| JLCPCB |14.50|5barePCBs;5.30goods +9.20shipping; no coupon counted |
| West3D |18.86|3drivers;11.97goods +6.89shipping |
| Adafruit |0.95|One3M8810sheet, merchandise only; shipping/tax not calculated |

**Displayed sum76.38USD.** The prior74.90USD sum was correct before the extra heatsinks, small freight increase and tape purchase. Adafruit's displayed0USD shipping/tax placeholders are NOT free delivery or confirmed tax exemptions. JLC's chosen shipping terms explicitly leave destination duty/tax/handling collectible later; the amount14.50USD is not a guarantee of a duty-paid landed total.

| Allocation | USD |
|---|---:|
| Displayed amounts above |76.38|
| Conditional NEMA23-only fallback adaptation materials |5.00|
| Remaining allowance for Adafruit delivery, uncollected tax/import/handling and price changes |18.62|
| Maximum allocated budget |100.00|

This is a **100USD allocation**, not a100USD supplier quotation or verified landed total. Remaining charges are unquoted and may exceed the allowance. Stop before buying if the actual aggregate exceeds the cap; do not remove protection/cooling to compensate. Owned enclosed fallback drivers cost0USD to acquire, but their interface, power wiring and firmware are unqualified. The5USD line is a materials allowance, not a selected complete backup kit; it is intentionally excluded from the purchase importer.

Without the current LCSC0.28USDpromotion, the displayed basis would be76.66USD and the remaining-charge allowance would reduce to18.34USD. No future coupon is required in this allocation. The earlier98.94USD manual plan and factory quotations are superseded budget scenarios preserved in history/alternate files, not a new final cost.

The build reuses the owned12V4Aadapter, ownedCNC/UnoA4988 setup, jumper wires and printing material. Motors/tools remain excluded from this carrier-parts budget. The entire motor system is not proven to fit48W; see POWER_AND_FALLBACK.md. Every carrier part, including U7/U8, is hand soldered in this option.

The two cooling purchases are selected: LCSCC5250879/F12-04-05 five8.8x8.8x5mmheatsinks (fit3), and Adafruit1467/3M8810sheet0.95USD. Cut thermal adhesive to the mating area, keep metal clear of electrical pads, and perform physical fit/thermal/adhesion checks. No new separate cooling fan is omitted: the SUNON blower remains in the LCSC list.
