# Pipeline Step 3 — Biome Data Layers + PCG Setup

1. Open Main_WP → World Partition → Data Layers
2. Create one layer per biome:
   - `DL_PCG_Biome_Mist` (0–1 km)
   - `DL_PCG_Biome_Arid` (1–3 km)
   - `DL_PCG_Biome_Forest` (3–5 km)
   - `DL_PCG_Biome_Ember` (5–7 km)
   - `DL_PCG_Biome_Crystal` (7–9 km)
   - `DL_PCG_Biome_Void` (9–12 km)
   - `DL_PCG_Biome_Tundra` (12–14 km)
   - `DL_PCG_Biome_Aether` (14–16 km)
   - `DL_PCG_Biome_Sanctum` (16–18 km)
   - `DL_PCG_Biome_Pinnacle` (18–20 km)
3. Assign each layer a landscape-visibility toggle
4. Set up PCG volume per biome band with biome-specific spawn rules

**See:** `docs/gameplay/WORLD_DESIGN.md`, `docs/gameplay/biomes/`
