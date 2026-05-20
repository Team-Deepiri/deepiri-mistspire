# deepiri-mistspire

**Mistspire** — *mist* + *spire*. A PCVR vertical exploration game: traverse a massive World Partition map and reach the **highest altitude** you can find.

```
  valley ──► red mesas ──► cloud forests ──► sky islands ──► orbital spire
                              │
                    personal best altitude (cm) = score
```

## Requirements

- Unreal Engine **5.5+** (Epic Launcher)
- OpenXR runtime (SteamVR, Oculus, or Monado on Linux)
- GPU with Vulkan 1.3 (RTX 40/50 series recommended)

## Quick start

```bash
./scripts/setup-linux-deps.sh
./scripts/verify-openxr-runtime.sh
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
├── game/                         # UE5 project (Mistspire.uproject)
│   ├── Source/Mistspire/         # Game module: pawn, altitude, summits
│   └── Plugins/MistspireOpenXRNative/
├── interaction_profiles/openxr/  # Meta, Valve, HTC, KHR bindings
├── native/xr-sandbox/            # OpenXR loader smoke test (no UE)
├── scripts/                      # Linux deps + runtime verify
└── docs/
```

## Documentation

- [Architecture](docs/ARCHITECTURE.md)
- [PCVR development](docs/PCVR_DEV_SETUP.md)
- [World design](docs/WORLD_DESIGN.md)
- [OpenXR runtimes](docs/OPENXR_RUNTIMES.md)
- [Build Main_WP map](game/Content/Maps/README.md)

## WSL

Fine for editing and `xr-sandbox` builds. **Not** for USB headset VR — use native Linux or Windows for playtests.

## License

Apache 2.0 — [LICENSE](LICENSE).
