# Current funding BOM and stale sync

The root BOM had reverted to the earlier teaching Mini R5 parts (potentiometers/encoder sensors) during a Half-Life repository sync. Those are not the R4.1 motor-controller purchase list.

Use `current-funding-BOM.csv` as the app-facing importer. It uses the six headers from the prior Half-Life importer: Item, Purpose, Quantity, Unit Cost USD, Vendor, Link. It contains 24 selected purchase lines: 21 LCSC lines ($28.47), three new West3D drivers ($11.97), and one five-board bare JLC batch ($5.30). One Adafruit thermal-tape sheet adds $0.95. Merchandise totals $46.69 before discounts, shipping, tax or import charges. Every imported line has a nonblank numeric unit price and actual source link; pack purchase quantities are retained. Unit costs use eight decimal places where necessary so the per-line cent totals match the displayed cart extensions.

Import/update these current parts in Half-Life before syncing back to GitHub; otherwise an old saved app list may overwrite the repaired root BOM again. Keep the detailed R4.1 BOM as the component/ownership/DNP record. The earlier R5 design is preserved separately.

Do not import freight, tax, owned parts, DNP/future components or the unselected $2 cooling reserve as purchasable electronics. Known displayed freight totals29.97USD; Adafruit delivery is unquoted. Cooling parts are now selected. The100USD allocation contains5USD for conditional fallback adaptation and18.62USD for remaining charges; it is not a final landed quote.

The existing four removable A4988 modules stay on the CNC shield; they are not assumed drop-in replacements for this TMC2209-specific carrier. This importer preserves three new fitted TMC modules because the unchanged locked firmware checks populated mask 7. It does not claim the controller or 12 V/4 A supply can drive arbitrary NEMA23 motors or a complete loaded arm.
