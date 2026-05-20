# Contributing to Mistspire

## Prerequisites

- UE 5.5+, Linux or Windows
- Follow [docs/PCVR_DEV_SETUP.md](docs/PCVR_DEV_SETUP.md)

## C++ modules

- `game/Source/Mistspire` — game code
- `game/Plugins/MistspireOpenXRNative` — OpenXR bridge

After pulling, regenerate project files if needed and rebuild both modules.

## OpenXR bindings

Edit JSON under `interaction_profiles/openxr/`, keep action names aligned with `UMistspireXRActionSubsystem`.

## Maps

Do not commit `.umap` without Git LFS. Document new summits in [docs/WORLD_DESIGN.md](docs/WORLD_DESIGN.md).

## Commits

Apache 2.0 codebase — use clear, complete commit messages per Team Deepiri conventions.
