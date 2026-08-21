# Developer bootstrap (one-run flow)

Pick your OS. `setup.sh` / `setup.ps1` run an **interactive headset walkthrough** first (unless `--non-interactive` / `-NonInteractive`), then install or verify deps and open the editor.

## Windows

1. **Prerequisites** — Epic Launcher UE **5.8+**, Visual Studio 2022 (C++ + Windows SDK), Git LFS, GPU drivers.
2. **Repo setup**
   ```powershell
   git lfs install
   powershell -File setup.ps1
   ```
   Or: `.\setup.ps1` from PowerShell in the repo root.
3. **Headset prompts** (interactive) — whether you have a VR headset; if not connected yet, pick your device and open the matching guide under [headsets/](headsets/README.md). Optional Steam install via winget if missing.  
   CI / automation: `.\setup.ps1 -NonInteractive`
4. **Windows toolchain check** also runs inside setup (`scripts/setup-windows-deps.ps1`).
5. **OpenXR check** — run by setup; re-check anytime:
   ```powershell
   powershell -File scripts/verify-openxr-runtime.ps1
   ```
6. **Editor** — setup launches `game/Mistspire.uproject` when UE 5.8 is found. Build **Mistspire** + **MistspireOpenXRNative** when prompted.
7. **Plugins** — OpenXR, MistspireOpenXRNative enabled.
8. **LFS map** — confirm `Main_WP` loads ([DAY1 Phase 2](DAY1_MAINTAINER_CHECKLIST.md)).
9. **VR Preview** — Play → VR Preview on `Main_WP` (if you have a headset); run `mistspire.AltitudeStats`.

**Launch again:** `.\run.ps1` or `.\run.ps1 packaged` after packaging.

## Linux (native)

1. **Repo setup**
   ```bash
   ./setup.sh
   ```
   Headset walkthrough → deps (where supported) → LFS → OpenXR check → optional compile → editor.
2. **Non-interactive:** `./setup.sh --non-interactive`
3. **Headset guides** — if you skipped prompts or need details later: [headsets/README.md](headsets/README.md) (SteamVR, Monado, WiVRn).
4. **OpenXR check**
   ```bash
   ./scripts/verify-openxr-runtime.sh
   ```
5. **Editor / VR** — same plugin and map checks as Windows ([DAY1](DAY1_MAINTAINER_CHECKLIST.md)).
6. **VR Preview** — Play → VR Preview when headset is ready; console cheats in [OPENXR_DEV_COMMON.md](OPENXR_DEV_COMMON.md).

**Launch again:** `./run.sh` or `./run.sh packaged`.

## WSL2

1. Clone and `git lfs pull` in WSL.
2. Build `native/xr-sandbox` if you want an OpenXR smoke test without UE.
3. **Do not expect VR Preview in WSL** — use Windows (`setup.ps1`) or native Linux (`setup.sh`) for headset testing.

## Validation

Complete [DAY1_MAINTAINER_CHECKLIST.md](DAY1_MAINTAINER_CHECKLIST.md) when you need a maintainer-grade pass/fail.

## Platform reference

Full matrix: [PLATFORMS.md](PLATFORMS.md).
