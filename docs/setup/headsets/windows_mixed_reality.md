# Windows Mixed Reality (incl. HP Reverb)

PCVR setup for **Windows Mixed Reality** headsets on **Windows 11**, including **HP Reverb G1/G2** (including Omnicept), Samsung Odyssey, Lenovo Explorer, Dell Visor, and similar.

The original **Mixed Reality Portal** app is discontinued on recent Windows 11 builds. Use the **Oasis** SteamVR driver instead.

Binding: [`bindings_khr_simple.json`](../../../interaction_profiles/openxr/bindings_khr_simple.json).

HP Reverb is **not** a separate category — it is a WMR headset covered here.

## What you need

- WMR headset + motion controllers.
- **Windows 11** (Oasis targets Win11 24H2+; Win10 users may use legacy Portal — out of scope for this guide).
- **NVIDIA GPU** (Oasis requirement); AMD **RDNA 5000+** on Win11 24H2+ per Oasis docs.
- [Steam](https://store.steampowered.com/) + **SteamVR**.
- [Oasis Driver for Windows Mixed Reality](https://store.steampowered.com/app/3824490/) (free on Steam).
- PC Bluetooth for controller pairing (headset built-in BT on some models).

## Choose your connection path

| Path | OpenXR runtime |
|------|----------------|
| **Oasis + SteamVR** | **SteamVR OpenXR** |

Single path: Oasis exposes the headset to SteamVR; SteamVR is the OpenXR runtime.

---

## Windows — Oasis + SteamVR

### Install

1. Install **Steam** and **SteamVR**.
2. Install **Oasis Driver for Windows Mixed Reality** from Steam (do not rely on Mixed Reality Portal on Win11 24H2+).
3. Pair motion controllers in **Windows Bluetooth settings** (if applicable).

You do **not** need: Mixed Reality Portal, “Windows Mixed Reality for SteamVR” shim, or OpenXR Tools from the Microsoft Store for Oasis.

### Unlock headset and controllers (once per PC/headset)

1. Exit SteamVR completely.
2. Steam Library → **Oasis Driver** → Launch → **Unlock your headset & controllers for Oasis**.
3. Follow prompts: disconnect/reconnect USB when asked; power-cycle controllers if prompted.
4. Run **SteamVR Room Setup**.

Repeat unlock after pairing new controllers or major Windows reinstall.

Wiki: [Oasis unlock procedure](https://github.com/mbucchia/Oasis-Driver-for-Windows-Mixed-Reality/wiki/Procedure-to-unlock-headset-and-controllers-for-Oasis).

### Set OpenXR runtime

1. Launch SteamVR.
2. **Set SteamVR as OpenXR Runtime** — [OPENXR_DEV_COMMON.md](../OPENXR_DEV_COMMON.md#set-steamvr-as-openxr-runtime).

### Mistspire dev test

Follow [OPENXR_DEV_COMMON.md](../OPENXR_DEV_COMMON.md) → VR Preview on `Main_WP`.

---

## Linux

**Oasis is Windows 11 only** — not available on Linux.

- HP Reverb / WMR on Linux: experimental **Monado** WMR driver only; not documented as a supported Mistspire dev path.
- Use a Windows 11 PC for Reverb/WMR development, or a different headset on Linux (Index + SteamVR/Monado, Quest + WiVRn).

---

## Troubleshooting

| Issue | Fix |
|-------|-----|
| `Unexpected error while opening MR USB device: 6` | Mixed Reality Portal interfering (Win11 23H2); disable MR device in Device Manager per Oasis wiki |
| Controllers not in SteamVR | Re-run unlock; pair BT controllers before unlock |
| Crash after room setup | Delete `C:\ProgramData\WindowsHolographicDevices` per Oasis wiki |
| Intel iGPU only | Oasis requires discrete NVIDIA/AMD per store page |

## Official links

- [Oasis on Steam](https://store.steampowered.com/app/3824490/)
- [Oasis GitHub wiki](https://github.com/mbucchia/Oasis-Driver-for-Windows-Mixed-Reality/wiki)
- [SteamVR](https://store.steampowered.com/app/250820/SteamVR/)
