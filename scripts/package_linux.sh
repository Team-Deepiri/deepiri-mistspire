#!/usr/bin/env bash
# Package Mistspire Linux — see tools/pipeline/step_17_README.md for full flow.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
UPROJECT="$ROOT/game/Mistspire.uproject"

echo "==> Mistspire Linux packaging"
echo ""
echo "Requires: UE 5.8 Linux toolchain + Vulkan drivers"
echo "See: $ROOT/tools/pipeline/step_17_README.md"
echo ""
echo "Example:"
echo "  <UE>/Engine/Build/BatchFiles/Linux/Build.sh \\"
echo "    -project=$UPROJECT -platform=Linux -configuration=Shipping"
echo ""
echo "Output (typical): game/Package/Linux/"
echo "After packaging: ./run.sh packaged"
