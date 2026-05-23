# Buildings & interiors

Mistspire uses **pocket interiors**: each enterable building teleports the VR pawn to a dedicated interior cell far from the open world, so you can author rooms without fighting World Partition overlap.

## How it works

1. `UMistspireWorldAtlasSubsystem` seeds **12 districts**, **16 buildings**, and **7 POIs** at production scale (~7 km footprint, expandable to 50 km²).
2. On `StartPlay`, `SpawnAuthoredWorldMarkers()` places:
   - `AMistspireBuildingEntrance` at each door
   - `AMistspireInteriorExit` inside each pocket
   - `AMistspirePOIMarker` at viewpoints and lore spots
3. Walk into the **door volume** → `UMistspireInteriorSubsystem` saves your return point and teleports you inside.
4. Walk into the **exit volume** → return to the door.

## Pocket interior coordinates

Interiors are spaced along **+X** starting at `X = 5,000,000 cm` (50 km), **800 m** apart per building. When sculpting `Main_WP`, duplicate a room template at:

| Building ID | Approx pocket origin (cm) |
|-------------|----------------------------|
| `building_valley_inn` | (5000000, 0, 20000) |
| `building_valley_gear` | (5800000, 0, 20000) |
| … | +800000 X per index |

See `SeedProductionWorld()` in `MistspireWorldAtlasSubsystem.cpp` for the full list.

## Authoring in editor

### Option A — Use spawned doors (default)

1. Play in VR — doors and POIs spawn automatically.
2. Build interior meshes at the pocket coordinates above.
3. Scale door volumes on spawned `AMistspireBuildingEntrance` if needed.

### Option B — Hand-place entrances

1. Place `AMistspireBuildingEntrance`, set `BuildingId` to match atlas (e.g. `building_mist_tea`).
2. Place matching interior geometry at the pocket location from the atlas.
3. Place `AMistspireInteriorExit` just inside the room.

### Extra detail

- `AMistspireLoreShard` — collectible text in alleys and rooms
- `AMistspireRestShelter`, `AMistspireOxygenCanister` — survival inside districts
- District flavor lines fire when crossing bounds (horizontal + altitude)

## Debug

```
mistspire.TeleportDistrict 0    # Valley Haven
mistspire.ExitInterior
mistspire.RespawnWorldMarkers
```

## Production scale

| Target | Horizontal | Vertical | Districts | Building slots |
|--------|------------|----------|-----------|----------------|
| Slice | 4 km² | 8 km | 12 seeded | 16 enterable |
| Full | 20–50 km² | 12+ km | 12+ | 100+ |

Expand `SeedProductionWorld()` and add World Partition cells per [WORLD_DESIGN.md](WORLD_DESIGN.md).
