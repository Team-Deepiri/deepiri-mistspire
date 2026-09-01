# AGENTS.md — deepiri-mistspire

## Project

**Mistspire** — UE 5.8 PCVR game. Objective: reach the highest altitude on a World Partition map. **Non-VR mode** (keyboard/mouse) is supported for editor PIE and packaged builds (`-nonvr`).

## Key paths

| Path | Purpose |
|------|---------|
| `game/Mistspire.uproject` | Open this in Unreal Editor |
| `game/Source/Mistspire/` | Game module (pawn, altitude, summits) |
| `game/Plugins/MistspireOpenXRNative/` | OpenXR native bridge |
| `interaction_profiles/openxr/` | Controller bindings (Meta, Valve, HTC) |
| `docs/setup/PCVR_DEV_SETUP.md` | PCVR hub; per-headset guides under `docs/setup/headsets/` |
| `game/Config/DefaultEngine.ini` | Linux Vulkan SM5/SM6 + Win64 D3D12 SM5/SM6 |
| `docs/setup/NONVR_MODE.md` | Keyboard/mouse play without a headset |

## Rules for agents

1. Do **not** call `xrCreateInstance` / `xrCreateSession` in game code — use `FMistspireOpenXRAccess` + UE `IOpenXRHMD`.
2. Do **not** call `xrWaitFrame` from `MistspireOpenXRNative` in shipping paths.
3. OpenXR action set name: `mistspire_gameplay`.
4. Binary `.umap` / `.uasset` — Git LFS; document map steps in `game/Content/Maps/README.md`.
5. Repo layout is **flat** under `deepiri-mistspire/` (no nested `deepiri-cloudloom/`).
6. Keep project docs in sync with code: when adding or changing features, update the matching files under `docs/` (`gameplay/`, `architecture/`, `setup/`) and this `AGENTS.md` when paths, rules, systems, or setup steps change.
7. **Non-VR mode:** use `FMistspireInputMode::IsNonVRMode(World)`; CLI `-nonvr` (alias `-demoflat`) forces keyboard/mouse; `-forcvr` or `-forcevr` forces VR. Editor **Play** = non-VR; **VR Preview** = VR. Do not break either path when changing input or pawn code.

## Quick start

| OS | Command |
|----|---------|
| Linux | `./setup.sh` then `./run.sh` (headset prompts; `--non-interactive` for CI) |
| Windows | `powershell -File setup.ps1` then `.\run.ps1` (`-NonInteractive` for CI) |

Platform matrix: [docs/setup/PLATFORMS.md](docs/setup/PLATFORMS.md). Bootstrap: [docs/setup/DEV_BOOTSTRAP.md](docs/setup/DEV_BOOTSTRAP.md).

Manual / PCVR: [docs/setup/PCVR_DEV_SETUP.md](docs/setup/PCVR_DEV_SETUP.md). Non-VR: [docs/setup/NONVR_MODE.md](docs/setup/NONVR_MODE.md).

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
| 01 | `UMistspireBiomeMist` | 0–1 km | — |
| 02 | `UMistspireBiomeArid` | 1–3 km | — |
| 03 | `UMistspireBiomeForest` | 3–5 km | — |
| 04 | `UMistspireBiomeEmber` | 5–7 km | HeatExhaustion |
| 05 | `UMistspireBiomeCrystal` | 7–9 km | SharpShards |
| 06 | `UMistspireBiomeVoid` | 9–12 km | VoidSickness |
| 07 | `UMistspireBiomeTundra` | 12–14 km | Blizzard |
| 08 | `UMistspireBiomeAether` | 14–16 km | GravityAnomaly |
| 09 | `UMistspireBiomeSanctum` | 16–18 km | OxygenVacuum |
| 10 | `UMistspireBiomePinnacle` | 18–20 km | CosmicRadiation |

## Interaction profiles (OpenXR native)

| Path | Headsets |
|------|----------|
| `interaction_profiles/openxr/bindings_meta_quest.json` | Meta Quest |
| `interaction_profiles/openxr/bindings_valve_index.json` | Valve Index |
| `interaction_profiles/openxr/bindings_htc_vive.json` | HTC Vive |
| `interaction_profiles/openxr/bindings_khr_simple.json` | Fallback/simple |

Bindings include: `move`, `strafe`, `turn`, `grip`, `jump`, `climb`, `grapple`, `glider`, `menu`, `teleport`.

## Debug console

- `mistspire.AltitudeStats`
- `mistspire.TeleportUp 5000`
- `mistspire.ShowAltitudeHUD 1` (or `mistspire.ShowWristAltimeter 1`)
- Full command list: see [docs/gameplay/IMMERSION.md](docs/gameplay/IMMERSION.md).

## Immersion stack

See [docs/gameplay/IMMERSION.md](docs/gameplay/IMMERSION.md) — zones, narrative, beacon, companion orb, ghosts, pickups, persistence.

## World scale & interiors

- `UMistspireWorldAtlasSubsystem` — districts, buildings, POIs
- `UMistspireInteriorSubsystem` — enter/exit buildings (VR teleport)
- [docs/gameplay/BUILDINGS_AND_INTERIORS.md](docs/gameplay/BUILDINGS_AND_INTERIORS.md)
