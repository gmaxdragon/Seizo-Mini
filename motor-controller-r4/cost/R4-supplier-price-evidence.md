# R4 purchase evidence — 2026-11-08

The estimate is **$98.84 before unquoted import fees**. It leaves only **$1.16** below the $100 limit, so it is not a guaranteed delivered price or clearance to order.

| Seller | Project merchandise | Shipping | Evidence |
|---|---:|---:|---|
| LCSC | $15.59 | $11.85 | Current saved R4 cart:17 exact SKUs, all in stock. Actual unit prices and displayed undiscounted line extensions are parsed directly from the DOM record (Private supplier-page evidence omitted from public export); actual screenshot (Private supplier-page evidence omitted from public export). The automatic$0.24 promotion is excluded conservatively. |
| West3D | $19.95 | $6.89 | Five BTT TMC2209 V1.3 modules at$3.99. Current unpaid checkout shows19.95goods+6.89shipping=26.84USD and tax0.00; conservative tax allowance retained separately. Bundle revision/heatsinks remain unconfirmed. |
| Waveshare | $8.87 | $10.00 | RP2040-Zero-M$4.99, Fan-4010-PWM-12V$2.69, USB data cable SKU35944$1.19. Previously observed cart and US Registered Post Air Mail after cable addition. An unrelated pre-existing$5.49 S3 item is excluded; all shared shipping is allocated to this project. |
| JLCPCB | $5.30 | $9.20 | Final R4 Gerber ZIP uploaded and auto-detected as100×100mm, two layers. Five boards, green,1.6mm,1oz, LeadFree HASL and tented vias: Web Price$5.30 plus$9.20 Global Standard Direct Line. Actual quote record (Private supplier-page evidence omitted from public export); actual screenshot (Private supplier-page evidence omitted from public export). Manufacturer final engineering review remains pending; conditional Desktop pricing is excluded. |

The earlier supplier records for the unchanged West3D/Waveshare amounts are preserved in R3's evidence (Private supplier-page evidence omitted from public export). The cost audit uses the root-observed supplier pages. Screenshots are actual website captures.

New R4 supervisor **TPS3808G01DBVR, LCSC C19653**, costs$0.4628 displayed unit/$0.46 line. It is the only new purchased SKU. The extra three10k resistors, one1k resistor, one100nF capacitor and two-pin header fit the existing MOQ packs. Updated current prices for Rubycon capacitors, the Molex fan header and the main diode offset$0.17 of the supervisor addition; LCSC merchandise therefore rises from$15.25 to$15.59; the current Molex header pack is$0.91.

The mandatory two-wire RUN/GND harness uses the user's owned jumper wires. R5 remains a fitted link made from a clipped lead from the purchased resistor pack. The MCU's23 male headers are included in the selected-M module. Ten female driver sockets are separately purchased. Parts populate **one controller**; the bare-PCB batch includes four spare unpopulated boards.

The estimate also includes a **$2.00 heatsink/adhesive reserve**. The HOLD bench link now uses an owned insulated jumper at no purchase cost. This remains an unquoted allowance; five heatsinks supplied with the modules have not been confirmed. The owned12V/4A adapter and printing material are reused. Motors and tools are excluded. Assembly is by the user; U7 is wide SOIC20 and new U8 is SOT23-6, so ordinary header soldering alone is not sufficient.

Merchandise plus reserves: **$51.71**. Shipping: **$37.94**. Conservative Dublin tax allowance at10.25% on both: **$9.19**. Total: **$98.84**. Actual checkout treatment of freight may differ. Duties, brokerage and other import fees are not assumed zero and could exceed the remaining margin.

The initial input bench-qualification envelope is at most **2.5A/30W shared**. This is not a protective current clamp or a certified continuous rating. Motor compatibility, cooling, diode/socket heating, regeneration, supply drop, the supervisor/RUN harness and firmware faults still require physical tests. The HOLD input is not an independent emergency stop or brake.

Use [R4-purchase-BOM.csv](R4-purchase-BOM.csv) and [R4-LCSC-import.csv](R4-LCSC-import.csv) for this revision. All53 native references are matched to the purchase BOM; all17 imported LCSC SKUs are from the current saved cart. See [the budget JSON](R4-native-purchase-budget.json) for the inventory hash and detailed limitations.

Final preorder review: the actual LCSC cart states that destination duties/taxes are the buyer's responsibility and that DDP needs customer-service inquiry. See saved current notice (Private supplier-page evidence omitted from public export) and [official terms](https://www.lcsc.com/help/terms-and-conditions). No supplier was contacted and no order/payment was placed. A landed quote within the $100 cap is still required.

Current authenticated LCSC checkout: merchandise$15.59 + shipping$11.85 - discount$0.24 = **$27.20**. FCA shipping explicitly leaves duties/customs/tax to the buyer. The planning estimate ignores the discount. Current JLC assembly quote preparation is separate from the earlier bare-board estimate; components and assembly are not included in the bare price. See CHECKOUT_SUMMARY.md.
