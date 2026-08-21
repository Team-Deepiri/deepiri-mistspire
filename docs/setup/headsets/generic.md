# Generic / other headsets

Fallback setup when your headset is not covered by a dedicated Mistspire guide.

Binding: [`bindings_khr_simple.json`](../../../interaction_profiles/openxr/bindings_khr_simple.json) — OpenXR profile `/interaction_profiles/khr/simple_controller`.

## What you need

- Vendor PC software or SteamVR support for your headset.
- GPU and cables per manufacturer specs.

## Choose your connection path

| Situation | Recommended runtime |
|-----------|---------------------|
| Headset sold for SteamVR | **SteamVR OpenXR** |
| Vendor ships OpenXR runtime | Vendor runtime, or SteamVR if supported |
| Standalone Android headset | Virtual Desktop, Steam Link, or WiVRn — see [streaming_options.md](streaming_options.md) |
| Unknown | **SteamVR OpenXR** first |

## Windows

1. Install the manufacturer's PC VR suite **or** Steam + SteamVR if the headset is SteamVR-compatible.
2. Complete headset pairing and room calibration.
3. Set **SteamVR as OpenXR runtime** when SteamVR is the stack ([OPENXR_DEV_COMMON.md](../OPENXR_DEV_COMMON.md#set-steamvr-as-openxr-runtime)).
4. If the vendor provides a dedicated OpenXR runtime, use their “set as active runtime” control instead.
5. UE → VR Preview per [OPENXR_DEV_COMMON.md](../OPENXR_DEV_COMMON.md).

## Linux

1. Try **SteamVR OpenXR** if your headset works with Linux SteamVR.
2. Else try **Monado** with `XR_RUNTIME_JSON` — see [OPENXR_RUNTIMES.md](../../architecture/OPENXR_RUNTIMES.md) and `./scripts/verify-openxr-runtime.sh`.
3. Standalone headsets: **WiVRn** or **Steam Link** — [streaming_options.md](streaming_options.md).

## Multiple runtimes installed

Only one OpenXR runtime is active at a time.

- Use SteamVR's OpenXR tab, vendor settings, or [OpenXR Explorer](https://github.com/mbucchia/OpenXR-Runtime-Debugger) to switch.
- Advanced: `XR_RUNTIME_JSON` environment variable (see [OPENXR_DEV_COMMON.md](../OPENXR_DEV_COMMON.md#advanced-runtime-override)).

## Mistspire input

Without a dedicated binding JSON, `mistspire_gameplay` actions map through the **KHR simple controller** profile. Upper-band biomes and full control surface may feel incomplete until a headset-specific JSON is added under `interaction_profiles/openxr/`.

## Troubleshooting

| Issue | Fix |
|-------|-----|
| Headset not in matrix | Add a guide in a future PR if the team standardizes on that hardware |
| Wrong profile | Check active runtime matches headset vendor |
| Partial controls | Expected with KHR simple; contribute `bindings_*.json` |

## Official links

- [Epic OpenXR prerequisites](https://dev.epicgames.com/documentation/en-us/unreal-engine/openxr-prerequisites-in-unreal-engine)
- [Khronos OpenXR](https://www.khronos.org/openxr/)
- [interaction_profiles/openxr/](../../../interaction_profiles/openxr/)
