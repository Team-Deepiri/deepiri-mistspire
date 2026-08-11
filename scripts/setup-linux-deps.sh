#!/usr/bin/env bash
# Mistspire — Linux development dependencies (native + optional WSL).
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
# shellcheck source=lib/ue-paths.sh
# shellcheck disable=SC1091  # dynamic ROOT; path given above for shellcheck -x
source "$ROOT/scripts/lib/ue-paths.sh"

echo "==> Mistspire dev setup (root: $ROOT)"

# Install listed pacman packages that exist in the sync DB (or are already installed).
mistspire_pacman_install_available() {
  local pkg available=()
  for pkg in "$@"; do
    if pacman -Q "$pkg" &>/dev/null || pacman -Si "$pkg" &>/dev/null; then
      available+=("$pkg")
    else
      echo "   [--] skip (not in SteamOS/Arch repos): $pkg"
    fi
  done
  if [[ ${#available[@]} -eq 0 ]]; then
    echo "!! No installable packages found in the requested set." >&2
    return 1
  fi
  sudo pacman -S --needed --noconfirm "${available[@]}"
}

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
elif mistspire_is_steamos; then
  echo "==> Installing SteamOS packages (immutable root)..."
  echo "   NOTE: Never use pacman -Syu on SteamOS (breaks A/B updates)."
  echo "   NOTE: pacman installs are wiped by the next steamos-update — re-run this script after OS updates."
  echo "   Prerequisite: set a sudo password if needed (Desktop Mode: passwd)."
  if ! command -v steamos-readonly >/dev/null; then
    echo "!! steamos-readonly not found — unexpected SteamOS layout." >&2
    exit 1
  fi
  # Always re-enable read-only, even if pacman fails.
  mistspire_steamos_restore_ro() {
    sudo steamos-readonly enable || true
  }
  trap mistspire_steamos_restore_ro EXIT
  sudo steamos-readonly disable
  # Keyring is often uninitialized on a fresh Deck.
  sudo pacman-key --init 2>/dev/null || true
  sudo pacman-key --populate archlinux holo 2>/dev/null || \
    sudo pacman-key --populate archlinux 2>/dev/null || true
  # Refresh DB only — do not -Syu.
  sudo pacman -Sy --noconfirm
  # Prefer individual Vulkan packages; vulkan-devel group may be absent on holo.
  mistspire_pacman_install_available \
    base-devel \
    cmake \
    git \
    git-lfs \
    vulkan-headers \
    vulkan-tools \
    vulkan-icd-loader \
    openxr \
    pkgconf \
    python
  trap - EXIT
  mistspire_steamos_restore_ro
  if command -v git-lfs >/dev/null; then
    git lfs install --skip-repo 2>/dev/null || git lfs install
  else
    echo "   [--] git-lfs unavailable in SteamOS repos — install manually or skip LFS pull."
  fi
  echo "   SteamOS note: full Unreal Editor on Deck is uncommon; use a desktop Linux/Windows PC for UE."
  echo "   Useful here: git, xr-sandbox build, OpenXR/SteamVR checks in Desktop Mode."
elif mistspire_is_arch_linux; then
  echo "==> Installing Arch Linux packages..."
  # Sync + upgrade before install (Arch: avoid -Sy without -u).
  # base-devel and vulkan-devel are package groups (pkgconf / vulkan-tools included).
  sudo pacman -Syu --needed --noconfirm \
    base-devel \
    cmake \
    git \
    git-lfs \
    vulkan-devel \
    openxr \
    python
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
  echo "    Supported: Debian/Ubuntu (apt), Arch (pacman), SteamOS (pacman + steamos-readonly), Fedora (dnf)." >&2
  exit 1
fi

echo ""
echo "==> Unreal Engine 5.8+"
echo "    Install via Epic Launcher (Linux). Open: $ROOT/game/Mistspire.uproject"
echo ""
echo "==> WSL note"
echo "    WSL2 is OK for editing and xr-sandbox builds."
echo "    USB HMD VR playtests: use native Linux or Windows — not WSL."
echo "    See: $ROOT/docs/setup/PLATFORMS.md"
echo ""
echo "==> Next: ./scripts/verify-openxr-runtime.sh"
