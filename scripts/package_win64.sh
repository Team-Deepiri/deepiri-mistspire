#!/usr/bin/env bash
# Package Mistspire Win64 - see tools/pipeline/step_17_README.md for full UAT flow.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
UPROJECT="$ROOT/game/Mistspire.uproject"
ARCHIVE_DIR="$ROOT/game/Package/Win64"

if [[ -z "${UE_ROOT:-}" ]]; then
  cat <<EOF
==> Mistspire Win64 packaging

Primary path: UE Editor -> Platforms -> Windows -> Package Project
Or RunUAT BuildCookRun (Win64 Shipping) - see:
  $ROOT/tools/pipeline/step_17_README.md

Set UE_ROOT to your Unreal Engine install to run this script directly, e.g.:
  export UE_ROOT=/path/to/UE_5.8
  ./scripts/package_win64.sh

Project: $UPROJECT
Output (typical): game/Package/Win64/
After packaging: powershell -File run.ps1 packaged
EOF
  exit 1
fi

UAT="$UE_ROOT/Engine/Build/BatchFiles/RunUAT.sh"
if [[ ! -x "$UAT" ]]; then
  UAT="$UE_ROOT/Engine/Build/BatchFiles/RunUAT.bat"
fi

echo "==> Cooking and packaging Mistspire Win64..."
echo "    Project: $UPROJECT"
echo "    Output:  $ARCHIVE_DIR"

"$UAT" BuildCookRun \
  -project="$UPROJECT" \
  -platform=Win64 \
  -clientconfig=Development \
  -cook -stage -pak -archive \
  -archivedirectory="$ARCHIVE_DIR"

echo ""
echo "==> Done. Launch non-VR packaged build:"
echo "    powershell -File scripts/launch_packaged_win64.ps1"
echo "    or: game/Package/Win64/.../Mistspire.exe -nonvr"
