# Main_WP — World Partition vertical slice

`Main_WP` and its World Partition externals live under `game/Content/Maps/` and are tracked with **Git LFS**. After clone: `git lfs install` then `git lfs pull`.

If LFS pointers are not pulled, the editor will show tiny pointer files instead of a real map — fix LFS before recreating assets.

## Open existing Main_WP

1. Open `game/Mistspire.uproject` in UE **5.8+**.
2. Open `/Game/Maps/Main_WP` (or let `DefaultEngine.ini` load it as the default map).
3. Confirm World Partition cells stream and Game Mode is `MistspireGameMode`.
4. **Play → VR Preview** with an active OpenXR runtime.

## Recreate or expand Main_WP

Use this only if you need a fresh map or a larger grid. Prefer editing the committed `Main_WP` when possible.

1. **File → New Level → Empty Open World** (World Partition enabled).
2. Save as `Content/Maps/Main_WP` (overwrite only intentionally).
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
   - Duplicate markers per [docs/gameplay/WORLD_DESIGN.md](../../../docs/gameplay/WORLD_DESIGN.md)
8. **Player start:** place `PlayerStart` in valley; set **Game Mode** `MistspireGameMode` in World Settings.
9. **HLOD:** Generate HLOD for distant cells (World Partition → HLOD).
10. **Building pockets:** author interior rooms at pocket coords — see [docs/gameplay/BUILDINGS_AND_INTERIORS.md](../../../docs/gameplay/BUILDINGS_AND_INTERIORS.md). Doors spawn at runtime from the world atlas.
11. **VR test:** VR Preview with OpenXR runtime active.
12. Commit map/externals via **Git LFS** (never commit raw binaries without LFS).

## Expand to full production grid

| Setting | Vertical slice | Full world |
|---------|----------------|------------|
| Cells | 16 × 16 | 128+ |
| Area | ~4 km² | 20–50 km² |
| Data Layers | 3 | 5+ |

See [docs/gameplay/WORLD_DESIGN.md](../../../docs/gameplay/WORLD_DESIGN.md).
