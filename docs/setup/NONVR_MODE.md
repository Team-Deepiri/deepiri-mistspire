# Mistspire — non-VR mode (keyboard / mouse)

Play Mistspire without a VR headset. Uses the same `AMistspireVRPawn` and game systems as PCVR, with keyboard/mouse input routing.

## Requirements

- Unreal Engine **5.8** (or 5.5+) from Epic Launcher
- Compile C++ once when the editor first opens the project

## Quick start — editor (Windows)

```powershell
.\run-nonvr.ps1
```

Open the project, then click **Play** (not **VR Preview**).

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
| W / A / S / D | Move |
| Mouse | Look |
| Space | Jump |
| Left Control (hold) | Climb (when facing a surface) |
| Left Shift (hold) | Sprint |
| F or Right Mouse | Grapple |
| G | Toggle glider |
| T | Teleport blink forward |
| E | Interact (buttons, lore shards) |
| `~` | Console |

## Console

```
mistspire.ShowAltitudeHUD 1
mistspire.ShowControls 1
mistspire.TeleportUp 5000
mistspire.AltitudeStats
mistspire.RefillSurvival
```

## Mode detection

| Situation | Mode |
|-----------|------|
| Editor **Play** | Non-VR (keyboard/mouse) |
| Editor **VR Preview** | VR (OpenXR controllers) |
| `-nonvr` or `-demoflat` CLI flag | Force non-VR |
| `-forcvr` or `-forcevr` CLI flag | Force VR input |
| Packaged, no HMD connected | Non-VR |

## World bootstrap

On non-VR start, after a short delay (to allow World Partition to stream), if there is **no ground** under `PlayerStart`, the game spawns a fallback floor (400 m) and stepping platforms. If `Main_WP` already has landscape, it uses the map as-is.

Atlas door/POI markers are **not** spawned in non-VR by default (set `bSpawnAtlasMarkersInNonVR` on `MistspireGameMode` to enable).

## Packaging

Cook and package (run from repo root, adjust `UE_ROOT`):

```powershell
$UAT = "$env:UE_ROOT\Engine\Build\BatchFiles\RunUAT.bat"
& $UAT BuildCookRun `
  -project="game\Mistspire.uproject" `
  -platform=Win64 `
  -clientconfig=Development `
  -cook -stage -pak -archive `
  -archivedirectory="game\Package\Win64"
```

Launch:

```powershell
.\game\Package\Win64\Windows\Mistspire\Binaries\Win64\Mistspire.exe -nonvr
```

OpenXR plugins remain enabled; they are not required when `-nonvr` is used and no headset is connected.

## VR unchanged

PCVR development is unchanged. Use **VR Preview** or `-forcvr` for headset testing. See [PCVR_DEV_SETUP.md](PCVR_DEV_SETUP.md).
