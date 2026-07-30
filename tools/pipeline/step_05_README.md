# Pipeline Step 5 — Buildings + Interior Volumes

Place structures along the ascent path:

1. **16 buildings** from `UMistspireWorldAtlasSubsystem` district list:
   - Placed manually or via `AMistspireBuildingEntrance` actor
   - Each needs an `AMistspireInteriorExit` at its interior pocket
2. **Pocket interiors** at X ≈ 50 km+ offset from world origin (out of WP streaming range)
3. **Door volumes:** Overlap trigger → `UMistspireInteriorSubsystem::EnterBuilding()`
4. Sheltered volumes must provide O₂ refill and audio reverb zone

| Building | District |
|----------|----------|
| Valley_Haven_Lodge | 0 — Valley Haven |
| Valley_Forge | 0 — Valley Haven |
| Ridge_Shelter_Alpha | 2 — Ridge Approach |
| Mesa_Trading_Post | 3 — Arid Mesa |
| Cloud_Garden_Pavilion | 5 — Cloud Gardens |
| Ember_Outpost | 7 — Ember Frontier |
| Crystal_Overlook | 8 — Crystal Ridge |
| Void_Watchtower | 9 — Void Verge |
| Tundra_Bunker | 10 — Tundra Reach |
| Aether_Observatory | 11 — Aether Expanse |
| Sanctum_Chapel | 12 — Sanctum Hollow |
| Pinnacle_Shrine | 13 — Pinnacle Ascent |

**See:** `docs/gameplay/BUILDINGS_AND_INTERIORS.md`
