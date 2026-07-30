# Pipeline Step 3 — Biome Data Layers + PCG Setup

1. Open Main_WP → World Partition → Data Layers
2. Create one layer per biome:
   - `DL_Biome_Mist` (0–2 km)
   - `DL_Biome_Arid` (2–4 km)
   - `DL_Biome_Forest` (4–6 km)
   - `DL_Biome_Ember` (6–8 km)
   - `DL_Biome_Crystal` (8–10 km)
   - `DL_Biome_Void` (10–12 km)
   - `DL_Biome_Tundra` (12–14 km)
   - `DL_Biome_Aether` (14–16 km)
   - `DL_Biome_Sanctum` (16–18 km)
   - `DL_Biome_Pinnacle` (18–20 km)
3. Assign each layer a landscape-visibility toggle
4. Set up PCG volume per biome band with biome-specific spawn rules

**See:** `docs/gameplay/WORLD_DESIGN.md`, `docs/gameplay/biomes/`
