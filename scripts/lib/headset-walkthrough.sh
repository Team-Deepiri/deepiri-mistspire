# Mistspire — interactive VR headset walkthrough (Linux).
# Source from setup.sh after ue-paths.sh. Requires ROOT to be set.
#
# mistspire_headset_walkthrough [non_interactive=0|1]
# Sets: MISTSPIRE_VR_MODE=0|1

mistspire_ask_yn() {
  local prompt="$1"
  local ans
  while true; do
    read -r -p "$prompt [y/n]: " ans || return 1
    case "${ans,,}" in
      y|yes) return 0 ;;
      n|no) return 1 ;;
      *) echo "   Please answer y or n." ;;
    esac
  done
}

mistspire_steam_present() {
  if mistspire_is_steamos 2>/dev/null; then
    return 0
  fi
  command -v steam >/dev/null 2>&1 && return 0
  [[ -x "$HOME/.local/share/Steam/steam.sh" ]] && return 0
  [[ -x "$HOME/.steam/steam/steam.sh" ]] && return 0
  [[ -d "/usr/games" ]] && command -v steam >/dev/null 2>&1 && return 0
  flatpak info com.valvesoftware.Steam &>/dev/null && return 0
  return 1
}

mistspire_offer_steam_install() {
  if mistspire_is_steamos 2>/dev/null; then
    echo "   Steam is built into SteamOS — use Desktop Mode; install SteamVR from the Steam library if needed."
    return 0
  fi
  if mistspire_steam_present; then
    echo "   [ok] Steam already present."
    return 0
  fi
  echo ""
  echo "   Steam was not found. Most PCVR paths need Steam + SteamVR OpenXR."
  if ! mistspire_ask_yn "   Install Steam now (distro package or Flatpak)?"; then
    echo "   Skipping Steam install — see your headset guide."
    return 0
  fi
  if [[ -f /etc/debian_version ]]; then
    echo "   Installing Steam via apt..."
    sudo dpkg --add-architecture i386 2>/dev/null || true
    sudo apt-get update
    sudo apt-get install -y steam-installer 2>/dev/null || sudo apt-get install -y steam || true
  elif mistspire_is_arch_linux 2>/dev/null; then
    echo "   Installing Steam via pacman..."
    sudo pacman -S --needed --noconfirm steam || true
  elif command -v dnf >/dev/null; then
    echo "   Installing Steam via dnf..."
    sudo dnf install -y steam || true
  elif command -v flatpak >/dev/null; then
    echo "   Installing Steam via Flatpak..."
    flatpak install -y flathub com.valvesoftware.Steam || true
  else
    echo "   !! No supported package manager for Steam."
    echo "      Download: https://store.steampowered.com/about/"
  fi
  if mistspire_steam_present; then
    echo "   [ok] Steam install looks present."
  else
    echo "   [--] Steam still not detected — install from https://store.steampowered.com/about/"
  fi
}

mistspire_print_headset_guide() {
  local key="$1"
  local rel=""
  local streaming=0
  local label=""

  case "$key" in
    1)
      label="Meta Quest / Oculus Rift"
      rel="docs/setup/headsets/meta_quest.md"
      streaming=1
      ;;
    2)
      label="Valve Index / HTC Vive (Lighthouse)"
      rel="docs/setup/headsets/steamvr_lighthouse.md"
      ;;
    3)
      label="HTC Vive Focus / XR Elite"
      rel="docs/setup/headsets/htc_vive_focus.md"
      streaming=1
      ;;
    4)
      label="Windows MR / HP Reverb"
      rel="docs/setup/headsets/windows_mixed_reality.md"
      ;;
    5)
      label="Pico"
      rel="docs/setup/headsets/pico.md"
      streaming=1
      ;;
    6)
      label="Steam Frame"
      rel="docs/setup/headsets/steam_frame.md"
      ;;
    7)
      label="Other / unsure"
      rel="docs/setup/headsets/generic.md"
      streaming=1
      ;;
    *)
      return 1
      ;;
  esac

  echo ""
  echo "   Headset: $label"
  echo "   Guide:   $ROOT/$rel"
  echo "   OpenXR:  $ROOT/docs/setup/OPENXR_DEV_COMMON.md"
  if [[ "$streaming" -eq 1 ]]; then
    echo "   Stream:  $ROOT/docs/setup/headsets/streaming_options.md"
  fi
  echo "   Index:   $ROOT/docs/setup/headsets/README.md"
  echo ""
  echo "   Follow the guide for your OS (Windows vs Linux sections), then set SteamVR"
  echo "   (or the path-specific runtime) as the active OpenXR runtime."
}

mistspire_pick_headset() {
  local choice
  echo ""
  echo "   Which headset do you have?"
  echo "     1) Meta Quest / Oculus Rift"
  echo "     2) Valve Index / HTC Vive (Lighthouse)"
  echo "     3) HTC Vive Focus / XR Elite"
  echo "     4) Windows MR / HP Reverb"
  echo "     5) Pico"
  echo "     6) Steam Frame"
  echo "     7) Other / unsure"
  while true; do
    read -r -p "   Choice [1-7]: " choice || return 1
    case "$choice" in
      1|2|3|4|5|6|7)
        mistspire_print_headset_guide "$choice"
        return 0
        ;;
      *) echo "   Enter a number from 1 to 7." ;;
    esac
  done
}

# Sets MISTSPIRE_VR_MODE=0|1
mistspire_headset_walkthrough() {
  local non_interactive="${1:-0}"
  MISTSPIRE_VR_MODE=0

  echo ""
  echo "==> Headset / VR walkthrough"

  if [[ "$non_interactive" -eq 1 ]] || [[ ! -t 0 ]]; then
    echo "   Non-interactive (or no TTY) — skipping headset prompts."
    echo "   Guides: $ROOT/docs/setup/headsets/README.md"
    MISTSPIRE_VR_MODE=0
    return 0
  fi

  if ! mistspire_ask_yn "   Do you have a VR headset for Mistspire PCVR?"; then
    echo "   No headset — skipping VR walkthrough (non-VR / desktop path later)."
    echo "   You can still open the editor; VR Preview is optional for now."
    MISTSPIRE_VR_MODE=0
    return 0
  fi

  MISTSPIRE_VR_MODE=1

  if mistspire_ask_yn "   Is your headset already connected and ready on this PC?"; then
    echo "   Great — continuing to dependency install. OpenXR check runs later."
    return 0
  fi

  echo ""
  echo "   We'll point you at the right headset guide before installing deps."
  mistspire_pick_headset || {
    echo "   Could not read headset choice — continuing without a specific guide."
    return 0
  }

  mistspire_offer_steam_install

  echo ""
  read -r -p "   Press Enter when you have completed the headset guide steps (or want to continue anyway)..." _
  echo "   Continuing to dependency install..."
}
