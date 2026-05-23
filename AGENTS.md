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

## Build

```bash
./scripts/setup-linux-deps.sh
# Open game/Mistspire.uproject → compile C++ modules
```

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
