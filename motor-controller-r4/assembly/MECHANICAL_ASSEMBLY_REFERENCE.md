# R4 mechanical assembly reference

All coordinates below are in millimetres, measured from the top-left Edge.Cuts centreline in the FRONT view. Back drawing is mirrored; do not reuse its screen coordinates as front coordinates.

| Mount | X | Y | Hole diameter |
|---|---:|---:|---:|
| H1 | 4.000 | 4.000 | 3.20 |
| H2 | 96.000 | 4.000 | 3.20 |
| H3 | 96.000 | 96.000 | 3.20 |
| H4 | 4.000 | 49.100 | 3.20 |

The mounting pattern is asymmetric; do not assume four corner holes. Model mounts from these centres or the native PCB. Leave clearance beneath rear-mounted parts and solder joints; retain the assembly guide’s minimum underside clearance.

PDFs use native plot scale1. Printer scaling can change their physical size; use the native CAD dimensions for mounts. SVG/PNG maps add a margin for readable annotations.

In the back map, D2 is rear-mounted; other component references identify front-side through-hole leads. U1–U6 pad numbers are the KiCad logical numbers; consult R4-pad-map.csv and the pin-map/assembly guide before equating those numbers with labels on the purchased modules.

Reference drawings show native footprint outlines, references and pad numbers. They do not prove actual driver/socket/heatsink/fan/cable fit, lead tolerances, connector strain relief, cooling or safe motor operation. Inspect the real components and follow the first-power procedure.

Status: **FROZEN NATIVE SNAPSHOT**
Native PCB SHA256: `636805945b92fed2fe233bc68a30c67dd05787d3209fb9537bd76ab27e720cde`

## Required J9 RUN service harness

J9 is shown in both maps. The square pad is pin1; use the pad numbers/net list rather than left/right assumptions, especially in the mirrored back view.

| J9 pin | Native net |
|---|---|
| 1 | `VMOT_RESET_N` |
| 2 | `GND` |

Required two-wire service harness: J9.1 VMOT_RESET_N to actualWaveshareZero Key2.1 RUN, J9.2 to Key2.2 GND. RUN is not on the23edgeheaderpads. Identify by manufacturer net/pad and unpowered continuity; never infer button-pad left/right orientation. No native carrier RUN pad has been invented.

This harness is part of the reset protection circuit. Follow the main assembly guide, identify the actual manufacturer button pads with all power removed, and provide strain relief. The map does not add a fictitious RUN pin to the 23 edge-header pins.
