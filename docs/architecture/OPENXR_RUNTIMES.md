# OpenXR Runtimes

Mistspire uses abstract actions in `mistspire_gameplay`. The active runtime maps them to device hardware.

**Primary runtime for Mistspire:** **SteamVR OpenXR** unless a connection path requires a vendor or VDXR runtime. See [setup/headsets/README.md](../setup/headsets/README.md).

## Headsets (interaction profiles)

| Device | Interaction profile | Typical runtime |
|--------|---------------------|-----------------|
| Meta Quest (Link) | `/interaction_profiles/oculus/touch_controller` | Meta Horizon Link; or SteamVR (Steam Link) |
| Valve Index | `/interaction_profiles/valve/index_controller` | SteamVR OpenXR |
| HTC Vive | `/interaction_profiles/htc/vive_controller` | SteamVR OpenXR |
| Pico / WMR / Focus / generic | `/interaction_profiles/khr/simple_controller` | SteamVR OpenXR (preferred) |

Binding files: [interaction_profiles/openxr/](../../interaction_profiles/openxr/). Per-headset setup: [setup/headsets/](../setup/headsets/).

## Windows

Set the active runtime per your connection path before UE VR Preview:

- **SteamVR** (Index, Vive, Steam Link, PICO Connect, Oasis/WMR): SteamVR → Settings → OpenXR
- **Meta Quest Link:** Meta Horizon Link → Settings → General
- **Virtual Desktop:** VDXR in streamer Options, or SteamVR

Details: [setup/OPENXR_DEV_COMMON.md](../setup/OPENXR_DEV_COMMON.md).

## Linux

**SteamVR + Steam Link** are supported on Linux with current Steam and GPU drivers — same SteamVR OpenXR setting as Windows.

**Monado** (tethered PCVR without SteamVR):

```bash
export XR_RUNTIME_JSON=/usr/share/openxr/openxr_monado.json
./scripts/verify-openxr-runtime.sh
./native/xr-sandbox/build/mistspire_xr_sandbox   # after cmake build
```

**WiVRn** (standalone headset streaming from Linux PC): see [setup/headsets/streaming_options.md](../setup/headsets/streaming_options.md).

## UE integration

Enabled in `Mistspire.uproject`: OpenXR, MistspireOpenXRNative (OpenXRHandTracking optional).

`FMistspireOpenXRAccess` reads session handles via UE `IOpenXRHMD` — do not start a second OpenXR session in game code.
