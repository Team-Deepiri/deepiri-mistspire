#!/usr/bin/env bash
# Mistspire — launch game (VR Preview via editor, or packaged build)
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

MODE="${1:-editor}"   # "editor" (VR Preview) or "packaged"

echo "============================================"
echo "  Mistspire — Launch"
echo "============================================"

# ── Verify OpenXR runtime ──────────────────────
echo ""
echo "==> Checking OpenXR runtime..."
./scripts/verify-openxr-runtime.sh || true

# ── Locate UE editor ────────────────────────────
UE_EDITOR=""
CANDIDATES=(
  "$HOME/UnrealEngine/5.5/Engine/Binaries/Linux/UnrealEditor"
  "$HOME/UE_5.5/Engine/Binaries/Linux/UnrealEditor"
  "/opt/unreal-engine/Engine/Binaries/Linux/UnrealEditor"
  "/usr/local/unreal-engine/Engine/Binaries/Linux/UnrealEditor"
)

for cand in "${CANDIDATES[@]}"; do
  if [[ -x "$cand" ]]; then
    UE_EDITOR="$cand"
    UE_ROOT="$(dirname "$(dirname "$(dirname "$cand")")")"
    break
  fi
done

if [[ -z "$UE_EDITOR" && -n "${UE_DOXY_ENGINE_ROOT:-}" && -x "$UE_DOXY_ENGINE_ROOT/Engine/Binaries/Linux/UnrealEditor" ]]; then
  UE_EDITOR="$UE_DOXY_ENGINE_ROOT/Engine/Binaries/Linux/UnrealEditor"
  UE_ROOT="$UE_DOXY_ENGINE_ROOT"
elif [[ -z "$UE_EDITOR" && -n "${UE_ROOT:-}" && -x "$UE_ROOT/Engine/Binaries/Linux/UnrealEditor" ]]; then
  UE_EDITOR="$UE_ROOT/Engine/Binaries/Linux/UnrealEditor"
fi

if [[ -z "$UE_EDITOR" ]]; then
  echo "!! Unreal Engine 5.5 not found."
  echo "   Set UE_ROOT=/path/to/UE_5.5 or install via Epic Launcher."
  echo "   Then open game/Mistspire.uproject manually."
  exit 1
fi

UPROJECT="$ROOT/game/Mistspire.uproject"

# ── Launch ──────────────────────────────────────
if [[ "$MODE" == "editor" ]]; then
  echo ""
  echo "==> Launching UE Editor..."
  echo "   Once loaded: Play -> VR Preview to test"
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
    echo "   Package first, or use 'editor' mode."
    exit 1
  fi
else
  echo "!! Unknown mode: $MODE"
  echo "   Usage: ./run.sh [editor|packaged]"
  exit 1
fi
