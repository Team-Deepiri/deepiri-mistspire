# Platforms — where Mistspire runs

The **game project** targets **Linux + Win64** (`game/Mistspire.uproject`). Repo **scripts and automation** differ by host OS.

## Task matrix

| Task | Linux native | Windows | WSL2 |
|------|--------------|---------|------|
| Clone / git hooks | `./setup.sh` | `.\setup.ps1` | Git + hooks only |
| System deps | `scripts/setup-linux-deps.sh` | `scripts/setup-windows-deps.ps1` (checks) | Linux deps script (no VR) |
| Git LFS / `Main_WP` | Both setup scripts | Both setup scripts | Yes |
| UE compile + editor | `./setup.sh` or open `.uproject` | `.\setup.ps1` or VS + `.uproject` | Open project; compile in editor |
| OpenXR check | `scripts/verify-openxr-runtime.sh` | `scripts/verify-openxr-runtime.ps1` | xr-sandbox only |
| VR playtest | SteamVR / Monado / WiVRn | SteamVR / Link / VD / Oasis | **Not supported** |
| Packaged run | `./run.sh packaged` (Linux) | `.\run.ps1 packaged` (Win64) | — |
| CI (GitHub) | validate + xr-sandbox | — | — |

## Which script to run

| Your machine | First command |
|--------------|---------------|
| **Windows** (native) | `powershell -File setup.ps1` |
| **Linux** (native) | `./setup.sh` |
| **WSL2** | Edit code here; run `setup.ps1` on Windows or `setup.sh` on Linux for VR |
| **macOS** | No automated setup; manual UE + headset docs |

## VR playtest

Use **native Windows or Linux** for headset playtests. WSL cannot reliably pass through USB HMDs.

Headset setup: [headsets/README.md](headsets/README.md).

## What this branch does not fix

Game module / `DefaultEngine.ini` platform tuning and hosted UE builds are **out of scope** for the scripts branch. See a future gameplay/platform branch if Win64/Linux behavior in-editor diverges.

## Related

- [DEV_BOOTSTRAP.md](DEV_BOOTSTRAP.md) — one-run flow per OS
- [PCVR_DEV_SETUP.md](PCVR_DEV_SETUP.md) — PCVR hub
- [DAY1_MAINTAINER_CHECKLIST.md](DAY1_MAINTAINER_CHECKLIST.md) — validation
