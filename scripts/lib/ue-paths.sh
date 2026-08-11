# Mistspire — shared Unreal Engine 5.8 path discovery (Linux).
# Source from setup.sh / run.sh: source "$ROOT/scripts/lib/ue-paths.sh"
#
# On success sets globals: UE_EDITOR, UE_ROOT

mistspire_find_ue_linux() {
  # Preserve caller/env UE_ROOT — do not clear it before the env fallback check.
  local env_ue_root="${UE_ROOT:-}"
  local env_doxy="${UE_DOXY_ENGINE_ROOT:-}"

  UE_EDITOR=""
  UE_ROOT=""

  local candidates=(
    "$HOME/UnrealEngine/5.8/Engine/Binaries/Linux/UnrealEditor"
    "$HOME/UE_5.8/Engine/Binaries/Linux/UnrealEditor"
    "/opt/unreal-engine/Engine/Binaries/Linux/UnrealEditor"
    "/usr/local/unreal-engine/Engine/Binaries/Linux/UnrealEditor"
  )

  local cand
  for cand in "${candidates[@]}"; do
    if [[ -x "$cand" ]]; then
      UE_EDITOR="$cand"
      UE_ROOT="$(dirname "$(dirname "$(dirname "$cand")")")"
      return 0
    fi
  done

  if [[ -n "$env_doxy" && -x "$env_doxy/Engine/Binaries/Linux/UnrealEditor" ]]; then
    UE_EDITOR="$env_doxy/Engine/Binaries/Linux/UnrealEditor"
    UE_ROOT="$env_doxy"
    return 0
  fi

  if [[ -n "$env_ue_root" && -x "$env_ue_root/Engine/Binaries/Linux/UnrealEditor" ]]; then
    UE_EDITOR="$env_ue_root/Engine/Binaries/Linux/UnrealEditor"
    UE_ROOT="$env_ue_root"
    return 0
  fi

  return 1
}

mistspire_is_windows_shell() {
  case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) return 0 ;;
    *) return 1 ;;
  esac
}

mistspire_is_wsl() {
  grep -qi microsoft /proc/version 2>/dev/null
}

# SteamOS (Steam Deck / holo). Check before Arch — SteamOS sets ID_LIKE=arch.
mistspire_is_steamos() {
  if [[ ! -f /etc/os-release ]]; then
    return 1
  fi
  (
    # shellcheck disable=SC1091
    . /etc/os-release
    [[ "${ID:-}" == "steamos" ]]
  )
}

# Arch Linux and derivatives (Manjaro, EndeavourOS, CachyOS, …). Not SteamOS.
# Do not use `command -v pacman` — Debian ships an unrelated `pacman` game package.
mistspire_is_arch_linux() {
  if mistspire_is_steamos; then
    return 1
  fi
  if [[ -f /etc/arch-release ]]; then
    return 0
  fi
  if [[ ! -f /etc/os-release ]]; then
    return 1
  fi
  # Subshell so ID/ID_LIKE from os-release do not leak into the caller.
  (
    # shellcheck disable=SC1091
    . /etc/os-release
    [[ "${ID:-}" == "arch" || " ${ID_LIKE:-} " == *" arch "* ]]
  )
}
