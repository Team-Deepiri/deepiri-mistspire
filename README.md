# deepiri-mistspire

**Mistspire** A PCVR vertical exploration game: traverse a massive World Partition map and reach the **highest altitude** you can find.

```
  valley ──► red mesas ──► cloud forests ──► sky islands ──► orbital spire
                              │
                    personal best altitude (cm) = score
```

## Requirements

- Unreal Engine **5.8+** (Epic Launcher)
- Git LFS (`git lfs pull` for `Main_WP`)
- OpenXR runtime (SteamVR, Oculus, or Monado on Linux)
- GPU with Vulkan 1.3 (RTX 40/50 series recommended)

## Quick start

```bash
./scripts/setup-linux-deps.sh
./scripts/verify-openxr-runtime.sh
git lfs pull
```

Open `game/Mistspire.uproject` in the UE editor, allow C++ compile, then **Play → VR Preview**.

Console (editor or PIE):

| Command | Action |
|---------|--------|
| `mistspire.AltitudeStats` | Log current vs personal-best altitude |
| `mistspire.TeleportUp 5000` | Debug teleport +Z (cm) |

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
- [PCVR development](docs/setup/PCVR_DEV_SETUP.md)
- [World design](docs/gameplay/WORLD_DESIGN.md)
- [OpenXR runtimes](docs/architecture/OPENXR_RUNTIMES.md)
- [Build / edit Main_WP](game/Content/Maps/README.md)

## WSL

Fine for editing and `xr-sandbox` builds. **Not** for USB headset VR — use native Linux or Windows for playtests.

## License

Apache 2.0 — [LICENSE](LICENSE).
