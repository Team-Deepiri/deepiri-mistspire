# Mistspire — shared Unreal Engine 5.8 path discovery (Linux).
# Source from setup.sh / run.sh: source "$(dirname "$0")/scripts/lib/ue-paths.sh"

mistspire_find_ue_linux() {
  local root="${1:-}"
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

  if [[ -n "${UE_DOXY_ENGINE_ROOT:-}" && -x "$UE_DOXY_ENGINE_ROOT/Engine/Binaries/Linux/UnrealEditor" ]]; then
    UE_EDITOR="$UE_DOXY_ENGINE_ROOT/Engine/Binaries/Linux/UnrealEditor"
    UE_ROOT="$UE_DOXY_ENGINE_ROOT"
    return 0
  fi

  if [[ -n "${UE_ROOT:-}" && -x "$UE_ROOT/Engine/Binaries/Linux/UnrealEditor" ]]; then
    UE_EDITOR="$UE_ROOT/Engine/Binaries/Linux/UnrealEditor"
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
