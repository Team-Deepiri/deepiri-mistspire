# Streaming options (standalone headsets)

Cross-cutting setup for **Virtual Desktop**, **Steam Link**, and **WiVRn**. Linked from Quest, Pico, and Vive Focus guides.

Mistspire's default OpenXR runtime is **SteamVR OpenXR**. Some paths use a different runtime — switch before launching UE **VR Preview**.

## Choose a path

| Preference | Path | Runtime on PC |
|------------|------|---------------|
| Official vendor tools, lowest latency (where available) | Meta Link, PICO Connect | Vendor or SteamVR |
| Wireless, router tuning, works on most Android standalones | Virtual Desktop | VDXR or SteamVR |
| Steam library, simplest default OpenXR | Steam Link | **SteamVR** (required) |
| Linux PC as streaming server | WiVRn | Monado-based OpenXR |

---

## Virtual Desktop

Supported on **Meta Quest**, **Pico**, **HTC Vive Focus / XR Elite**, and most Android-based standalones.

### What you need

- Virtual Desktop app on the headset (store purchase).
- [Virtual Desktop Streamer](https://www.vrdesktop.net/) on the PC (Windows or macOS for the streamer host).
- 5 GHz or 6 GHz Wi‑Fi; PC on Ethernet to the router is strongly recommended.

### Setup

1. Install the streamer on the PC and the headset app on the device.
2. Sign in / pair per the VD wizard.
3. On the PC streamer, open **Options**:
   - **Use Virtual Desktop OpenXR runtime (VDXR)** — bypasses SteamVR for OpenXR apps; lower overhead for some titles.
   - **Automatic / SteamVR** — uses SteamVR as OpenXR when VDXR is off.
4. Connect from the headset; confirm the stream is stable before opening UE.

### OpenXR runtime

- **VDXR path:** enable VDXR in streamer Options; verify in the VD performance overlay (Runtime = `VDXR`).
- **SteamVR path:** disable VDXR; set [SteamVR as OpenXR runtime](../OPENXR_DEV_COMMON.md#set-steamvr-as-openxr-runtime).

### Mistspire dev test

After streaming works, follow [OPENXR_DEV_COMMON.md](../OPENXR_DEV_COMMON.md) → VR Preview on `Main_WP`.

### Linux note

The VD **streamer runs on Windows or macOS**, not Linux. On a Linux dev machine, use **WiVRn** or **Steam Link** instead.

---

## Steam Link

Supported on **Meta Quest**, **Pico**, and **HTC** headsets via the Steam Link VR app.

### What you need

- [Steam](https://store.steampowered.com/) on the PC.
- **SteamVR** installed and working.
- Steam Link app on the headset (Meta / Pico / HTC store listings).

### Setup

1. Install Steam + SteamVR on the PC; complete room setup if prompted.
2. **Set SteamVR as OpenXR runtime** (required) — see [OPENXR_DEV_COMMON.md](../OPENXR_DEV_COMMON.md#set-steamvr-as-openxr-runtime).
3. Install Steam Link on the headset; connect to the PC on the same network.
4. Launch a SteamVR title once to confirm the link works before UE.

### OpenXR runtime

**SteamVR OpenXR only.** Meta Link or VDXR must be deactivated. If OpenXR apps show a black screen or `XR_ERROR_FORM_FACTOR_UNAVAILABLE`, the wrong runtime is active.

### Linux

Steam Link and SteamVR run on **Linux** with current Steam and GPU drivers. Same steps: SteamVR as OpenXR → Steam Link on headset → UE VR Preview.

---

## WiVRn (Linux server)

[WiVRn](https://wivrn.github.io/) streams from a **Linux PC** to standalone headsets (Quest, Pico, and others).

### What you need

- WiVRn server/dashboard on Linux (Flatpak, AUR, or distro package).
- WiVRn client on the headset (Meta Store for Quest 2/3/Pro, or sideload APK).
- Good 5 GHz Wi‑Fi or USB + ADB for wired mode.
- For Steam/OpenVR games: **xrizer** or OpenComposite (Flatpak WiVRn bundles these).

### Setup

1. Install WiVRn on the PC; run the connection wizard.
2. Install the client on the headset; connect from the headset app.
3. When the dashboard shows “Connection ready”, start your OpenXR app on the PC (UE VR Preview).
4. For Steam games, copy launch options from the WiVRn dashboard into Steam.

### OpenXR runtime

WiVRn acts as the OpenXR runtime on the Linux host (Monado-based). Use `./scripts/verify-openxr-runtime.sh` and optional `openxr-info`.

### Firewall (if needed)

Open UDP/TCP **9757** and mDNS (**5353** UDP); enable `avahi-daemon` on Linux.

---

## Troubleshooting

| Symptom | Likely cause |
|---------|----------------|
| Black screen in UE | Wrong OpenXR runtime for your path |
| `XR_ERROR_FORM_FACTOR_UNAVAILABLE` | Steam Link with Meta runtime active |
| VD works, Link does not | Runtime still set to VDXR or Meta |
| Linux Steam game won't use HMD | Missing `PRESSURE_VESSEL_IMPORT_OPENXR_1_RUNTIMES=1` or xrizer |

See [OPENXR_DEV_COMMON.md](../OPENXR_DEV_COMMON.md) and your headset-specific guide.
