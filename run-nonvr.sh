#!/usr/bin/env bash
# Mistspire — non-VR launch (keyboard/mouse)
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

echo "============================================"
echo "  Mistspire — Non-VR Mode"
echo "============================================"
echo ""
echo "Once the editor loads, click Play (NOT VR Preview)."
echo "Controls: docs/setup/NONVR_MODE.md"
echo ""

UE_EDITOR=""
CANDIDATES=(
  "${UE_ROOT:-}/Engine/Binaries/Linux/UnrealEditor"
  "$HOME/UE_5.8/Engine/Binaries/Linux/UnrealEditor"
  "$HOME/UE_5.5/Engine/Binaries/Linux/UnrealEditor"
)

for cand in "${CANDIDATES[@]}"; do
  if [[ -n "$cand" && -x "$cand" ]]; then
    UE_EDITOR="$cand"
    break
  fi
done

if [[ -z "$UE_EDITOR" ]]; then
  echo "!! Unreal Editor not found. Set UE_ROOT or open game/Mistspire.uproject manually."
  exit 1
fi

UPROJECT="$ROOT/game/Mistspire.uproject"
exec "$UE_EDITOR" "$UPROJECT" -nonvr
