# Pipeline Step 7 — Lighting + Sky Atmosphere

1. Add `SkyAtmosphere` + `VolumetricCloud` + `SkyLight` to Main_WP
2. Configure **sun direction** to cast dramatic raking light along the spire
3. Fog settings:
   - Exponential Height Fog at origin, falloff scaled for 20 km vertical
   - Tint fog per biome band via Blueprint (overlap volumes)
4. Add a `DirectionalLight` with cast shadows (dynamic for VR)
5. **Performance:** Use static lighting where possible, `r.VolumetricFog=0` on low

**Lighting per biome:**
| Biome | Sun tint | Fog color |
|-------|----------|-----------|
| Mist (0–2 km) | Grey-white | Pale grey |
| Arid (2–4 km) | Warm amber | Dusty tan |
| Forest (4–6 km) | Green dappled | Soft green |
| Ember (6–8 km) | Orange-red | Ash grey |
| Crystal (8–10 km) | Cool blue | Ice blue |
| Void (10–12 km) | Purple-black | Dark violet |
| Tundra (12–14 km) | Pale white | Frost grey |
| Aether (14–16 km) | Silver-white | Luminous white |
| Sanctum (16–18 km) | Golden-white | Warm white |
| Pinnacle (18–20 km) | Star-blue | Deep space |

Set in `UMistspireEnvironmentSubsystem` via console variable overrides.
