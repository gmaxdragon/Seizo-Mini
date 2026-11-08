# R4 four-pass PCB review

Checked 2026-11-08. All four reviews refer to the same native PCB, with no CAD changes.

| Pass | Actual check | Result |
|---|---|---|
| 1. Electrical design | Native ERC and DRC, schematic/PCB parity, 219 pin mappings and 217 independent schematic assertions | Zero rule violations, unconnected items or parity issues. |
| 2. Copper and power | Twenty terminal-to-driver phase paths, all 52 ground pads, five motor-power branches and protection/fan paths | Connected. Phase traces at least 1.0 mm with no phase vias. This is not thermal or transient qualification. |
| 3. Parts and geometry | 253 nominal geometry assertions: driver/socket pitches, drills, terminals, jack, MCU body/pads and rear TVS placement | No new nominal geometry defect. Five U7 in-pad vias remain an assembly-process limitation. |
| 4. Manufacturing exports | Fresh native export compared with seven Gerber layers and 456 plated plus five nonplated holes | Exported copper, outline and drill files match the reviewed board. |

## What these results mean

The bare-board files pass digital fabrication review. They do not certify a finished controller or a loaded robotic arm. Motor compatibility/current, actual component tolerances, soldering, power dynamics, cooling, USB/UART operation and regeneration still need physical tests.

## Pre-soldering

Factory assembly is possible in principle, but no complete delivered quote below $100 is verified. U7 pads 7, 9, 16, 18 and 19 contain ordinary open vias. The hand-assembly instruction to prefill and flatten them cannot silently be applied to automatic reflow. The factory must accept an appropriate process, or the routing must change and be rechecked.

D2 is on the back while U7/U8/F1/F2 are on the front. A one-sided assembly quote therefore does not cover all five SMD parts. See FACTORY_ASSEMBLY_OPTIONS.md for the published service limits, fees and missing order files. No automatic assembly order has been submitted.

## Fallback

The existing five-board fabrication quote supplies four spare bare PCBs after one is populated. Most small-part packs also leave spares, and the five drivers are socketed. A complete spare controller is not included. The Zero is directly soldered and is harder to replace. See REPAIR_AND_BACKUP.md for staged checks and repairs.

Keep the existing CNC/Uno system unchanged as a partial operational backup. Another generic Uno CNC shield is not a five-independent-axis replacement. No unverified backup board has been added to the budget.

Native PCB SHA256: `636805945b92fed2fe233bc68a30c67dd05787d3209fb9537bd76ab27e720cde`

Full machine-readable evidence is in checks/quadruple-preassembly-review.json and the five referenced audit reports.
