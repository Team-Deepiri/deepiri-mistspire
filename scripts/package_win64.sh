#!/usr/bin/env bash
# Package Mistspire Win64 — see tools/pipeline/step_17_README.md for full UAT flow.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
UPROJECT="$ROOT/game/Mistspire.uproject"

echo "==> Mistspire Win64 packaging"
echo ""
echo "Primary path: UE Editor → Platforms → Windows → Package Project"
echo "Or RunUAT BuildCookRun (Win64 Shipping) — see:"
echo "  $ROOT/tools/pipeline/step_17_README.md"
echo ""
echo "Project: $UPROJECT"
echo "Output (typical): game/Package/Win64/"
echo ""
echo "After packaging: .\\run.ps1 packaged"
