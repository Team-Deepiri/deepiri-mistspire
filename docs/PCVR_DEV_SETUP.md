# PCVR Development Setup

## Platforms

| Environment | Use |
|-------------|-----|
| Linux native | Primary: UE editor, Vulkan, Monado/SteamVR |
| WSL2 | Code, git, `xr-sandbox` — **no HMD** |
| Windows | Meta Link, SteamVR when Linux path fails |

## First open

1. Install UE **5.5+**.
2. Open `game/Mistspire.uproject`.
3. Build **Mistspire** + **MistspireOpenXRNative** when prompted.
4. Confirm plugins: OpenXR, OpenXRCore, MistspireOpenXRNative.

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
```

Enable on-screen altitude: `mistspire.ShowAltitudeHUD 1`

## Packaging (later)

- Configuration: **Shipping**
- Disable Live Coding
- Win64 + Linux once map assets exist
