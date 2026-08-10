# PCVR Development Setup

Hub for getting Mistspire running in **VR Preview**. For headset-specific steps, use [headsets/README.md](headsets/README.md).

**Primary OpenXR runtime:** SteamVR OpenXR (unless your connection path requires Meta Link or VDXR).

## Platforms and bootstrap

| Doc | Purpose |
|-----|---------|
| [PLATFORMS.md](PLATFORMS.md) | Linux vs Windows vs WSL — what runs where |
| [DEV_BOOTSTRAP.md](DEV_BOOTSTRAP.md) | One-run setup flow per OS |
| [setup.ps1](../../setup.ps1) / [setup.sh](../../setup.sh) | Automated repo setup |

## Platforms (summary)

| Environment | Setup script | VR playtest |
|-------------|--------------|-------------|
| Linux native | `./setup.sh` | Yes |
| Windows native | `.\setup.ps1` | Yes |
| WSL2 | git/LFS only | **No HMD** |

## Flow

1. **Project** — steps below (UE, LFS, plugins).
2. **Headset** — [headsets/README.md](headsets/README.md) → pick your device and connection path.
3. **VR session** — [OPENXR_DEV_COMMON.md](OPENXR_DEV_COMMON.md) (runtime, VR Preview, console).
4. **Validation** — [DAY1_MAINTAINER_CHECKLIST.md](DAY1_MAINTAINER_CHECKLIST.md).

## First open

1. Install UE **5.8+**.
2. `git lfs pull` (required for `Main_WP`).
3. Open `game/Mistspire.uproject`.
4. Build **Mistspire** + **MistspireOpenXRNative** when prompted.
5. Confirm plugins: OpenXR, MistspireOpenXRNative (OpenXRHandTracking optional).

## VR Preview (summary)

1. Complete headset setup for your path ([headsets/](headsets/README.md)).
2. Set the correct OpenXR runtime ([OPENXR_DEV_COMMON.md](OPENXR_DEV_COMMON.md)).
3. Linux: `./scripts/verify-openxr-runtime.sh` (optional).
4. Editor → **Play** → **VR Preview** on `Main_WP`.

## Headsets

| Guide | Devices |
|-------|---------|
| [meta_quest.md](headsets/meta_quest.md) | Quest; Rift legacy |
| [steamvr_lighthouse.md](headsets/steamvr_lighthouse.md) | Index, Vive tethered |
| [htc_vive_focus.md](headsets/htc_vive_focus.md) | Focus 3, XR Elite |
| [windows_mixed_reality.md](headsets/windows_mixed_reality.md) | WMR, HP Reverb (Oasis) |
| [pico.md](headsets/pico.md) | Pico 4 / Neo / Ultra |
| [steam_frame.md](headsets/steam_frame.md) | Steam Frame (upcoming) |
| [generic.md](headsets/generic.md) | Other |
| [streaming_options.md](headsets/streaming_options.md) | Virtual Desktop, Steam Link, WiVRn |

Bindings (authoring): [`interaction_profiles/openxr/README.md`](../../interaction_profiles/openxr/README.md) — runtime maps hardware → `mistspire_gameplay` actions.

## Debug

```
mistspire.AltitudeStats
mistspire.TeleportUp 5000
mistspire.ShowAltitudeHUD 1
mistspire.ShowWristAltimeter 1  # alias
```

Full console commands: [docs/gameplay/IMMERSION.md](../gameplay/IMMERSION.md).

## Packaging (later)

- Configuration: **Shipping**
- Disable Live Coding
- Win64 + Linux (map assets are in-repo via LFS)
