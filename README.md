# Project Mistspire

**Project Mistspire** is an experimental PCVR vertical exploration game: traverse a massive World Partition map and reach the **highest altitude** you can find.

```
  valley ──► red mesas ──► cloud forests ──► sky islands ──► orbital spire
                              │
                    personal best altitude (cm) = score
```

## Requirements

- Unreal Engine **5.8+** (Epic Launcher) — targets **Linux + Win64**
- Git LFS (`git lfs pull` for `Main_WP`)
- OpenXR runtime (SteamVR primary; see [headset guides](docs/setup/headsets/README.md))
- GPU with Vulkan 1.3 (RTX 40/50 series recommended)

## Quick start

| OS | Setup | Launch |
|----|-------|--------|
| **Linux** | `./setup.sh` | `./run.sh` |
| **Windows** | `powershell -File setup.ps1` | `.\run.ps1` |
| **WSL** | git/LFS only — VR on Windows or native Linux | — |

Full matrix: [docs/setup/PLATFORMS.md](docs/setup/PLATFORMS.md). Walkthrough: [docs/setup/DEV_BOOTSTRAP.md](docs/setup/DEV_BOOTSTRAP.md).

PCVR / headset detail: [PCVR development](docs/setup/PCVR_DEV_SETUP.md).

Debug console cheat sheet: [docs/gameplay/IMMERSION.md](docs/gameplay/IMMERSION.md) — e.g. `mistspire.TeleportUp 5000`.

## Repository layout

```
deepiri-mistspire/
├── game/                         # UE 5.8 project (Mistspire.uproject)
│   ├── Source/Mistspire/         # Game module: pawn, altitude, summits
│   ├── Content/Maps/             # Main_WP (Git LFS)
│   └── Plugins/MistspireOpenXRNative/
├── interaction_profiles/openxr/  # Meta, Valve, HTC, KHR bindings
├── native/xr-sandbox/            # OpenXR loader smoke test (no UE)
├── scripts/                      # Linux deps + runtime verify
└── docs/
```

## Documentation

- [Docs index](docs/README.md)
- [Architecture](docs/architecture/ARCHITECTURE.md)
- [Day 1 maintainer checklist](docs/setup/DAY1_MAINTAINER_CHECKLIST.md)
- [PCVR development](docs/setup/PCVR_DEV_SETUP.md) — [headset guides](docs/setup/headsets/README.md)
- [World design](docs/gameplay/WORLD_DESIGN.md)
- [OpenXR runtimes](docs/architecture/OPENXR_RUNTIMES.md)
- [Build / edit Main_WP](game/Content/Maps/README.md)

## WSL

Fine for editing and `xr-sandbox` builds. **Not** for USB headset VR — use native Linux or Windows for playtests.

## License

Apache 2.0 — [LICENSE](LICENSE).
