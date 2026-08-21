# Mistspire World Design

**Fantasy:** The world is a stack of mist bands between stone spires. You ascend until the air runs out.

## Win condition

Highest **personal altitude** (cm). Optional **summit** achievements at authored markers.

## Scale

| Phase | Footprint | Vertical |
|-------|-----------|----------|
| v1 slice (`Main_WP`) | 4 × 4 km | 0 – 8 km (authored slice; code biomes continue above) |
| Full biome stack (C++) | 20 – 50 km² target | 0 – 20 km (`BiomeFromAltitude`) |

**C++ atlas (v2):** `UMistspireWorldAtlasSubsystem` seeds 12 horizontal **districts**, 16 **enterable buildings** (pocket interiors), and POI markers. See [BUILDINGS_AND_INTERIORS.md](BUILDINGS_AND_INTERIORS.md).

## World Partition

- Cell size: **128–256 m**
- Streaming source: VR pawn
- Data layers (keep the `PCG` token — these are Procedural Content Generation layers, distinct from authored landmarks and dynamic weather):
  - `DL_Landmarks_Authored` — spires, caves, story beats
  - `DL_PCG_Biome_Mist`
  - `DL_PCG_Biome_Arid`
  - `DL_PCG_Biome_Forest`
  - `DL_PCG_Biome_Ember`
  - `DL_PCG_Biome_Crystal`
  - `DL_PCG_Biome_Void`
  - `DL_PCG_Biome_Tundra`
  - `DL_PCG_Biome_Aether`
  - `DL_PCG_Biome_Sanctum`
  - `DL_PCG_Biome_Pinnacle`
  - `DL_Weather_Dynamic` (optional)

## Biome altitude bands (source-of-truth)

The altitude-to-biome mapping is driven by `UMistspireEnvironmentSubsystem::BiomeFromAltitude` (thresholds in centimeters).

| # | Biome (C++ subsystem) | Altitude range | World Partition layer | Hazard |
|---|------------------------|----------------|-------------------------|--------|
| 01 | `UMistspireBiomeMist` | 0–1 km | `DL_PCG_Biome_Mist` | — |
| 02 | `UMistspireBiomeArid` | 1–3 km | `DL_PCG_Biome_Arid` | — |
| 03 | `UMistspireBiomeForest` | 3–5 km | `DL_PCG_Biome_Forest` | — |
| 04 | `UMistspireBiomeEmber` | 5–7 km | `DL_PCG_Biome_Ember` | HeatExhaustion |
| 05 | `UMistspireBiomeCrystal` | 7–9 km | `DL_PCG_Biome_Crystal` | SharpShards |
| 06 | `UMistspireBiomeVoid` | 9–12 km | `DL_PCG_Biome_Void` | VoidSickness |
| 07 | `UMistspireBiomeTundra` | 12–14 km | `DL_PCG_Biome_Tundra` | Blizzard |
| 08 | `UMistspireBiomeAether` | 14–16 km | `DL_PCG_Biome_Aether` | GravityAnomaly |
| 09 | `UMistspireBiomeSanctum` | 16–18 km | `DL_PCG_Biome_Sanctum` | OxygenVacuum |
| 10 | `UMistspireBiomePinnacle` | 18–20 km | `DL_PCG_Biome_Pinnacle` | CosmicRadiation |

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
