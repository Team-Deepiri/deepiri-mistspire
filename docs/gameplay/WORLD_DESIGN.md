# Mistspire World Design

**Fantasy:** The world is a stack of mist bands between stone spires. You ascend until the air runs out.

## Win condition

Highest **personal altitude** (cm). Optional **summit** achievements at authored markers.

## Scale

| Phase | Footprint | Vertical |
|-------|-----------|----------|
| v1 slice (`Main_WP`) | 4 × 4 km | 0 – 8 km |
| Production | 20 – 50 km² | 0 – 12+ km |

**C++ atlas (v2):** `UMistspireWorldAtlasSubsystem` seeds 12 horizontal **districts**, 16 **enterable buildings** (pocket interiors), and POI markers. See [BUILDINGS_AND_INTERIORS.md](BUILDINGS_AND_INTERIORS.md).

## World Partition

- Cell size: **128–256 m**
- Streaming source: VR pawn
- Data layers:
  - `DL_Landmarks_Authored` — spires, caves, story beats
  - `DL_PCG_Biome_Forest`
  - `DL_PCG_Biome_Arid`
  - `DL_Weather_Dynamic` (optional)

## Summits (seeded in C++)

| ID | Theme |
|----|-------|
| `summit_valley_gate` | Tutorial exit |
| `summit_mesa_crown` | First major climb |
| `summit_cloud_garden` | Sky island |
| `summit_obelisk_prime` | Tallest authored spire |
| `summit_orbital_needle` | Endgame |

Reposition in editor after creating `Main_WP`.

## Traversal

| Mechanic | Input (OpenXR) |
|----------|----------------|
| Move / turn | Thumbstick |
| Climb (slow) | Grip hold |
| Jump | Face button (bound per headset JSON) |
| Glider | Toggle via Blueprint/C++ hook |
| Comfort teleport | `menu` action short press |

## PCVR budget

Target **90 FPS** on high-end PC. Use HLOD, scalability ini, and fog for distant strata.

Build: [game/Content/Maps/README.md](../../game/Content/Maps/README.md).
