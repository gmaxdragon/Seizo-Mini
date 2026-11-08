# R4 landed-cost source review

Read-only review, 2026-11-07. No cart, CAD, order or payment changes.

The current cost file is $98.84 before unquoted import fees, including a $2 heatsink reserve. This is not a bounded delivered total.

## Import-charge evidence

- [LCSC terms](https://www.lcsc.com/help/terms-and-conditions), Shipping & Delivery: import licenses, duties, tariffs, taxes and brokerage remain the customer's responsibility. Prices exclude these charges. The actual cart's similar notice is stronger than assuming Global Direct is prepaid.
- [JLCPCB shipping methods](https://jlcpcb.com/help/article/shipping-methods-and-delivery-time), updated September 9, 2026: most international shipments incur additional duties/taxes, assigned to the customer and additional to shipping. A named DDP checkout quote or supplier confirmation must override that general limitation for the specific order.
- [Waveshare shipping terms](https://www.waveshare.com/help/shipping-terms.htm): indexed official page assigns international duties/taxes/tariffs to the customer. The current browser cart must establish any specific prepaid service.

Changing only JLC shipping from the previously observed $9.20 Global Standard Direct Line to its $26.60 UPS-DDP option increases the current planning total by about $19.18 including the conservative tax allowance, to about $118.02. That alone cannot meet $100. This comparison is a planning calculation from prior saved quotes, not a new checkout.

## Verified retail alternatives

| Seller | Exact component | Observed goods price | Import treatment and availability |
|---|---|---:|---|
| RobotShop | Waveshare 20335 RP2040-Zero-M, SKU RB-Wav-360 | $7.66 | Page explicitly says USA tariff-inclusive. On Demand, not normally stocked, no retrievable ETA. Shipping and sales tax extra. |
| RobotShop | Waveshare FAN-4010-PWM-12V, SKU RB-Wav-1226 | $3.26 | Same tariff-inclusive statement and On Demand limitation. Exact fan dimensions/connector match the selected product. |
| OpenELAB | Waveshare-20335, presoldered Zero-M variant | $5.45 | Page explicitly says duties/customs included, China warehouse. Indexed page marks variants sold out. Root needs live UI stock before using it. Shipping/tax extra. |
| ShillehTek | RP2040 Zero with presoldered headers | $5.99 | Seller ships from Chicago, in stock. Product is ShillehTek-branded; not established as the exact Waveshare module. Do not silently substitute. |
| ShillehTek | USB-A male to USB-C male, 0.5m, USB2 data cable AA787 | $4.49 | Seller product manual confirms data + power and connectors. Domestic shipping extra below $35; no suitable matching fan established. |

Sources:

- [RobotShop exact MCU](https://www.robotshop.com/products/rp2040-zero-pico-like-mcu-board-based-on-rp2040-mini-version-w-header)
- [RobotShop exact fan](https://www.robotshop.com/products/waveshare-dedicated-cooling-fan-for-compute-module-4-io-board-pwm-speed-adjustment)
- [OpenELAB exact MCU](https://openelab.com/products/waveshare-rp2040-zero-a-pico?country=US&currency=USD&variant=56251465269572)
- [ShillehTek MCU](https://shillehtek.com/products/rp2040-zero-microcontroller-board-presoldered-headers)
- [ShillehTek cable](https://shillehtek.com/products/black-type-c-usb-cable-2a-0-5m)
- [Cable data specifications](https://shillehtek.com/blogs/shillehtek-product-manuals/black-type-c-usb-cable-2a-0-5m-manual)
- [Chicago shipping policy](https://shillehtek.com/policies/shipping-policy)

## Practical result

No fully qualified, in-stock, fee-bounded under-$100 source replacement was verified by this read-only research. RobotShop establishes a real duty-inclusive alternative for the MCU/fan, but its higher item prices, missing cable/shipping total and On Demand status prevent claiming a complete order. Avoid speculative tariff percentages, coupon-dependent totals or treating seller boilerplate as a specific DDP quote.
