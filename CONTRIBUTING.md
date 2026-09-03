# Contributing to Mistspire

## Prerequisites

- UE **5.8+**, Linux or Windows
- Git LFS (`git lfs install` + `git lfs pull`)
- Follow [docs/setup/PCVR_DEV_SETUP.md](docs/setup/PCVR_DEV_SETUP.md)

## C++ modules

- `game/Source/Mistspire` — game code
- `game/Plugins/MistspireOpenXRNative` — OpenXR bridge

After pulling, regenerate project files if needed and rebuild both modules.

## OpenXR bindings

Edit JSON under `interaction_profiles/openxr/`, keep action names aligned with `UMistspireXRActionSubsystem`.

## Maps

`Main_WP` and related binaries use **Git LFS**. Do not commit `.umap` / `.uasset` without LFS. Document new summits in [docs/gameplay/WORLD_DESIGN.md](docs/gameplay/WORLD_DESIGN.md). Map authoring: [game/Content/Maps/README.md](game/Content/Maps/README.md).

## CI

Hosted GitHub Actions cannot build Unreal. PRs should keep green:

- `validate.yml` — JSON, `shellcheck -x setup.sh run.sh scripts/*.sh`, pinned Ruff (`ruff==0.16.0`); docs-only PRs still report via `CI gate`. No `push` trigger (avoids post-merge double-fire); Monday 06:00 UTC `schedule` plus `workflow_dispatch` cover `main`. The `Build xr-sandbox (native, no Unreal Editor)` check name is a stub (not a compile); cmake runs in CodeQL when the gate sets `run_code=true`.
- `codeql.yml` — C++ analysis after building `native/xr-sandbox`. `push` to `main` plus the weekly schedule keep the default-branch GHAS config so the PR **CodeQL** check is not `neutral`. Docs-only PRs upload empty SARIF (no extra `category`) so the upload still matches `codeql.yml:analyze`.

See [.github/codeql/README.md](.github/codeql/README.md).

## Commits

Apache 2.0 codebase — use clear, complete commit messages per Team Deepiri conventions.
