#!/usr/bin/env bash
# Verify OpenXR loader and optional runtimes on Linux.
set -euo pipefail

echo "==> Mistspire OpenXR runtime check"

ok=0

if command -v vulkaninfo >/dev/null; then
  echo "[ok] vulkaninfo present"
  vulkaninfo --summary 2>/dev/null | head -n 20 || echo "    (vulkaninfo ran with warnings)"
else
  echo "[!!] vulkaninfo not found — install vulkan-tools"
  ok=1
fi

for lib in libopenxr_loader.so libopenxr_loader.so.1; do
  if ldconfig -p 2>/dev/null | grep -q "$lib"; then
    echo "[ok] $lib in loader cache"
    break
  fi
done

if [[ -n "${XR_RUNTIME_JSON:-}" ]]; then
  echo "[ok] XR_RUNTIME_JSON=$XR_RUNTIME_JSON"
  if [[ -f "$XR_RUNTIME_JSON" ]]; then
    echo "    manifest exists"
  else
    echo "[!!] manifest file missing"
    ok=1
  fi
else
  echo "[--] XR_RUNTIME_JSON not set (loader picks default runtime)"
  for candidate in \
    /usr/share/openxr/openxr_monado.json \
    /usr/share/openxr/1/openxr_monado.json; do
    if [[ -f "$candidate" ]]; then
      echo "    found: $candidate (use: export XR_RUNTIME_JSON=$candidate)"
    fi
  done
fi

if command -v openxr-info >/dev/null; then
  echo "[ok] openxr-info:"
  openxr-info 2>/dev/null | head -n 30 || true
elif command -v xr-list >/dev/null; then
  echo "[ok] xr-list:"
  xr-list 2>/dev/null || true
else
  echo "[--] install monado-tools for openxr-info (optional)"
fi

if grep -qi microsoft /proc/version 2>/dev/null; then
  echo ""
  echo "[WSL] You are in WSL — HMD USB/display passthrough is unreliable."
  echo "      Use native Linux or Windows for VR Preview playtests."
  echo "      See docs/setup/PCVR_DEV_SETUP.md"
fi

if [[ $ok -ne 0 ]]; then
  exit 1
fi

echo ""
echo "==> Runtime check finished"
