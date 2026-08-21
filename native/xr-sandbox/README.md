# Mistspire xr-sandbox

Standalone OpenXR loader test (no Unreal). Validates your Linux dev environment before opening UE.

## Build

```bash
./scripts/setup-linux-deps.sh   # OpenXR loader + Vulkan (apt / pacman / SteamOS / dnf)
cd native/xr-sandbox
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/mistspire_xr_sandbox
```

Uses **pkg-config** (`openxr`) when available (Debian/Ubuntu, Arch, SteamOS, Fedora); falls back to `find_package(OpenXR)` elsewhere.

## Expected output

```
Mistspire xr-sandbox OK — OpenXR instance + HMD system id=...
```

## Exit codes

| Code | Meaning |
|------|---------|
| 1 | No OpenXR runtime / loader |
| 2 | Runtime up but no HMD (OK on headless CI) |

```bash
export XR_RUNTIME_JSON=/usr/share/openxr/openxr_monado.json
```
