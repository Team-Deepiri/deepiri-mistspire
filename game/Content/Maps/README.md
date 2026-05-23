# Main_WP — World Partition vertical slice

Binary `.umap` assets are not committed in v1. Create the map in UE Editor after opening `game/Mistspire.uproject`.

## Create Main_WP (4 × 4 km vertical slice)

1. **File → New Level → Empty Open World** (World Partition enabled).
2. Save as `Content/Maps/Main_WP`.
3. **World Settings → World Partition:**
   - Cell Size: **25600** (256 m) or **12800** (128 m)
   - Loading Range: **2** cells
   - Enable **Runtime Hash Set** default grid
4. **Landscape:**
   - New Landscape → 8129×8129 or use **Import** heightmap (see `Content/Terrain/README.md`)
   - Scale Z exaggerated (e.g. 100–200) for dramatic vertical strata
   - Position so playable valley starts near origin
5. **Data Layers** (Window → World Partition → Data Layers):
   - `DL_Landmarks_Authored`
   - `DL_PCG_Biome_Forest`
   - `DL_PCG_Biome_Arid`
6. **PCG (Forest biome):**
   - Add PCG Volume in `DL_PCG_Biome_Forest`
   - Graph: surface sampler → mesh spawner (trees/rocks)
   - Exclude cells overlapping landmark spire
7. **Summit landmark:**
   - Place `AMistspireSummitMarker` actor on layer `DL_Landmarks_Authored`
   - `SummitId = summit_obelisk_prime`, set `OfficialAltitudeCm` to marker Z
   - Duplicate markers per [docs/WORLD_DESIGN.md](../../../docs/WORLD_DESIGN.md)
8. **Player start:** place `PlayerStart` in valley; set **Game Mode** `MistspireGameMode` in World Settings.
9. **HLOD:** Generate HLOD for distant cells (World Partition → HLOD).
10. **Building pockets:** author interior rooms at pocket coords — see [docs/BUILDINGS_AND_INTERIORS.md](../../../docs/BUILDINGS_AND_INTERIORS.md). Doors spawn at runtime from the world atlas.
11. **VR test:** VR Preview with OpenXR runtime active.

## Expand to full production grid

| Setting | Vertical slice | Full world |
|---------|----------------|------------|
| Cells | 16 × 16 | 128+ |
| Area | ~4 km² | 20–50 km² |
| Data Layers | 3 | 5+ |

See [docs/WORLD_DESIGN.md](../../../docs/WORLD_DESIGN.md).
