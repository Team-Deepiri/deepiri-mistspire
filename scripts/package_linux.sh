#!/usr/bin/env bash
# Package Mistspire Linux - see tools/pipeline/step_17_README.md for full flow.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
UPROJECT="$ROOT/game/Mistspire.uproject"

cat <<EOF
==> Mistspire Linux packaging

Requires: UE 5.8 Linux toolchain + Vulkan drivers
See: $ROOT/tools/pipeline/step_17_README.md

Example (set UE_ROOT to your engine install):
  "\$UE_ROOT/Engine/Build/BatchFiles/Linux/Build.sh" \\
    -project=$UPROJECT -platform=Linux -configuration=Shipping

Output (typical): game/Package/Linux/
After packaging: ./run.sh packaged
EOF
