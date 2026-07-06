#!/usr/bin/env bash
# Mistspire — Linux development dependencies (native + optional WSL).
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
echo "==> Mistspire dev setup (root: $ROOT)"

if [[ -f /etc/debian_version ]]; then
  echo "==> Installing Debian/Ubuntu packages..."
  sudo apt-get update
  sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    git-lfs \
    vulkan-tools \
    libvulkan-dev \
    libopenxr-dev \
    pkg-config \
    python3
  if ! command -v git-lfs >/dev/null; then
    echo "git-lfs not found after install" >&2
    exit 1
  fi
  git lfs install --skip-repo 2>/dev/null || git lfs install
elif command -v dnf >/dev/null; then
  echo "==> Installing Fedora packages..."
  sudo dnf install -y \
    gcc-c++ \
    cmake \
    git \
    git-lfs \
    vulkan-tools \
    libvulkan-devel \
    openxr-loader-devel \
    pkgconf-pkg-config \
    python3
  git lfs install --skip-repo 2>/dev/null || git lfs install
else
  echo "Unsupported distro — install manually: cmake, vulkan, openxr loader, git-lfs" >&2
  exit 1
fi

echo ""
echo "==> Unreal Engine 5.5+"
echo "    Install via Epic Launcher (Linux). Open: $ROOT/game/Mistspire.uproject"
echo ""
echo "==> WSL note"
echo "    WSL2 is OK for editing and xr-sandbox builds."
echo "    USB HMD VR playtests: use native Linux or Windows — not WSL."
echo "    See: $ROOT/docs/PCVR_DEV_SETUP.md"
echo ""
echo "==> Next: ./scripts/verify-openxr-runtime.sh"
