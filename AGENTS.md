# AGENTS.md — deepiri-mistspire

## Project

**Mistspire** — UE 5.5 PCVR game. Objective: reach the highest altitude on a World Partition map.

## Key paths

| Path | Purpose |
|------|---------|
| `game/Mistspire.uproject` | Open this in Unreal Editor |
| `game/Source/Mistspire/` | Game module (pawn, altitude, summits) |
| `game/Plugins/MistspireOpenXRNative/` | OpenXR native bridge |
| `interaction_profiles/openxr/` | Controller bindings (Meta, Valve, HTC) |
| `docs/PCVR_DEV_SETUP.md` | Linux / WSL / Windows VR dev |

## Rules for agents

1. Do **not** call `xrCreateInstance` / `xrCreateSession` in game code — use `FMistspireOpenXRAccess` + UE `IOpenXRCore`.
2. Do **not** call `xrWaitFrame` from `MistspireOpenXRNative` in shipping paths.
3. OpenXR action set name: `mistspire_gameplay`.
4. Binary `.umap` / `.uasset` — Git LFS; document map steps in `game/Content/Maps/README.md`.
5. Repo layout is **flat** under `deepiri-mistspire/` (no nested `deepiri-cloudloom/`).

## Quick start

```bash
./setup.sh          # Install deps + compile + launch editor
./run.sh            # Launch VR preview or packaged build
```

## Manual build

```bash
./scripts/setup-linux-deps.sh
# Open game/Mistspire.uproject → compile C++ modules
```

## Pipeline tools

| Path | Purpose |
|------|---------|
| `tools/pipeline/step_*_README.md` | 20-step world-authoring guide |
| `tools/pipeline/step_01_create_map.py` | UE Python script to auto-create Main_WP |

## World biomes (10 total, C++ subsystems)

| # | Subsystem | Altitude | Hazard |
|---|-----------|----------|--------|
| 01 | `UMistspireBiomeMist` | 0–2 km | — |
| 02 | `UMistspireBiomeArid` | 2–4 km | — |
| 03 | `UMistspireBiomeForest` | 4–6 km | — |
| 04 | `UMistspireBiomeEmber` | 6–8 km | HeatExhaustion |
| 05 | `UMistspireBiomeCrystal` | 8–10 km | SharpShards |
| 06 | `UMistspireBiomeVoid` | 10–12 km | VoidSickness |
| 07 | `UMistspireBiomeTundra` | 12–14 km | Blizzard |
| 08 | `UMistspireBiomeAether` | 14–16 km | GravityAnomaly |
| 09 | `UMistspireBiomeSanctum` | 16–18 km | OxygenVacuum |
| 10 | `UMistspireBiomePinnacle` | 18–20 km | CosmicRadiation |

## Interaction profiles (OpenXR native)

| Path | Headsets |
|------|----------|
| `interaction_profiles/openxr/bindings_meta_quest*.json` | Meta Quest 2/3/Pro |
| `interaction_profiles/openxr/bindings_valve_index.json` | Valve Index |
| `interaction_profiles/openxr/bindings_htc_vive.json` | HTC Vive |
| `interaction_profiles/openxr/bindings_khr_simple.json` | Fallback/simple |

Bindings include: `move`, `strafe`, `turn`, `grip`, `jump`, `climb`, `grapple`, `glider`, `menu`, `teleport`.

## Debug console

- `mistspire.AltitudeStats`
- `mistspire.TeleportUp 5000`
- `mistspire.ShowAltitudeHUD 1`
- `mistspire.SetWeather 0` (0=Clear, 1=MistStorm, 2=Electric, 3=ZenithGlow)
- `mistspire.RefillSurvival`
- `mistspire.SaveProgress` / `mistspire.LoadProgress`

## Immersion stack

See [docs/IMMERSION.md](docs/IMMERSION.md) — zones, narrative, beacon, companion orb, ghosts, pickups, persistence.

## World scale & interiors

- `UMistspireWorldAtlasSubsystem` — districts, buildings, POIs
- `UMistspireInteriorSubsystem` — enter/exit buildings (VR teleport)
- [docs/BUILDINGS_AND_INTERIORS.md](docs/BUILDINGS_AND_INTERIORS.md)
