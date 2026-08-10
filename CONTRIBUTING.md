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

- `validate.yml` — JSON, shellcheck, pinned Ruff (`ruff==0.16.0`), `native/xr-sandbox` cmake build
- `codeql.yml` — C++ analysis after building `native/xr-sandbox`

See [.github/codeql/README.md](.github/codeql/README.md).

## Commits

Apache 2.0 codebase — use clear, complete commit messages per Team Deepiri conventions.
