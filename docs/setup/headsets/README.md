# Headset setup guides

Pick your headset, then follow the guide for your **connection path**. Multiple paths are documented for standalone headsets (Quest, Pico, Vive Focus).

**Default OpenXR runtime:** SteamVR OpenXR — unless a path requires Meta Link or VDXR (see table).

**Shared streaming setup:** [streaming_options.md](streaming_options.md) (Virtual Desktop, Steam Link, WiVRn).

## Support matrix

| Category | Connection path | Active OpenXR runtime on PC | Binding JSON | Linux |
|----------|----------------|----------------------------|--------------|-------|
| **Meta Quest** | Meta Horizon Link | Meta Horizon Link | `bindings_meta_quest.json` | WiVRn (Link is Windows-only) |
| **Meta Quest** | Virtual Desktop | VDXR or SteamVR OpenXR | same | WiVRn |
| **Meta Quest** | Steam Link | **SteamVR OpenXR** (required) | same | SteamVR + Steam Link; WiVRn |
| **Oculus Rift / Rift S** | USB tethered | Meta Horizon (legacy) | touch or KHR simple | Windows-only |
| **Valve Index / HTC Vive** | SteamVR (USB/DP) | **SteamVR OpenXR** | `bindings_valve_index.json` / `bindings_htc_vive.json` | SteamVR OpenXR; or Monado |
| **Windows MR** (incl. HP Reverb) | Oasis + SteamVR | **SteamVR OpenXR** | `bindings_khr_simple.json` | Oasis unsupported |
| **Pico** | PICO Connect | **SteamVR OpenXR** | KHR simple | WiVRn; SteamVR + Steam Link |
| **Pico** | Virtual Desktop | VDXR or SteamVR OpenXR | same | WiVRn |
| **Pico** | Steam Link | **SteamVR OpenXR** (required) | same | SteamVR + Steam Link |
| **HTC Vive Focus** | Virtual Desktop | VDXR or SteamVR OpenXR | KHR simple | WiVRn |
| **HTC Vive Focus** | Steam Link | **SteamVR OpenXR** (required) | same | SteamVR + Steam Link |
| **HTC Vive Focus** | Vendor streaming | SteamVR where supported | same | WiVRn |
| **Steam Frame** | 6 GHz dongle → PC | **SteamVR OpenXR** | Index-like (pre-release) | SteamVR when supported |
| **Generic** | Vendor or SteamVR | SteamVR OpenXR preferred | `bindings_khr_simple.json` | SteamVR or Monado |

HP Reverb is covered under [windows_mixed_reality.md](windows_mixed_reality.md) (Oasis driver), not a separate category.

## Guides

| File | Devices |
|------|---------|
| [meta_quest.md](meta_quest.md) | Quest 2/3/3S/Pro; Rift legacy |
| [steamvr_lighthouse.md](steamvr_lighthouse.md) | Index, Vive Pro, Vive |
| [htc_vive_focus.md](htc_vive_focus.md) | Focus 3, XR Elite |
| [windows_mixed_reality.md](windows_mixed_reality.md) | Reverb, Odyssey, Explorer, etc. |
| [pico.md](pico.md) | Pico 4, Neo, Ultra |
| [steam_frame.md](steam_frame.md) | Steam Frame (upcoming) |
| [generic.md](generic.md) | Fallback |
| [streaming_options.md](streaming_options.md) | VD, Steam Link, WiVRn |

## Script hints (future bootstrap)

| Detect signal | Suggested runtime | Verify |
|---------------|-------------------|--------|
| Default / unknown | steamvr | SteamVR → OpenXR tab |
| Steam Link session | steamvr (required) | SteamVR OpenXR tab |
| Meta Link + Link path | meta | Link Settings → General |
| VDXR enabled | vdxr | VD Streamer overlay |
| `XR_RUNTIME_JSON` set | custom | `verify-openxr-runtime.sh` |
| Linux tethered Index | steamvr or monado | SteamVR / `openxr-info` |
| Linux standalone stream | wivrn or steamvr+steamlink | WiVRn dashboard |
