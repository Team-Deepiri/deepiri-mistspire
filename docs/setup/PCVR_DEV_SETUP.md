# PCVR Development Setup

## Platforms

| Environment | Use |
|-------------|-----|
| Linux native | Primary: UE editor, **Vulkan SM5/SM6**, SteamVR / Monado |
| WSL2 | Code, git, `xr-sandbox` — **no HMD** |
| Windows | Meta Link, SteamVR; **D3D12 SM5/SM6** (engine default RHI) |

Project cook targets: **Win64 + Linux** (`game/Mistspire.uproject`). RHI lists live in `game/Config/DefaultEngine.ini`. See [ARCHITECTURE.md](../architecture/ARCHITECTURE.md#engine-targets-win64--linux).

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

See [`interaction_profiles/openxr/README.md`](../../interaction_profiles/openxr/README.md) for the binding-file → device mapping for `mistspire_gameplay`.

Runtime maps hardware → `mistspire_gameplay` actions.

## Debug

```
mistspire.AltitudeStats
mistspire.TeleportUp 5000
mistspire.ShowAltitudeHUD 1
mistspire.ShowWristAltimeter 1  # alias
```

Full console commands: [docs/gameplay/IMMERSION.md](../gameplay/IMMERSION.md).

Day-1 smoke path: [DAY1_MAINTAINER_CHECKLIST.md](DAY1_MAINTAINER_CHECKLIST.md).

## Packaging (later)

- Configuration: **Shipping**
- Disable Live Coding
- Win64 + Linux (Vulkan SM5/SM6 on Linux; D3D12 SM5/SM6 on Win64; map assets via LFS)
