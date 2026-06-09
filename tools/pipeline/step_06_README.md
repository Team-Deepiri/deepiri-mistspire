# Pipeline Step 6 — Summit Markers + Rest Shelters

Place `AMistspireSummitMarker` actors along the climb:

| Summit ID | Approx Altitude | Theme |
|-----------|----------------|-------|
| `summit_valley_gate` | 500 m | Tutorial exit |
| `summit_mesa_crown` | 2500 m | First major climb |
| `summit_cloud_garden` | 5000 m | Sky island |
| `summit_obelisk_prime` | 8500 m | Tallest authored spire |
| `summit_orbital_needle` | 12000 m | Endgame |

Also place `AMistspireRestShelter` at ~1 km intervals:
- Overlap volume refills stamina + oxygen
- Enables save checkpoint on exit
- Assign haptic feedback + ambient audio

**Properties to set per SummitMarker:**
- `SummitId` (string matching the C++ enum)
- `ReachRadiusCm` (default 500)
- `BeaconColor` / `BeaconPulseRate`
