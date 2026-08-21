# Pico VR

PCVR setup for **Pico 4**, **Pico Neo 3**, **Pico Ultra**, and related headsets.

Binding: [`bindings_khr_simple.json`](../../../interaction_profiles/openxr/bindings_khr_simple.json) until a dedicated Pico profile is added.

## What you need

- Pico headset (PICO OS **5.11.2+** for current PICO Connect).
- USB-C / DisplayPort cable (wired) or 5 GHz Wi‑Fi (wireless).
- Windows PC for PICO Connect and Virtual Desktop streamer.
- Steam + SteamVR for PICO Connect, Steam Link, and many VD setups.

## Choose your connection path

| Path | OpenXR runtime on PC |
|------|---------------------|
| **A — PICO Connect** (official) | **SteamVR OpenXR** |
| **B — Virtual Desktop** | VDXR or SteamVR OpenXR |
| **C — Steam Link** | **SteamVR OpenXR** (required) |

Shared VD / Steam Link: [streaming_options.md](streaming_options.md).

---

## Path A — PICO Connect

### Install

1. Download [PICO Connect](https://www.picoxr.com/global/software/pico-link) for Windows (≥ PICO OS 5.11.2 on headset).
2. Install **Steam** and **SteamVR** on the PC.
3. **Set SteamVR as OpenXR runtime** before streaming.
4. On PC: start PICO Connect → wired (**DP Connection**) or wireless (**WiFi connection**).
5. On headset: open the PICO Link / Connect app; confirm connection.

### Mistspire dev test

Follow [OPENXR_DEV_COMMON.md](../OPENXR_DEV_COMMON.md) → VR Preview on `Main_WP`.

---

## Path B — Virtual Desktop

Pico 4 / Ultra are supported by Virtual Desktop.

See [streaming_options.md](streaming_options.md#virtual-desktop) — VDXR or SteamVR OpenXR.

---

## Path C — Steam Link

Pico is listed for the Steam Link VR app.

See [streaming_options.md](streaming_options.md#steam-link) — **SteamVR OpenXR required**.

---

## Linux

- **PICO Connect streamer:** Windows-focused; on Linux use **WiVRn** or **SteamVR + Steam Link**.
- **WiVRn:** install Pico client via dashboard wizard (APK sideload if not in store).
- **SteamVR + Steam Link:** first-class Linux path with current drivers; set SteamVR OpenXR, Steam Link on headset.

---

## Developer mode (optional)

For sideloading or Pico-specific Android builds, enable developer mode via the [PICO Developer](https://developer.picoxr.com/) portal. Not required for PCVR streaming dev with Connect/VD/Steam Link.

---

## Troubleshooting

| Issue | Fix |
|-------|-----|
| PICO Connect won't start SteamVR | Install SteamVR; set as OpenXR runtime |
| Old Streaming Assistant app | Update headset OS; migrate to PICO Connect |
| Black screen in UE | Wrong runtime (Meta/VDXR vs SteamVR for your path) |

## Official links

- [PICO Connect download](https://www.picoxr.com/global/software/pico-link)
- [PICO Developer](https://developer.picoxr.com/)
- [Virtual Desktop](https://www.vrdesktop.net/)
- [WiVRn](https://wivrn.github.io/)
