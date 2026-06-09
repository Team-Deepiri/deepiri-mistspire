# Pipeline Step 4 — Ground Cover + Vegetation

Per biome band, place PCG spawners:

| Biome | Cover | Key Assets |
|-------|-------|------------|
| Mist | Lichen, fog grass, wetland reeds | `SM_Mist_*` |
| Arid | Sparse scrub, tumbleweed, cracked ground | `SM_Arid_*` |
| Forest | Moss, ferns, trees, vines | `SM_Forest_*` |
| Ember | Ash, cinder plants, lava glow flora | `SM_Ember_*` |
| Crystal | Sharp shards, reflective ground cover | `SM_Crystal_*` |
| Void | Null grass, obsidian fragments | `SM_Void_*` |
| Tundra | Ice crystal, snow tufts, hardy moss | `SM_Tundra_*` |
| Aether | Floating particulates, light wisps | `SM_Aether_*` |
| Sanctum | White marble ground cover, glow vines | `SM_Sanctum_*` |
| Pinnacle | Star-dusted lichen, cosmic residue | `SM_Pinnacle_*` |

1. Add PCG volume → assign spawner graph per biome layer
2. Use landscape layer weights for distribution
3. Budget: < 500k instances total at any view
