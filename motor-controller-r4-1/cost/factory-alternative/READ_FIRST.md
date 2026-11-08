# R4.1 purchase plan — not a final order

`Final_BOM.csv` covers one completed carrier using three new plug-in drivers. All five socket footprints receive **ten female 8-pin header rows**. U4/U5 receive no driver module. Motor terminal connectors are the verified upright 5.08 mm screw terminals; ordinary Dupont signal headers are not substituted for phase current.

| Supplier/category | Merchandise | Freight | Coupon |
|---|---:|---:|---:|
| West3D,3 drivers |11.97|6.89|—|
| Waveshare, RP2040-Zero-M, fan and data cable |8.87|10.00|—|
| LCSC, including two loose IC spares |15.59|11.85|—|
| JLC,5 PCBs,2 boards with U7/U8 only |26.76|16.03|−10.00|
| Unselected heatsink/adhesive reserve |2.00|unknown if separate|—|

Subtotal after coupon before tax $99.96; conservative 10.25% tax allowance $10.25; **$110.21 before unquoted imports or other checkout charges**. Unknown fees are not priced at zero. The JLC new-user coupon expires 2026-11-10 and must still apply at checkout; without it this plan rises. Final R4.1 uploaded manufacturing review is pending. All quantities/freight and destination duties/taxes must be checked before ordering.

The observed JLC partial-service quote total is $32.79 ($26.76 + $16.03 − $10). It is not the cost of a fully soldered controller. Only U7/U8 are fitted by that scope; no through-hole assembly is included. The other three SMD parts, F1/F2/D2, remain manual assembly.

Keeping the two spare ICs in the LCSC purchase leaves merchandise $15.59, above its $15 small-order threshold. Removing them gives$14.01 and an apparent $108.46 total **before** small-order handling/imports; that is not a verified saving. The separate consolidation proposal is unverified and is not the promoted purchase route.

Other carts may still contain old quantities; the current West3D basket has three drivers. Use this list to update them; do not pay from an older five-driver cart. Reuse of the owned 12 V/4 A adapter is explicit, but its actual label, plug, polarity, regulation and available current remain physical checks. Motors, tools and printer material are excluded. No vendor was contacted for this package.


## Power budget scope

The reused 12 V/4 A adapter is allocated to the **new carrier plan**. The existing CNC shield also needs motor power. This BOM does **not** establish that one adapter can safely power the combined six-motor arm, and actual motor winding currents are unknown. Concurrent CNC-plus-carrier operation, shared adapter wiring and combined peak/continuous demand are unqualified. A separate appropriately sized supply and rated distribution may be necessary and would cost extra. Do not parallel adapter outputs. This is a carrier-component budget, not a guaranteed complete new-arm motor/power budget.


## Current saved quotation

The current unpaid R4.1 TOP2 cart was verified at **$32.79**: $26.76 merchandise + $16.03 selected freight − one conditional $10 coupon. Final destination tax/imports and manufacturer CAM/DFM acceptance remain unconfirmed. See [QUOTE_RECHECK.md](QUOTE_RECHECK.md) for exact scope and placement verification.
