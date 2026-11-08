# Seizo motor controller R4 — assembly quotation inputs

**Quotation only. These files do not clear manufacture, assembly or ordering.** They are derived from the unchanged R4 native PCB with SHA256 `636805945b92fed2fe233bc68a30c67dd05787d3209fb9537bd76ab27e720cde`.

## Single-side partial quotation

Use `JLC_R4_TOP4_SMT_QUOTE_BOM.csv` and `JLC_R4_TOP4_SMT_QUOTE_CPL.csv` with the assembly Gerber ZIP. The scope is **only U7, U8, F1 and F2**: four front SMD parts, 30 joints per carrier. All other 49 native references are excluded from this assembly scope. This option still leaves the rear TVS diode, all through-hole parts, the MCU, sockets, plug-in drivers and external harness for later work. It is not a fully presoldered controller.

The part numbers are existing R4 sourcing selections; an LCSC part number does not establish that the same part is currently stocked and selectable in JLC assembly. Check every selected JLC match, stock quantity, price and package in the quote UI. Native KiCad rotations are supplied; JLC library orientation, pin 1 and polarity must be reviewed in its placement viewer before acceptance.

**U7 has ordinary in-pad vias at pins 7, 9, 16, 18 and 19.** A hand-soldering instruction to wick/fill these flat is not a factory process. Unchanged R4 reflow needs an accepted fill/cap or other explicit manufacturing process; ordinary tenting does not prove suitability. This quotation input does not assert factory DFM approval.

## Full carrier quotation

`JLC_R4_FULL_CARRIER_QUOTE_BOM.csv` and its matching CPL contain **58 separately fitted carrier items**: 48 native items other than U1–U5, plus ten female socket rows. This is 200 through-hole and 32 SMD joints, 232 total. D2 is on the rear, so fitting all five SMD parts requires a two-sided service rather than Economic single-side assembly.

The five native logical module footprints U1–U5 are represented by assembly-only socket designators U1A/U1B through U5A/U5B. Each is a separate BOOMELE `2.54-1*8P` female socket, LCSC `C27438`. The full carrier files do **not** instruct the factory to solder the five BTT driver modules directly onto the carrier. Those modules remain separate plug-in items. The crosswalk gives exact native pad rows; these synthetic references are not printed on the existing native silkscreen and need explicit engineering acceptance and assembly instructions.

Two full-carrier items remain unmatched: U6 must be the exact **Waveshare RP2040-Zero-M** with its 23 pre-soldered male pins; a bare RP2040-Zero is not an automatically approved substitute. R5 is a required fitted tinned lead wire link, not an invented catalogue 0-ohm resistor. Their JLC part fields are deliberately blank. Full-carrier THT CPL entries retain the native footprint anchors rather than asserting verified component body centres or JLC insertion rotations. They are engineering quotation inputs, not ready-to-run insertion data.

The required external J9 to Zero Key2 RUN/GND two-wire harness, HOLD loop, off-board fan, five plug-in drivers, five heatsinks, USB cable, reused supply and mounting are outside factory-fitted BOM scope. A completely finished controller needs these as well. The RUN connection is not available on the Zero's ordinary edge header; it requires the documented external harness operation.

## Gerbers and checks

`Seizo_R4_Assembly_Quote_Gerbers.zip` contains the same seven copper, mask, silk and outline layers as the existing fabrication package, with newly exported front/back paste layers, two unchanged drill files and the corresponding Gerber job file. The original seven layers compare byte-for-byte after removing their two creation-date comments. No copper, placement, outline, mask or silkscreen change was made. Native coordinates are retained consistently in Gerbers and CPL: board X50–150mm, Y−50 to−150mm; no additional bottom-side X mirror is applied.

Every authored CSV was round-tripped through the spreadsheet library and checked cell-for-cell. BOM/CPL reference sets match for both scopes. `QUOTE_FILE_AUDIT.json` records counts, hashes and checks. This validates file consistency, not real solderability, motor performance, thermal behaviour, module compatibility or successful physical commissioning.

## Price and quantity limits

JLC's published assembly minimum is **two assembled boards**, so one set of parts is not automatically enough. Its promotion offsets an assembly setup fee through a qualifying coupon. Parts, manual through-hole work, extended-component feeders, freight, tax and any other charges still require a real unpaid quote. Exact coupon availability and eligibility must be read from the signed-in account. Do not count the EasyEDA-only coupon on this KiCad project. There is no verified complete assembled delivered total below $100 in these files.

Official sources: [promotion](https://jlcpcb.com/news/free-assembly-1-6-layer-pcb-discount-8-20-layer-pcba), [assembly price table](https://jlcpcb.com/help/article/pcb-assembly-price), [assembly capabilities](https://jlcpcb.com/capabilities/pcb-assembly-capabilities), [BOM/CPL preparation](https://jlcpcb.com/help/article/advice-for-bom-and-cpl-files-preparation), [assembly FAQ](https://jlcpcb.com/help/article/pcb-assembly-faqs), [via-in-pad guidance](https://jlcpcb.com/blog/via-in-pad-pcb).
