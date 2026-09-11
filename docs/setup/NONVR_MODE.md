# Mistspire — non-VR mode (keyboard / mouse)

Play Mistspire without a VR headset. Uses the same `AMistspireVRPawn` and game systems as PCVR, with **Enhanced Input** for keyboard/mouse. VR Preview still uses OpenXR (`mistspire_gameplay`), not these mappings.

## Requirements

- Unreal Engine **5.8** (or 5.5+) from Epic Launcher
- Compile C++ once when the editor first opens the project

## Quick start — editor (Windows)

```powershell
.\run-nonvr.ps1
```

Open the project, then click **Play** (not **VR Preview**). You will see the **Mistspire** title over the live world; press any key (or mouse button) to start.

## Quick start — editor (Linux)

```bash
./run-nonvr.sh
```

## Quick start — packaged Win64

After cooking/packaging locally (requires licensed UE install):

```powershell
.\scripts\launch_packaged_win64.ps1
```

See [Packaging](#packaging) below for cook commands.

## Controls

| Input | Action |
|-------|--------|
| W / A / S / D | Move (eases in/out; not instant) |
| Mouse | Look (camera bobs while walking) |
| Space | Jump |
| Left Control (hold) | Climb (when facing a surface) |
| Left Shift (hold) | Sprint |
| F or Right Mouse | Grapple (aim with center +; press again to cancel) |
| G | Toggle glider |
| T | Teleport blink forward |
| E | Interact (buttons, lore shards) |
| Esc | Settings (fullscreen, mouse sensitivity, view bobbing, FOV, keyboard controls hint) |
| `~` | Console |
| Alt+Enter / F11 | Toggle fullscreen (outside settings) |

Bindings are created in C++ (`FMistspireInputMode::CreateNonVREnhancedInput`) — there are no Content Input Action assets. `DefaultInput.ini` uses `EnhancedPlayerInput` / `EnhancedInputComponent`.

Esc opens a Slate settings overlay (soft-pause: cursor on, gameplay input ignored). Values persist via `UMistspireGameUserSettings`. The on-screen keyboard controls line is **off by default**; enable it under **Show keyboard controls** in settings (or `mistspire.ShowControls 1` to force on).

A white **+** crosshair is drawn at screen center (`AMistspireHUD`) for grapple aim. The cable tip extends to the hit before pull starts; press F / RMB again to cancel while extending or reeling. The far end stays locked to the world hit (it does not follow look).

## Console

```
mistspire.ShowAltitudeHUD 1
mistspire.ShowControls 1
mistspire.TeleportUp 5000
mistspire.AltitudeStats
mistspire.RefillSurvival
mistspire.ToggleSettings
```

In the editor, **Esc stops PIE**. Use `mistspire.ToggleSettings` (tilde console) to open/close the settings menu instead. Packaged builds still use Esc.
## Troubleshooting input

- **Editor upgrade toasts:** UE 5.8 migrates leftover Axis/Action mappings to Enhanced Input. This project already uses Enhanced Input; restart after pulling so `DefaultInput.ini` has no legacy mappings.
- **Keys do nothing:** click inside the PIE viewport; recompile C++ after pulling; launch with `.\run-nonvr.ps1` (`-nonvr`) and use **Play**, not **VR Preview**.
- **Still no movement:** confirm Output Log has no `Mistspire non-VR requires EnhancedInputComponent` error.
- **Magenta/cyan noise only on the sky / behind ground (two bottom tiles + right-edge strip):** leftover **instanced stereo** L/R eye rects. Project defaults are now mono-safe (`r.ClearSceneMethod=1`, `vr.InstancedStereo=False`). Runtime uses `ECVF_SetByCode` so overrides actually win over ini. Fully restart the editor after pulling so RendererSettings reload — toggling mid-session is not enough.

## Mode detection

| Situation | Mode |
|-----------|------|
| Editor **Play** | Non-VR (keyboard/mouse) |
| Editor **VR Preview** | VR (OpenXR controllers) |
| `-nonvr` or `-demoflat` CLI flag | Force non-VR |
| `-forcvr` or `-forcevr` CLI flag | Force VR input |
| Packaged, no HMD connected | Non-VR |

## World bootstrap

On non-VR start, the game polls for ground under `PlayerStart` for up to ~5 s (World Partition streaming). If there is still **no ground**, it spawns a fallback **cube** floor (thick, `BlockAll`, ~400 m) and stepping platforms. If `Main_WP` already has landscape, it uses the map as-is.

Atlas door/POI markers are **not** spawned in non-VR by default (set `bSpawnAtlasMarkersInNonVR` on `MistspireGameMode` to enable).

Non-VR walking uses step-up and wall-slide on uneven collision (rocks/debris), sticky ground support while standing still, and Ctrl-climb crest mantle so lip jitter is reduced. Grapple skips ground-snap while reeling.

## Packaging

Cook and package (run from repo root, adjust `UE_ROOT`):

```powershell
$UAT = "$env:UE_ROOT\Engine\Build\BatchFiles\RunUAT.bat"
& $UAT BuildCookRun `
  -project="game\Mistspire.uproject" `
  -platform=Win64 `
  -clientconfig=Development `
  -build -cook -stage -pak -archive `
  -archivedirectory="game\Package\Win64"
```

Launch:

```powershell
.\game\Package\Win64\Mistspire\Binaries\Win64\Mistspire.exe -nonvr
# or:
.\scripts\launch_packaged_win64.ps1
# optional: .\scripts\launch_packaged_win64.ps1 -ForceWindowed
```

Default display follows `UMistspireGameUserSettings` (windowed on first launch). Pass `-ForceWindowed` or `-windowed` to override. In-game **Esc** opens settings (fullscreen, sensitivity, bob, FOV, keyboard hint). Alt+Enter / F11 also toggle fullscreen.

OpenXR plugins remain enabled; they are not required when `-nonvr` is used and no headset is connected.

## VR unchanged

PCVR development is unchanged. Use **VR Preview** or `-forcvr` for headset testing. See [PCVR_DEV_SETUP.md](PCVR_DEV_SETUP.md).
