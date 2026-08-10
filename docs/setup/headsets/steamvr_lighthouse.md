# Valve Index and HTC Vive (tethered)

PCVR setup for **tethered** headsets: Valve Index, HTC Vive, Vive Pro, and similar Lighthouse / SteamVR kits.

**Not** the standalone Vive Focus line — see [htc_vive_focus.md](htc_vive_focus.md).

Bindings:
- Index: [`bindings_valve_index.json`](../../../interaction_profiles/openxr/bindings_valve_index.json) — `/interaction_profiles/valve/index_controller`
- Vive: [`bindings_htc_vive.json`](../../../interaction_profiles/openxr/bindings_htc_vive.json) — `/interaction_profiles/htc/vive_controller`

## What you need

- Headset, controllers, base stations (or inside-out kit per model).
- DisplayPort/USB to the GPU (not motherboard video).
- [Steam](https://store.steampowered.com/) + **SteamVR** installed.

## Choose your connection path

| Path | Devices | OpenXR runtime |
|------|---------|----------------|
| **SteamVR (tethered)** | Index, Vive, Vive Pro | **SteamVR OpenXR** |

This is a single native PCVR path — no wireless streaming app required.

---

## Windows — SteamVR

### Install and pair

1. Install Steam; install **SteamVR** from the Steam library.
2. Connect the headset; power on controllers and base stations.
3. Run **SteamVR Room Setup** (standing or seated per your play space).
4. Confirm tracking in the SteamVR status window.

### Set OpenXR runtime

1. SteamVR → menu (≡) → **Settings → OpenXR** (or **Developer** on older builds).
2. **Set SteamVR as OpenXR Runtime**.
3. Leave SteamVR running or launch it before UE VR Preview.

Details: [OPENXR_DEV_COMMON.md](../OPENXR_DEV_COMMON.md#set-steamvr-as-openxr-runtime).

### Mistspire dev test

Follow [OPENXR_DEV_COMMON.md](../OPENXR_DEV_COMMON.md) → VR Preview on `Main_WP`.

---

## Linux — SteamVR (primary)

SteamVR and OpenXR run on **Linux** with current Steam client and GPU drivers.

1. Install Steam (native or Flatpak per distro notes).
2. Install and run **SteamVR**; complete room setup.
3. **Set SteamVR as OpenXR runtime** (same menu as Windows).
4. Launch UE → VR Preview.

## Linux — Monado (alternative)

Open-source stack without SteamVR:

1. Install Monado + optional libsurvive (6DoF Lighthouse tracking).
2. `export XR_RUNTIME_JSON=/usr/share/openxr/openxr_monado.json` (path may vary).
3. Optional: `STEAMVR_LH_ENABLE=true` for Lighthouse via Monado's SteamVR driver.
4. Run `./scripts/verify-openxr-runtime.sh`.

Guide: [Monado Valve Index setup](https://monado.freedesktop.org/valve-index-setup.html). Wiki: [Linux VR — Monado](https://wiki.vronlinux.org/docs/fossvr/monado/).

For Steam/Proton games that use OpenVR only, add **xrizer** or OpenComposite and `PRESSURE_VESSEL_IMPORT_OPENXR_1_RUNTIMES=1` launch options.

---

## Troubleshooting

| Issue | Fix |
|-------|-----|
| Headset not detected | Check DP/USB GPU ports; update GPU drivers |
| Gray SteamVR room | Re-run room setup; check base station line-of-sight |
| UE black screen | Confirm SteamVR OpenXR is active |
| Monado: no 6DoF | Install libsurvive; rebuild monado with survive support |

## Official links

- [SteamVR](https://store.steampowered.com/app/250820/SteamVR/)
- [Epic OpenXR prerequisites](https://dev.epicgames.com/documentation/en-us/unreal-engine/openxr-prerequisites-in-unreal-engine)
- [Monado](https://monado.freedesktop.org/)
