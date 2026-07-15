#!/usr/bin/env bash
# Mistspire — one-command setup: install everything, then launch UE editor.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

echo "============================================"
echo "  Mistspire — Full Setup"
echo "============================================"
FAILED=0

# ── 0. Git Hooks ────────────────────────────────
echo ""
echo "==> [0/6] Configuring git hooks..."
if [ -d ".git-hooks" ]; then
    git config core.hooksPath .git-hooks
    echo "   Git hooks configured (core.hooksPath = .git-hooks)"
else
    echo "   No .git-hooks directory found, skipping hooks setup"
fi

# ── 1. System dependencies ──────────────────────
echo ""
echo "==> [1/6] Installing system dependencies..."
if bash ./scripts/setup-linux-deps.sh; then
  echo "   Dependencies installed."
else
  echo "   Warning: dep install had issues (non-fatal)."
fi

# ── 2. Git LFS ──────────────────────────────────
echo ""
echo "==> [2/6] Pulling Git LFS assets..."
if command -v git-lfs &>/dev/null || git lfs version &>/dev/null; then
  git lfs pull 2>/dev/null && echo "   LFS pulled." || echo "   No LFS assets (fine)."
else
  echo "   git-lfs not installed — skip LFS pull."
fi

# ── 3. UE5 engine detection ─────────────────────
echo ""
echo "==> [3/6] Locating Unreal Engine 5.5..."
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
  echo "!! Could not find Unreal Engine 5.5."
  echo "   Set UE_ROOT=/path/to/UE_5.5 or install via Epic Launcher."
  echo "   Open manually: game/Mistspire.uproject"
  SKIP_UE_LAUNCH=1
else
  echo "   Found: $UE_EDITOR"
  SKIP_UE_LAUNCH=0
fi

# ── 4. OpenXR verification ──────────────────────
echo ""
echo "==> [4/6] Verifying OpenXR runtime..."
bash ./scripts/verify-openxr-runtime.sh 2>/dev/null && echo "   OpenXR OK." || echo "   (OpenXR check non-fatal)"

# ── 5. Build C++ modules ────────────────────────
echo ""
echo "==> [5/6] Building C++ game modules..."
UPROJECT="$ROOT/game/Mistspire.uproject"

if [[ "$SKIP_UE_LAUNCH" -eq 0 ]]; then
  BUILD_SH="$UE_ROOT/Engine/Build/BatchFiles/Linux/Build.sh"
  UBT_DLL="$UE_ROOT/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.dll"
  UBT_BIN="$UE_ROOT/Engine/Binaries/Linux/UnrealBuildTool"
  BUILT=0

  if [[ -x "$BUILD_SH" ]]; then
    echo "   Building via Build.sh..."
    if "$BUILD_SH" MistspireEditor Linux Development "$UPROJECT" -Progress 2>&1 | tail -20; then
      echo "   Build succeeded."
      BUILT=1
    else
      echo "   Build FAILED — check errors above."
      FAILED=1
    fi
  elif [[ -f "$UBT_DLL" ]] && command -v dotnet &>/dev/null; then
    echo "   Building via dotnet UBT..."
    if dotnet "$UBT_DLL" MistspireEditor Linux Development -Project="$UPROJECT" -Progress 2>&1 | tail -20; then
      echo "   Build succeeded."
      BUILT=1
    else
      echo "   Build FAILED — check errors above."
      FAILED=1
    fi
  elif [[ -x "$UBT_BIN" ]]; then
    echo "   Building via native UBT..."
    if "$UBT_BIN" MistspireEditor Linux Development -Project="$UPROJECT" -Progress 2>&1 | tail -20; then
      echo "   Build succeeded."
      BUILT=1
    else
      echo "   Build FAILED — check errors above."
      FAILED=1
    fi
  else
    echo "   No UBT/Build.sh found — editor will compile on first launch."
  fi
fi

# ── 6. Launch editor ────────────────────────────
echo ""
echo "==> [6/6] Launching Unreal Editor..."
if [[ "$SKIP_UE_LAUNCH" -eq 0 ]] && [[ "$FAILED" -eq 0 ]]; then
  echo "   Opening Mistspire.uproject..."
  "$UE_EDITOR" "$ROOT/game/Mistspire.uproject" &
  echo "   Editor launching (PID $!)"
  echo "   Once loaded: Play -> VR Preview to test"
else
  echo "   Open manually: game/Mistspire.uproject"
fi

echo ""
echo "============================================"
echo "  Setup complete."
echo "============================================"
echo ""
echo "  ./run.sh              Launch game"
echo "  mistspire.TeleportUp 5000   (in-game)"
echo ""
exit $FAILED
