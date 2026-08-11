#!/usr/bin/env bash
# Mistspire — one-command setup: install everything, then launch UE editor (Linux native).
# Usage: ./setup.sh [--non-interactive]
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

NON_INTERACTIVE=0
for arg in "$@"; do
  case "$arg" in
    --non-interactive|-y|--yes) NON_INTERACTIVE=1 ;;
    -h|--help)
      echo "Usage: ./setup.sh [--non-interactive]"
      echo "  Interactive headset walkthrough, then deps / LFS / UE / OpenXR / build / launch."
      exit 0
      ;;
  esac
done

# shellcheck source=scripts/lib/ue-paths.sh
source "$ROOT/scripts/lib/ue-paths.sh"
# shellcheck source=scripts/lib/headset-walkthrough.sh
source "$ROOT/scripts/lib/headset-walkthrough.sh"

if mistspire_is_windows_shell; then
  echo "============================================"
  echo "  Mistspire — Windows detected (Git Bash)"
  echo "============================================"
  echo ""
  echo "  Use PowerShell setup instead:"
  echo "    powershell -File setup.ps1"
  echo ""
  echo "  See: docs/setup/PLATFORMS.md"
  exit 0
fi

if mistspire_is_wsl; then
  echo "[WSL] Native Linux path — USB HMD VR playtests may not work in WSL."
  echo "      Use Windows (setup.ps1) or native Linux for headset testing."
  echo "      See: docs/setup/PLATFORMS.md"
  echo ""
fi

echo "============================================"
echo "  Mistspire — Full Setup (Linux)"
echo "============================================"
FAILED=0
MISTSPIRE_VR_MODE=0

# ── 0. Headset / VR walkthrough ─────────────────
mistspire_headset_walkthrough "$NON_INTERACTIVE"

# ── 1. Git Hooks ────────────────────────────────
echo ""
echo "==> [1/7] Configuring git hooks..."
if [ -d ".git-hooks" ]; then
    git config core.hooksPath .git-hooks
    echo "   Git hooks configured (core.hooksPath = .git-hooks)"
else
    echo "   No .git-hooks directory found, skipping hooks setup"
fi

# ── 2. System dependencies ──────────────────────
echo ""
echo "==> [2/7] Installing system dependencies..."
if bash ./scripts/setup-linux-deps.sh; then
  echo "   Dependencies installed."
else
  echo "   Warning: dep install had issues (non-fatal)."
fi

# ── 3. Git LFS ──────────────────────────────────
echo ""
echo "==> [3/7] Pulling Git LFS assets..."
if command -v git-lfs &>/dev/null || git lfs version &>/dev/null; then
  git lfs pull 2>/dev/null && echo "   LFS pulled." || echo "   No LFS assets (fine)."
else
  echo "   git-lfs not installed — skip LFS pull."
fi

# ── 4. UE5 engine detection ─────────────────────
echo ""
echo "==> [4/7] Locating Unreal Engine 5.8..."
SKIP_UE_LAUNCH=1
if mistspire_find_ue_linux; then
  echo "   Found: $UE_EDITOR"
  SKIP_UE_LAUNCH=0
else
  echo "!! Could not find Unreal Engine 5.8."
  echo "   Set UE_ROOT=/path/to/UE_5.8 or install via Epic Launcher."
  echo "   Open manually: game/Mistspire.uproject"
  echo "   Windows: powershell -File setup.ps1"
fi

# ── 5. OpenXR verification ──────────────────────
echo ""
echo "==> [5/7] Verifying OpenXR runtime..."
if [[ "${MISTSPIRE_VR_MODE:-0}" -eq 0 ]]; then
  echo "   Skipped VR walkthrough — OpenXR check is informational only."
fi
bash ./scripts/verify-openxr-runtime.sh 2>/dev/null && echo "   OpenXR OK." || echo "   (OpenXR check non-fatal)"

# ── 6. Build C++ modules ────────────────────────
echo ""
echo "==> [6/7] Building C++ game modules..."
UPROJECT="$ROOT/game/Mistspire.uproject"

if [[ "$SKIP_UE_LAUNCH" -eq 0 ]]; then
  BUILD_SH="$UE_ROOT/Engine/Build/BatchFiles/Linux/Build.sh"
  UBT_DLL="$UE_ROOT/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.dll"
  UBT_BIN="$UE_ROOT/Engine/Binaries/Linux/UnrealBuildTool"

  # Use PIPESTATUS[0] so tail does not mask UBT failure (pipefail is set).
  if [[ -x "$BUILD_SH" ]]; then
    echo "   Building via Build.sh..."
    "$BUILD_SH" MistspireEditor Linux Development "$UPROJECT" -Progress 2>&1 | tail -20
    if [[ ${PIPESTATUS[0]} -eq 0 ]]; then
      echo "   Build succeeded."
    else
      echo "   Build FAILED — check errors above."
      FAILED=1
    fi
  elif [[ -f "$UBT_DLL" ]] && command -v dotnet &>/dev/null; then
    echo "   Building via dotnet UBT..."
    dotnet "$UBT_DLL" MistspireEditor Linux Development -Project="$UPROJECT" -Progress 2>&1 | tail -20
    if [[ ${PIPESTATUS[0]} -eq 0 ]]; then
      echo "   Build succeeded."
    else
      echo "   Build FAILED — check errors above."
      FAILED=1
    fi
  elif [[ -x "$UBT_BIN" ]]; then
    echo "   Building via native UBT..."
    "$UBT_BIN" MistspireEditor Linux Development -Project="$UPROJECT" -Progress 2>&1 | tail -20
    if [[ ${PIPESTATUS[0]} -eq 0 ]]; then
      echo "   Build succeeded."
    else
      echo "   Build FAILED — check errors above."
      FAILED=1
    fi
  else
    echo "   No UBT/Build.sh found — editor will compile on first launch."
  fi
fi

# ── 7. Launch editor ────────────────────────────
echo ""
echo "==> [7/7] Launching Unreal Editor..."
if [[ "$SKIP_UE_LAUNCH" -eq 0 ]] && [[ "$FAILED" -eq 0 ]]; then
  echo "   Opening Mistspire.uproject..."
  "$UE_EDITOR" "$ROOT/game/Mistspire.uproject" &
  echo "   Editor launching (PID $!)"
  if [[ "${MISTSPIRE_VR_MODE:-0}" -eq 1 ]]; then
    echo "   Once loaded: Play -> VR Preview to test"
  else
    echo "   VR Preview optional — no headset walkthrough this run."
  fi
else
  echo "   Open manually: game/Mistspire.uproject"
fi

echo ""
echo "============================================"
echo "  Setup complete."
echo "============================================"
echo ""
echo "  ./run.sh              Launch game (Linux)"
echo "  docs/setup/DEV_BOOTSTRAP.md"
if [[ "${MISTSPIRE_VR_MODE:-0}" -eq 1 ]]; then
  echo "  mistspire.TeleportUp 5000   (in-game)"
else
  echo "  Headset guides (later): docs/setup/headsets/README.md"
fi
echo ""
exit $FAILED
