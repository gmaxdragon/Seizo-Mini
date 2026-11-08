# R4 factory-assembly options

Checked 2026-11-08 using supplier websites only. No supplier was contacted and no order or payment was made. The saved website quotation is described in cost/CHECKOUT_SUMMARY.md. It is not manufacturing approval or a complete landed price.

R4 can be factory assembled in principle. **No complete, delivered factory-assembled R4 order below $100 has been verified.** The latest manual-assembly planning figure supplied to this review is $98.84 before unquoted imports, leaving $1.16. Existing purchase prices cannot simply be treated as a factory's component quote.

## What the factory would need to do

One carrier has 58 separately fitted items and 232 carrier solder joints: 200 through-hole and 32 surface-mount. The five small surface-mount parts are U7, U8, F1, F2 and rear-mounted D2. U1–U5 are five logical driver references, but the carrier is actually soldered with ten separate eight-pin female socket rows. The five driver modules plug in afterward. U6 is the RP2040-Zero-M soldered through its 23 header pins. The mandatory two-wire connection from J9 to the Zero's actual RESET contacts is an additional manual operation.

The current purchase BOM is not a ready-to-upload factory assembly BOM. It must distinguish the sockets, modules, soldered carrier parts and off-board work. A small-module SKU being sold at LCSC does not establish that the exact item is available in JLCPCB's assembly inventory.

## Practical options

| Option | Factory work | Work remaining at home | Current conclusion |
|---|---|---|---|
| Partial top-side assembly | U7, U8, F1 and F2, subject to exact part matching and U7 process approval | Rear D2, all through-hole parts, sockets, Zero and reset wires; driver insertion and cooling | Most plausible way to avoid the two small IC soldering jobs, but still needs an uploaded-file quote and is not fully presoldered. |
| Full mixed assembly | Front and back soldered parts, sockets and module mounting as explicitly ordered | Any excluded module fitting, programming, reset harness, fan/strain relief and commissioning | Technically possible; no verified price within $100. |
| Current DIY assembly | None | All assembly, with the existing detailed instructions | Only the presently costed assembly method; its landed cost is still unbounded. |

JLCPCB's capability table supports mixed SMT/through-hole assembly. Economic assembly uses one placement side; Standard permits both sides. Both list a minimum of two assembled boards. Thus all five R4 SMD parts, including the existing rear D2, are not a normal single-side Economic order. Leaving D2 for hand soldering is a partial-assembly option; moving it just to alter a quote would require a new power-layout review. [JLCPCB capabilities](https://jlcpcb.com/capabilities/pcb-assembly-capabilities)

The same capability documentation permits socket/module combinations when both components receive assembly entries and explicit instructions, with added manual costs. It also permits separately specified wire work. This does not establish availability or acceptance of R4's particular modules and RESET-contact soldering. The factory must confirm that scope through its order process. [JLCPCB capabilities](https://jlcpcb.com/capabilities/pcb-assembly-capabilities)

## Published charges are not an R4 quote

JLCPCB's live price table, updated October 22, 2026, lists an Economic setup charge of $8.18, a $1.53 stencil, $3.58 hand-solder labor per order, $0.0164 per manual joint and $0.0016 per SMT joint at this volume. An extended-part feeder is $3.07 per type. The table lists Standard double-side setup at $51.12 and its two-side stencil at $16.42. Parts, special handling and other applicable charges remain additional. Even adding only the Economic setup fee to the unchanged $98.84 manual basket would exceed $100, but a real factory basket must also remove duplicated purchases and reprice freight. Coupons must not be assumed. [JLCPCB price details](https://jlcpcb.com/help/article/pcb-assembly-price)

PCBWay advertises a $29 prototype assembly offer and supports mixed assembly. That marketing figure is not a delivered, component-inclusive R4 quote. A project-specific online quote is still required. [PCBWay assembly service](https://www.pcbway.com/pcb-assembly.html)

Removing the four top-side SMD purchase lines from the separate LCSC basket would also move that basket below its $15 small-order threshold. Recheck handling and shipping rather than subtracting those line prices alone.

## Existing process issue to resolve before automated soldering

U7 still has ordinary escape vias inside pads 7, 9, 16, 18 and 19. The DIY instructions require solder filling and flattening those holes before fitting the IC. Factory reflow cannot silently assume that hand operation happened. The assembly order must receive an accepted process, or the layout must be changed and rechecked. Standard tenting is not equivalent to filled and capped vias. JLCPCB's fabrication table describes a dedicated filling/capping process; its design guidance warns about solder loss through unfilled via-in-pad features. [Fabrication capabilities](https://jlcpcb.com/capabilities/Capabilities%2C/), [via-in-pad guidance](https://jlcpcb.com/blog/via-in-pad-pcb)

## Files missing from the current bare-board package

The original bare-board package contains seven layers and drills. Additional paste layers, matched BOM/CPL quotation inputs and their scope audit are now in assembly/quotation-only. The original copper, mask, silk, outline and drill geometry is unchanged. The full website quote matched 56/58 carrier items; U6 and R5 remain manual. These inputs still require manufacturer process/orientation review.

For JLCPCB, prepare its reference-matched assembly BOM and CPL, include only the intended placed parts, select exact matching parts and inspect their rotation/polarity in the order viewer. The supplier says its system populates only matched/selected parts. [BOM/CPL preparation](https://jlcpcb.com/help/article/advice-for-bom-and-cpl-files-preparation), [assembly FAQs](https://jlcpcb.com/help/article/pcb-assembly-faqs)

PCBWay requests Gerbers, an assembly BOM, silkscreen/assembly reference and centroid coordinates for SMD parts; its production guidance also requests the relevant solder-paste data. Its assembly BOM must identify surface-mount, through-hole and mixed work. [PCBWay required files](https://www.pcbway.com/helpcenter/pcb_assembly_ordering/What_files_are_requested_for_assembly_production_.html), [assembly process](https://www.pcbway.com/assembly-process.html)

Factory assembly reduces the user's soldering work. It does not prove motor compatibility, motor torque, power stability, cooling, regenerative-energy handling or safe gravity-load behavior. Those remain physical qualification tasks.

## Actual uploaded quote

On 2026-11-08 the signed-in JLC website priced five PCBs and two double-sided carriers with 56 matched items each at **$137 before shipping**. Its cart estimated **$165.39** including $38.39 shipping and a $10 coupon. Final checkout still needs its own delivery address, so this is not the final landed amount. The full project planning comparison is $213.10 before additional charges; see cost/CHECKOUT_SUMMARY.md. No payment or production order was submitted.
