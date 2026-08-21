# Pipeline Step 11 — Beacon + POI Markers

1. Place `AMistspirePOIMarker` actors at notable locations:
   - Viewpoints overlooking biome transitions
   - Lore plaques (ancient carvings, ruins)
   - Landmarks visible from distance

2. Place `AMistspireLoreShard` for collectible narrative fragments

3. Configure `UMistspireBeaconSubsystem`:
   - Set target summit ID for the beacon bearing
   - Beacon pulse rate increases within 500 m of target
   - Wrist HUD shows ▲ distance + bearing

**POI type flags:**
| Type | Auto-discovered | Gives XP |
|------|----------------|----------|
| Viewpoint | Yes (on sight) | No |
| Lore Plaque | Yes (on interact) | Yes |
| Landmark | Yes (on sight) | No |
| Lore Shard | Yes (on collect) | Yes |

**See:** `UMistspireWorldAtlasSubsystem` — POI auto-spawn districts list
