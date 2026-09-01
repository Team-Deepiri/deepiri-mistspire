#!/usr/bin/env bash
# Mistspire — launch game (VR Preview via editor, or packaged build) on Linux.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

# shellcheck source=scripts/lib/ue-paths.sh
source "$ROOT/scripts/lib/ue-paths.sh"

if mistspire_is_windows_shell; then
  echo "On Windows (Git Bash), use: powershell -File run.ps1"
  echo "See: docs/setup/PLATFORMS.md"
  exit 0
fi

MODE="${1:-editor}"   # "editor" (VR Preview) or "packaged"

echo "============================================"
echo "  Mistspire — Launch (Linux)"
echo "============================================"

# ── Verify OpenXR runtime ──────────────────────
echo ""
echo "==> Checking OpenXR runtime..."
./scripts/verify-openxr-runtime.sh || true

UPROJECT="$ROOT/game/Mistspire.uproject"

# ── Launch ──────────────────────────────────────
if [[ "$MODE" == "editor" ]]; then
  if ! mistspire_find_ue_linux; then
    echo "!! Unreal Engine 5.8 not found."
    echo "   Set UE_ROOT=/path/to/UE_5.8 or install via Epic Launcher."
    echo "   Windows: powershell -File setup.ps1"
    exit 1
  fi
  echo ""
  echo "==> Launching UE Editor..."
  echo "   Once loaded: Play -> VR Preview (headset) or Play (non-VR keyboard/mouse)"
  echo "   Non-VR shortcut: ./run-nonvr.sh"
  echo ""
  "$UE_EDITOR" "$UPROJECT" 2>&1
elif [[ "$MODE" == "packaged" ]]; then
  PACKAGE_DIR="$ROOT/game/Package/Linux"
  if [[ -x "$PACKAGE_DIR/Mistspire.sh" ]]; then
    echo ""
    echo "==> Launching packaged build..."
    "$PACKAGE_DIR/Mistspire.sh" -vr 2>&1
  else
    echo "!! No packaged build found at $PACKAGE_DIR"
    echo "   Package first — see scripts/package_linux.sh"
    exit 1
  fi
else
  echo "!! Unknown mode: $MODE"
  echo "   Usage: ./run.sh [editor|packaged]"
  exit 1
fi
