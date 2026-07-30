# PCVR Development Setup

## Platforms

| Environment | Use |
|-------------|-----|
| Linux native | Primary: UE editor, Vulkan, Monado/SteamVR |
| WSL2 | Code, git, `xr-sandbox` — **no HMD** |
| Windows | Meta Link, SteamVR when Linux path fails |

## First open

1. Install UE **5.8+**.
2. `git lfs pull` (required for `Main_WP`).
3. Open `game/Mistspire.uproject`.
4. Build **Mistspire** + **MistspireOpenXRNative** when prompted.
5. Confirm plugins: OpenXR, MistspireOpenXRNative (OpenXRHandTracking optional).

## VR Preview

1. `./scripts/verify-openxr-runtime.sh`
2. Wear headset, set active OpenXR runtime for your device.
3. Editor → **Play** → **VR Preview**.

## Headsets (agnostic actions)

Interaction JSON under `interaction_profiles/openxr/`:

- Meta Quest Touch
- Valve Index
- HTC Vive
- Khronos simple controller

Runtime maps hardware → `mistspire_gameplay` actions.

## Debug

```
mistspire.AltitudeStats
mistspire.TeleportUp 10000
mistspire.ShowAltitudeHUD 1
mistspire.SetWeather 0
mistspire.RefillSurvival
mistspire.SaveProgress
mistspire.LoadProgress
```

Day-1 smoke path: [DAY1_MAINTAINER_CHECKLIST.md](DAY1_MAINTAINER_CHECKLIST.md).

## Packaging (later)

- Configuration: **Shipping**
- Disable Live Coding
- Win64 + Linux (map assets are in-repo via LFS)
