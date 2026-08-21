#!/usr/bin/env bash
# Package Mistspire Win64 - see tools/pipeline/step_17_README.md for full UAT flow.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
UPROJECT="$ROOT/game/Mistspire.uproject"

cat <<EOF
==> Mistspire Win64 packaging

Primary path: UE Editor -> Platforms -> Windows -> Package Project
Or RunUAT BuildCookRun (Win64 Shipping) - see:
  $ROOT/tools/pipeline/step_17_README.md

Project: $UPROJECT
Output (typical): game/Package/Win64/
After packaging: powershell -File run.ps1 packaged
EOF
