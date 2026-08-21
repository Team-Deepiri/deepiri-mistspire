# OpenXR development (all headsets)

Shared steps for testing Mistspire in **VR Preview**. Assumes you have never used PCVR before.

## Primary runtime

**SteamVR OpenXR** is the default for Mistspire unless your [headset guide](headsets/README.md) specifies a path-specific runtime (Meta Horizon Link, VDXR, etc.).

When you switch connection paths (Link → Steam Link → Virtual Desktop), you must **change the active OpenXR runtime** each time. Only one runtime can be active on the PC.

## Glossary

| Term | Meaning |
|------|---------|
| **OpenXR runtime** | Software that talks to your headset (SteamVR, Meta Link, VDXR, Monado). UE uses whichever runtime is *active*. |
| **SteamVR** | Valve's VR platform; can be the OpenXR runtime for Index, Vive, Steam Link, PICO Connect, Oasis/WMR, and more. |
| **OpenVR** | Older Valve API; some Steam games still use it. Mistspire uses **OpenXR** only. |
| **VR Preview** | UE editor mode that renders to the HMD. Use **Play → VR Preview**, not plain Play. |
| **Connection path** | How the headset reaches the PC (USB, Link, Virtual Desktop, Steam Link, WiVRn). |

## Before every VR Preview session

1. **Quit** other VR apps (SteamVR home, Meta Link, Virtual Desktop stream, etc.) unless your path needs one running.
2. **Set the OpenXR runtime** for your chosen path (see your headset page).
3. **Wear the headset** and confirm tracking (SteamVR status, Link, or streamer overlay).
4. Open `game/Mistspire.uproject` → load `Main_WP` if needed.
5. **Play → VR Preview**.

## Mistspire project checks

Complete [DAY1_MAINTAINER_CHECKLIST.md](DAY1_MAINTAINER_CHECKLIST.md) Phases 1–3 first:

- UE **5.8+**, `git lfs pull`, modules **Mistspire** + **MistspireOpenXRNative** compiled
- Plugins: **OpenXR**, **MistspireOpenXRNative** (OpenXRHandTracking optional)
- `Main_WP` loads; Game Mode **MistspireGameMode**; PlayerStart above ground

## Set SteamVR as OpenXR runtime

Use this when your headset page says SteamVR is required or preferred:

1. Launch **SteamVR** on the PC.
2. Menu (≡) → **Settings** → **OpenXR** (or **Developer** on older builds).
3. Click **Set SteamVR as OpenXR Runtime**.
4. Restart SteamVR if prompted.

Works on **Windows and Linux** (Steam client + recent GPU drivers).

## Other runtimes (path-specific)

| Runtime | When |
|---------|------|
| **Meta Horizon Link** | Quest via official Link / Air Link (Windows) |
| **VDXR** | Virtual Desktop with “Use Virtual Desktop OpenXR runtime” enabled |
| **Monado** | Linux tethered PCVR without SteamVR; set `XR_RUNTIME_JSON` |

## Verify runtime (Linux)

```bash
./scripts/verify-openxr-runtime.sh
```

Optional: `openxr-info` (from monado-tools) after setting `XR_RUNTIME_JSON`.

## Console smoke test

In VR Preview, open the console (`~` or `` ` ``) and run:

```
mistspire.AltitudeStats
mistspire.TeleportUp 5000
mistspire.ShowAltitudeHUD 1
```

Full list: [IMMERSION.md](../gameplay/IMMERSION.md).

## Switching paths

Example: you used Meta Link yesterday and Steam Link today.

1. Exit Meta Link / SteamVR completely.
2. For Steam Link: set **SteamVR OpenXR** (not Meta).
3. Launch Steam Link on the headset, connect to PC.
4. Then start UE VR Preview.

## Advanced: runtime override

**Linux / manual:** `export XR_RUNTIME_JSON=/path/to/openxr_manifest.json`

**Windows (IT-locked accounts):** registry `HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\OpenXR\1\ActiveRuntime` — ask IT or see vendor docs.

## WSL

WSL2 is fine for git, scripts, and `xr-sandbox`. **Do not expect USB HMD passthrough in WSL.** Use native Windows or Linux for VR Preview.

## Known gap

JSON binding files under `interaction_profiles/openxr/` are **not automatically loaded** at runtime today. C++ creates the `mistspire_gameplay` action set; controller input may still need runtime wiring. See [DAY1_MAINTAINER_CHECKLIST.md](DAY1_MAINTAINER_CHECKLIST.md).
