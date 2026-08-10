# Pipeline Step 5 — Buildings + Interior Volumes

Place structures along the ascent path:

1. **16 buildings** from `UMistspireWorldAtlasSubsystem` district list:
   - Placed manually or via `AMistspireBuildingEntrance` actor
   - Each needs an `AMistspireInteriorExit` at its interior pocket
2. **Pocket interiors** at X ≈ 50 km+ offset from world origin (out of WP streaming range)
3. **Door volumes:** Overlap trigger → `UMistspireInteriorSubsystem::EnterBuilding()`
4. Sheltered volumes must provide O₂ refill and audio reverb zone

For the authoritative atlas-seeded `BuildingId` list and pocket-coordinate layout, see **`docs/gameplay/BUILDINGS_AND_INTERIORS.md`**.
