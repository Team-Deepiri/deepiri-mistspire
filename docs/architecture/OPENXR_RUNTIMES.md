# OpenXR Runtimes

Mistspire uses abstract actions in `mistspire_gameplay`. The active runtime maps them to device hardware.

## Headsets

| Device | Interaction profile | Typical runtime |
|--------|---------------------|-----------------|
| Meta Quest (Link) | `/interaction_profiles/oculus/touch_controller` | Oculus OpenXR |
| Valve Index | `/interaction_profiles/valve/index_controller` | SteamVR OpenXR |
| HTC Vive | `/interaction_profiles/htc/vive_controller` | SteamVR OpenXR |
| Generic | `/interaction_profiles/khr/simple_controller` | Any |

Binding files: [interaction_profiles/openxr/](../../interaction_profiles/openxr/) — Meta Quest uses `bindings_meta_quest.json` (OpenXR path `/interaction_profiles/oculus/touch_controller`).

## Linux (Monado)

```bash
export XR_RUNTIME_JSON=/usr/share/openxr/openxr_monado.json
./scripts/verify-openxr-runtime.sh
./native/xr-sandbox/build/mistspire_xr_sandbox   # after cmake build
```

## Windows

Set active runtime in SteamVR or Meta Quest Developer Hub before launching UE VR Preview.

## UE integration

Enabled in `Mistspire.uproject`: OpenXR, MistspireOpenXRNative (OpenXRHandTracking optional).

`FMistspireOpenXRAccess` reads session handles via UE `IOpenXRHMD` — do not start a second OpenXR session in game code.
