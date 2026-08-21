# Meta Quest (and legacy Rift)

PCVR setup for **Meta Quest 2 / 3 / 3S / Pro**. Legacy **Oculus Rift / Rift S** notes at the bottom.

Binding: [`bindings_meta_quest.json`](../../../interaction_profiles/openxr/bindings_meta_quest.json) — OpenXR profile `/interaction_profiles/oculus/touch_controller`.

## What you need

- Quest with charged controllers; USB-C cable (wired Link) or strong 5/6 GHz Wi‑Fi (Air Link).
- Windows PC for Meta Horizon Link (Link is **Windows-only**).
- GPU meeting [Meta Link requirements](https://www.meta.com/help/quest/articles/headsets-and-accessories/oculus-link/link-graphics-card/).
- Admin rights to set the OpenXR runtime on Windows.

## Choose your connection path

| Path | Best for | OpenXR runtime on PC |
|------|----------|---------------------|
| **A — Meta Horizon Link** | Official UE/Link workflow, wired or Air Link | **Meta Horizon Link** |
| **B — Virtual Desktop** | Wireless tuning, alternative to Air Link | VDXR or SteamVR OpenXR |
| **C — Steam Link** | Steam library, default SteamVR path | **SteamVR OpenXR** (required) |

Shared VD / Steam Link steps: [streaming_options.md](streaming_options.md).

---

## Path A — Meta Horizon Link (Windows)

### Install and pair

1. Install [Meta Horizon Link](https://www.meta.com/experiences/) on the PC.
2. On the headset: **Settings → Quest Link** → enable **Quest Link**.
3. Connect USB or use Air Link; select **Launch Quest Link** on the headset.
4. Confirm the headset tracks in the Link desktop window.

### Set OpenXR runtime

1. Meta Horizon Link → **Settings → General**.
2. **Set Meta Horizon Link as active** OpenXR runtime (admin required).
3. **Settings → Developer** → enable **Developer Runtime Features** (hand tracking / passthrough dev features over Link).

### Mistspire dev test

Follow [OPENXR_DEV_COMMON.md](../OPENXR_DEV_COMMON.md) → VR Preview on `Main_WP`.

---

## Path B — Virtual Desktop

See [streaming_options.md](streaming_options.md#virtual-desktop).

- Enable **VDXR** in the streamer for OpenXR without SteamVR, **or** use SteamVR OpenXR with VDXR off.
- Switch runtime before UE if you previously used Link or Steam Link.

---

## Path C — Steam Link

See [streaming_options.md](streaming_options.md#steam-link).

- **Must** set SteamVR as OpenXR runtime (Meta runtime breaks OpenXR over Steam Link).

---

## Linux

- **Meta Horizon Link:** not available on Linux.
- **WiVRn:** [wivrn.github.io](https://wivrn.github.io/) — Quest client from Meta Store; Linux PC runs the server. See [streaming_options.md](streaming_options.md#wivrn-linux-server).
- **SteamVR + Steam Link:** supported on Linux with current Steam/GPU drivers; set SteamVR OpenXR, then use Steam Link on the Quest.

---

## Legacy: Oculus Rift / Rift S

- USB tethered PC headset (not Quest Link).
- Install **Meta Horizon** PC software; set **Meta Horizon** as OpenXR runtime.
- Windows only; treat as legacy hardware. New contributors should prefer Quest.
- Binding: touch profile if recognized, else [`bindings_khr_simple.json`](../../../interaction_profiles/openxr/bindings_khr_simple.json).

---

## Troubleshooting

| Issue | Fix |
|-------|-----|
| Black screen in VR Preview | Wrong runtime for path (Link vs Steam Link vs VDXR) |
| Link works, Steam Link does not | Set SteamVR OpenXR; exit Link |
| Hand tracking odd over Steam Link | Expected limitation; use Link + Developer Runtime Features for real hand tracking |
| `XR_ERROR_FORM_FACTOR_UNAVAILABLE` | Steam Link with Meta runtime still active |

## Official links

- [Meta Link for Unreal](https://developers.meta.com/horizon/documentation/unreal/unreal-link/)
- [Meta headset dev setup](https://developers.meta.com/horizon/documentation/unity/unity-env-device-setup/)
- [Virtual Desktop](https://www.vrdesktop.net/)
- [WiVRn](https://wivrn.github.io/)
