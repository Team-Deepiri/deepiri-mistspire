# Setup documentation

Onboarding for Mistspire contributors — project bootstrap, headset PCVR setup, and day-1 validation.

## Quick path

1. **Platform** — [PLATFORMS.md](PLATFORMS.md) (Linux vs Windows vs WSL); [DEV_BOOTSTRAP.md](DEV_BOOTSTRAP.md) (one-run flow).
2. **Scripts** — Windows: `setup.ps1` / `run.ps1` — Linux: `setup.sh` / `run.sh` (interactive headset walkthrough before deps; `-NonInteractive` / `--non-interactive` to skip).
3. **Headset** — [headsets/README.md](headsets/README.md) (also offered by setup when you have an HMD).
4. **VR session** — [OPENXR_DEV_COMMON.md](OPENXR_DEV_COMMON.md).
5. **Smoke test** — [DAY1_MAINTAINER_CHECKLIST.md](DAY1_MAINTAINER_CHECKLIST.md).

## Headset guides

| Guide | Headsets |
|-------|----------|
| [meta_quest.md](headsets/meta_quest.md) | Meta Quest; legacy Oculus Rift |
| [steamvr_lighthouse.md](headsets/steamvr_lighthouse.md) | Valve Index, HTC Vive (tethered) |
| [htc_vive_focus.md](headsets/htc_vive_focus.md) | HTC Vive Focus 3, XR Elite (standalone) |
| [windows_mixed_reality.md](headsets/windows_mixed_reality.md) | WMR headsets incl. HP Reverb (Oasis + SteamVR) |
| [pico.md](headsets/pico.md) | Pico 4 / Neo / Ultra |
| [steam_frame.md](headsets/steam_frame.md) | Steam Frame (upcoming) |
| [generic.md](headsets/generic.md) | Other headsets (KHR simple fallback) |

**Streaming (shared):** [headsets/streaming_options.md](headsets/streaming_options.md) — Virtual Desktop, Steam Link, WiVRn.

**Primary OpenXR runtime:** SteamVR OpenXR (unless your connection path requires a vendor or VDXR runtime — see your headset page).

## Related

- [Architecture: OpenXR runtimes](../architecture/OPENXR_RUNTIMES.md)
- [Interaction profiles](../../interaction_profiles/openxr/README.md)
- [Maps / Main_WP](../../game/Content/Maps/README.md)
