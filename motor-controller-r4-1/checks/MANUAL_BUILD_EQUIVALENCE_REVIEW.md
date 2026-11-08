# R4.1 manual-build substitutions: independent review

Reviewed 2026-11-08. Read-only review of the frozen R4.1 circuit; no CAD, firmware, cart, order or supplier contact changed by this review. Prices and delivered totals are outside this review.

## Outcome

The genuine bare Waveshare RP2040-Zero plus correctly fitted straight headers is an electrically equivalent assembly approach to the preheadered Zero-M. The selected SUNON blower is compatible with the protected 12 V fan branch on paper, but needs a different mounting bracket/duct. These sourcing changes do not require altering the control or protection circuit. They do not establish hardware operation, thermal performance, startup fuse behavior, or final manufacturing quality.

## U6: bare Zero and manually fitted headers

- Buy genuine **Waveshare RP2040-Zero, SKU20187 / LCSC C5350143**, not an unspecified compatible clone. The manufacturer's schematic provides the same 23 edge-header signals, native USB-C, BOOT and RESET/KEY2 circuit used by the existing carrier. The Zero-M/20335 naming indicates the preheadered assembly; factory header dimensions were not independently specified in the accessible manufacturer schematic.
- The native carrier footprint has 23 holes arranged **9+9+5**, 2.54 mm pitch, 15.24 mm between side rows, with **1.0 mm drills**. Keep the existing USB-edge orientation and pin mapping. Firmware/GPIO assignments remain unchanged.
- The freshly fetched ZHOURI manufacturer drawing for **2.54-1*40 / C2977586**, dated 2025-02-26, specifies **0.63 mm square pins**, 2.54 mm pitch, 3.0±0.1 mm short tails, 6.0±0.1 mm long ends and 2.54 mm insulation height. A nominal pin diagonal of 0.891 mm fits a nominal 1.0 mm carrier hole; actual purchased parts and finished-hole tolerances still require a gentle dry fit. Do not force bent pins.
- Cut one strip into **9, 9 and 5 pins**; 17 pins remain spare. Fit short tails through the Zero and long ends toward the carrier, keeping the insulating spacers between boards. Check underside component clearance before soldering. An unpowered alignment fixture is useful. This adds **23 module-header solder joints** to the existing 23 carrier joints. It is a manually assembled module, not a factory-presoldered purchase.
- **Keep the mandatory J9-to-KEY2 RUN/GND harness.** RUN is not an exposed edge-header signal. Verify the correct switch terminals with continuity before attaching; do not substitute a GPIO for RUN or omit the harness. This sourcing change does not remove that requirement.
- Update purchasing/BOM and assembly descriptions from Zero-M to bare Zero plus headers, without changing the verified pin map. Do not describe the manual variant as supplied with factory-soldered pins.

Manufacturer schematic: [Waveshare RP2040_Zero.pdf](https://files.waveshare.com/upload/4/4c/RP2040_Zero.pdf). This was freshly accessible; the manufacturer's product/wiki pages returned403 during this review, so the exact SKU pairing is not represented here as a fresh manufacturer-page verification. The [C5350143 listing](https://www.lcsc.com/product-detail/C5350143.html) identifies Waveshare and RP2040-Zero. [ZHOURI drawing](https://datasheet.lcsc.com/datasheet/pdf/2b0aad031cd5aaf3aaf9d995e0fb9960.pdf?productCode=C2977586).

## Blower, branch loading and direct lead termination

Exact SUNON **MF50151VX-1B00C-A99 / C49308057** approval specification, rev1, dated2020-08-18, was re-read locally, including the dimensions drawing. It states **12 V nominal, 4.5–13.8 V operating, 4.5 V switched-start at25°C, 0.155 A average /0.179 A maximum rated current, 1.86/2.15 W**. The rated-current test is after ten minutes; **0.179 A is not a guaranteed startup/inrush peak**. Do not PWM its power or ground.

R4.1 feeds it through **F2 0454.500MR, 0.5 A time-delay, and D3 1N5819** from VIN_FUSED. J8.1 is **GND**, J8.2 **FAN_VMOT**; J8.3/4 unused. At rated running maximum, loading is **35.8% of F2's nominal current**, and **17.9% of D3's 1 A rating**. A regulated12 V±10% source remains within the operating window after the series diode on paper. Reserve at least **0.179 A /2.15 W** in the shared supply budget; a2.5 A initial board-input envelope leaves2.321 A for other input loads. This is arithmetic, not enforced current limiting or a measured simultaneous-motor result. Startup, supply sag, fuse behavior and temperature require first-board measurements.

Dimensions are **51.7±0.5 ×51.6±0.5 ×15(+0.5/−0) mm** with two4.5±0.3 mm holes: **not a40 mm axial drop-in**. Red is positive; black ground. Omit the J8 Molex header and solder the supplied26AWG leads into pads2/1 respectively, trimming exposed strands, keeping insulation close, inspecting bridges, adding separate strain relief and preserving insulated underside clearance. Provide a bracket/duct with clear inlet and outlet aimed across the three fitted driver heatsinks and D1. Airflow ratings alone do not verify cooling. No motor-connected powered qualification has occurred.

[Exact SUNON approval specification](https://datasheet.lcsc.com/datasheet/pdf/ee5d2a9d0beb092f4b58af8906878919.pdf?productCode=C49308057); inspected local copy: `consolidation-references/C49308057.pdf`, pages5,8,11,13. Do not silently substitute catalogue suffix1B000 for the reviewed1B00C part.

## Release boundary

This review accepts the substitutions for documentation and sourcing with the stated manual assembly steps. It does not certify a complete robot, guarantee soldering fault tolerance, or replace first-board continuity, isolated power-up, fan startup and loaded thermal tests. U4/U5 remain unpopulated under the three-driver plan. All existing motor-power, reset, enable defaults and movement locks remain required.
