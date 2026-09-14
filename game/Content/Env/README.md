# Mistspire Env Kit — staging + Fab cart

Hybrid demo dress assets. Fab packs are **in project Content** and verified loadable (2026-09-13).

## Status

| Item | Path | Status |
|------|------|--------|
| Mountain Tops (Arghanion) | `/Game/Iceland_Environment` | **Verified** — 78 uassets, 14 SM, materials/textures load |
| Rock Collection 04 (Shadowmire) | `/Game/Rock_Collection_04` | **Verified** — 41 uassets, SM_Rock_01…07 + MI each |
| Modular Rural Cabins (Maarten Hof) | `/Game/Modular_Rural_Cabin` | **Verified** — 582 uassets; Modular/Foliage/Props |
| Poly Haven CC0 staging | `Content/Env/Import/` | On disk only (not yet Interchange-imported) |
| Fab plugin in `.uproject` | Editor | Enabled |

Content Browser: **All → Content** shows the three pack folders above (plus existing Maps/Env/…).

## Verified sample loads (editor MCP)

| Asset | Class | Load |
|-------|-------|------|
| `/Game/Iceland_Environment/Static_Meshes/SM_Mountain_01` | StaticMesh | OK |
| `/Game/Iceland_Environment/Static_Meshes/SM_Iceland_Eroded_Mountain` | StaticMesh | OK |
| `/Game/Iceland_Environment/Static_Meshes/SM_Iceland_Crest` | StaticMesh | OK |
| `/Game/Rock_Collection_04/Meshes/Rock_01/StaticMeshes/SM_Rock_01` | StaticMesh | OK |
| `/Game/Rock_Collection_04/Meshes/Rock_04/StaticMeshes/SM_Rock_04` | StaticMesh | OK |
| `/Game/Rock_Collection_04/Meshes/Rock_01/Materials/MI_Rock_01` | MaterialInstanceConstant | OK |
| `/Game/Modular_Rural_Cabin/Meshes/Modular/Wall_4m` | StaticMesh | OK |
| `/Game/Modular_Rural_Cabin/Meshes/Modular/Porch_4x4m` | StaticMesh | OK |
| `/Game/Modular_Rural_Cabin/Meshes/Modular/Door_01` | StaticMesh | OK |
| `/Game/Modular_Rural_Cabin/Meshes/Foliage/SM_Pine_Tree_01` | StaticMesh | OK |
| `/Game/Modular_Rural_Cabin/Meshes/Foliage/Grass_Patch_1` | StaticMesh | OK |

No linker errors for these packs in `Mistspire.log` at verify time. Editor rebuilt `SM_Mountain_01` + Iceland textures on first open (expected).

### Pack inventory

**Iceland_Environment (~1.0 GB)** — Mountain Tops  
- Meshes: `SM_Mountain_01`…`09`, `SM_Iceland_*`, plateaus/crest  
- Materials: `M_Iceland_Mountains`, `MI_Iceland_Mountains_01`…`06`, landscape `M_AutoLandscape` / `MI_Landscape_*`  
- Example maps: `Example_Map/Lv_Mountain_Tops_Elevation`, `Lv_Mountain_Tops_VT`

**Rock_Collection_04 (~0.67 GB)**  
- `Meshes/Rock_01`…`Rock_07` each with `SM_Rock_##`, `MI_Rock_##`, A/N/ORM textures  
- Master: `MasterMaterial/MM_Rocks`  
- Demo: `Demo/Demo_01`

**Modular_Rural_Cabin (~1.6 GB)**  
- Meshes: Foliage 23, Modular 60, Props 104, Unique 4, Utility 1 (192 under Meshes)  
- Materials: Masters (`MM_Basic`, `MM_Foliage`, …) + Instances + Decals (109 under Materials)  
- Showcase maps under `Maps/` (Rural_Cabins, Modular_Showcase, …)

### Style notes (authoring)

- Cabin foliage pines are **stylized / low-poly planes** — fine for mid-distance dress; prefer Iceland/Rock photoreal pieces for close-up cliffs.
- Iceland mountain meshes are large hero cliffs — scale carefully on Demo Spire / valley strip.
- Quixel “Free” remains **UEFN-only** for many listings; these three packs are the UE kit.

## Fab listing IDs (claimed)

| Role | Listing | UUID |
|------|---------|------|
| Mountains | Mountain Tops | `8cdfadfb-5fa6-4b4e-99db-1a02b01295c2` |
| Rocks | Rock Collection 04 | `a51e61ac-98fa-4c54-ab23-fc533687afb7` |
| Inn / cabin | Modular Rural Cabins | `508fe84a-4976-4cfe-9a40-c2b9533da601` |

## Poly Haven CC0 — `Import/` (not verified as uassets)

Still raw glTF/JPG/HDR under `Content/Env/Import/` (gitignored). Import via Interchange when needed for filler surfaces.

## Next (authoring)

1. Live demo: `-demoworld` / `mistspire.RebuildDemoScaffold` — scaffold soft-loads Fab dress.
2. Main_WP valley strip actors live under outliner folder `DL_Landmarks_Authored` (`DemoEnv_*`). Iceland hero mountains stay distant so their collision does not trap origin; PlayerStart / demo spawn is on `DemoEnv_RockApproach` (~-500, 280, 380).
3. Save Main_WP (LFS) after strip edits; scaffold itself stays runtime-only.
