# HTC Vive Focus (standalone)

PCVR setup for **standalone** HTC headsets: **Vive Focus 3**, **Vive XR Elite**, and similar — streamed from a PC.

**Not** the tethered Lighthouse Vive kit — see [steamvr_lighthouse.md](steamvr_lighthouse.md).

Binding: [`bindings_khr_simple.json`](../../../interaction_profiles/openxr/bindings_khr_simple.json) until a dedicated profile exists.

## What you need

- Standalone HTC headset with controllers.
- Windows PC for Virtual Desktop or Steam Link streamers.
- 5/6 GHz Wi‑Fi (wireless) or USB where the app supports it.
- Steam + SteamVR for Steam Link and many VD configurations.

## Choose your connection path

| Path | OpenXR runtime on PC |
|------|---------------------|
| **A — Virtual Desktop** | VDXR or SteamVR OpenXR |
| **B — Steam Link** | **SteamVR OpenXR** (required) |
| **C — Vendor / business streaming** | SteamVR where supported |

Shared steps: [streaming_options.md](streaming_options.md).

---

## Path A — Virtual Desktop

Virtual Desktop supports **Vive Focus / XR Elite** alongside Quest and Pico.

1. Install VD on the headset and the PC streamer ([vrdesktop.net](https://www.vrdesktop.net/)).
2. Choose **VDXR** or **SteamVR** in streamer Options — see [streaming_options.md](streaming_options.md#virtual-desktop).
3. Connect from the headset; verify tracking.
4. Follow [OPENXR_DEV_COMMON.md](../OPENXR_DEV_COMMON.md) → VR Preview.

---

## Path B — Steam Link

1. Install **Steam Link** VR app on the headset.
2. On PC: Steam + SteamVR; **set SteamVR as OpenXR runtime**.
3. Pair and connect; confirm SteamVR home loads in the headset.
4. UE → VR Preview.

See [streaming_options.md](streaming_options.md#steam-link).

---

## Path C — Vendor streaming

If your organization uses HTC business streaming tools, follow HTC's current docs for that product. For Mistspire dev consistency with other standalones, **Virtual Desktop** or **Steam Link** are the recommended paths.

---

## Linux

- **Virtual Desktop streamer:** Windows/macOS host only — use WiVRn or Steam Link on Linux instead.
- **SteamVR + Steam Link:** supported on Linux; primary native path alongside WiVRn.
- **WiVRn:** if a client is available for your device (check [WiVRn compatibility](https://wivrn.github.io/)); otherwise sideload APK per dashboard wizard.

---

## Troubleshooting

| Issue | Fix |
|-------|-----|
| Confused with tethered Vive | This guide is for Focus/XR Elite; use [steamvr_lighthouse.md](steamvr_lighthouse.md) for Index/Vive PC kit |
| VD vs Steam Link runtime conflict | Set runtime per path before UE |
| Black screen | Wrong OpenXR runtime active |

## Official links

- [HTC VIVE](https://www.vive.com/)
- [Virtual Desktop](https://www.vrdesktop.net/)
- [Steam Link](https://store.steampowered.com/app/353380/Steam_Link/)
- [WiVRn](https://wivrn.github.io/)
