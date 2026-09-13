# Demo World — Demo Spire

Owner-recording vertical slice for Mistspire. Runtime geometry is spawned by `AMistspireDemoClimbScaffold` when `-demoworld` / `mistspire.DemoMode 1` is active (or via `mistspire.RebuildDemoScaffold`).

## Fantasy

A compact **helix of stone stations** rising through ten mist bands. Empty kilometers between stations stay fog; each station owns a short climb vignette so VR stays readable.

## Layout (source of truth)

Shared constants: [`MistspireDemoSpireLayout.h`](../../game/Source/Mistspire/Public/MistspireDemoSpireLayout.h)

| Param | Value |
|-------|-------|
| Helix radius | 2500 cm |
| Angle step | 36° (10 stations) |
| Valley floor | Z ≈ 0 |
| PlayerStart | near (0, 0, 200) |
| Vignette under pad | 5000 cm |
| Approach | Walkable stairs 40 cm rise, 450° helix, StartRadius 2200 |
| Grapple shaft | Floating pads every 45 m, ±400 Y stagger |
| Station vignette | Walkable stairs 40 cm × 12 radial into pad |
| Tour landing clearance | 120 cm above pad |

| Idx | Biome | Z (cm) | Summit ID |
|-----|-------|--------|-----------|
| 0 | Mist | 50 000 | `summit_valley_gate` |
| 1 | Arid | 200 000 | `summit_mesa_crown` |
| 2 | Forest | 400 000 | `summit_cloud_garden` |
| 3 | Ember | 600 000 | `summit_ember_crown` |
| 4 | Crystal | 800 000 | `summit_rift_observatory` |
| 5 | Void | 1 050 000 | `summit_spire_cathedral` |
| 6 | Tundra | 1 300 000 | `summit_obelisk_prime` |
| 7 | Aether | 1 500 000 | `summit_aether_span` |
| 8 | Sanctum | 1 700 000 | `summit_sanctum_crown` |
| 9 | Pinnacle | 1 900 000 | `summit_orbital_needle` |

`mistspire.DemoTour N` lands on station pad centers from this table.

## Modules (greybox)

Engine BasicShapes (cube / cylinder) with biome-tint MIDs:

- Valley floor + Valley Gate arch + brazier light
- Walkable approach stairs (40 cm) to 60 m, then grapple pad highway to Mist
- Central lit mast + per-band beacons for vertical readability
- Per station: large pad, rails, jump stairs into pad, silhouette, point light
- Distant non-colliding silhouette pillars for parallax

## Immersion props (spawned with scaffold)

- `AMistspireSummitMarker` on every station (`ReachRadiusCm` 700)
- `AMistspireRestShelter` + `AMistspireOxygenCanister` on Ember+ (idx ≥ 3)
- `AMistspireWindCrystal` on Forest, Aether, Pinnacle
- Lore shard at Mist valley
- Valley spawn teleport under `-demoworld`

## VR / traversal notes

- Pawn budget: step 45 cm, jump apex ~88 cm, grapple 80 m, no lateral climb
- Approach = walkable stairs; Mist ascent after 60 m = **grapple highway** between floating pads
- Station vignettes = jump stairs into the summit pad (no overhanging climb walls)
- Pads ≥ 250 cm; station pads 600 cm with comfort rails
- Recording: walk Mist stairs, grapple a few shaft pads, `DemoTour` for upper biomes

## Console

```
mistspire.RebuildDemoScaffold
mistspire.DemoTour 0..9
mistspire.DemoTour -1
mistspire.ApplyDemoPresentation
```

## MCP polish (optional)

With editor MCP up, place/adjust `PlayerStart`, folder actors under `DemoSpire/*`, and lights on `Main_WP`. Bulk geometry stays C++ so Git LFS map churn stays low.

## Out of scope

Production PCG meshes, full atlas district dress, Valley of the Ancient, continuous 19 km hand-climb (use DemoTour between stations for recording).
