# Developer bootstrap (one-run flow)

Step-by-step onboarding before the interactive `dev-bootstrap` script automates this. Pick your OS.

## Windows

1. **Prerequisites** — Epic Launcher UE **5.8+**, Visual Studio 2022 (C++ + Windows SDK), Git LFS, GPU drivers.
2. **Repo setup**
   ```powershell
   git lfs install
   powershell -File setup.ps1
   ```
   Or: `.\setup.ps1` from PowerShell in the repo root.
3. **Windows toolchain check** (optional)
   ```powershell
   powershell -File scripts/setup-windows-deps.ps1
   ```
4. **Headset** — [headsets/README.md](headsets/README.md) → your device → set OpenXR runtime ([OPENXR_DEV_COMMON.md](OPENXR_DEV_COMMON.md)).
5. **OpenXR check**
   ```powershell
   powershell -File scripts/verify-openxr-runtime.ps1
   ```
6. **Editor** — open `game/Mistspire.uproject` (setup.ps1 can launch it). Build **Mistspire** + **MistspireOpenXRNative** when prompted.
7. **Plugins** — OpenXR, MistspireOpenXRNative enabled.
8. **LFS map** — confirm `Main_WP` loads ([DAY1 Phase 2](DAY1_MAINTAINER_CHECKLIST.md)).
9. **VR Preview** — Play → VR Preview on `Main_WP`; run `mistspire.AltitudeStats`.

**Launch again:** `.\run.ps1` or `.\run.ps1 packaged` after packaging.

## Linux (native)

1. **Repo setup**
   ```bash
   ./setup.sh
   ```
   Installs deps (where supported), LFS pull, OpenXR check, optional compile, launches editor.
2. **Headset** — [headsets/README.md](headsets/README.md) → Linux section for your path (SteamVR, Monado, WiVRn).
3. **OpenXR check**
   ```bash
   ./scripts/verify-openxr-runtime.sh
   ```
4. **Editor / VR** — same plugin and map checks as Windows ([DAY1](DAY1_MAINTAINER_CHECKLIST.md)).
5. **VR Preview** — Play → VR Preview; console cheats in [OPENXR_DEV_COMMON.md](OPENXR_DEV_COMMON.md).

**Launch again:** `./run.sh` or `./run.sh packaged`.

## WSL2

1. Clone and `git lfs pull` in WSL.
2. Build `native/xr-sandbox` if you want an OpenXR smoke test without UE.
3. **Do not expect VR Preview in WSL** — use Windows (`setup.ps1`) or native Linux (`setup.sh`) for headset testing.

## Validation

Complete [DAY1_MAINTAINER_CHECKLIST.md](DAY1_MAINTAINER_CHECKLIST.md) when you need a maintainer-grade pass/fail.

## Platform reference

Full matrix: [PLATFORMS.md](PLATFORMS.md).
