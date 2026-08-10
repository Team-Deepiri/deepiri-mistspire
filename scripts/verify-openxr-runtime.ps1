# Mistspire — OpenXR / SteamVR sanity check on Windows.
#Requires -Version 5.1
$ErrorActionPreference = "Continue"

Write-Host "==> Mistspire OpenXR runtime check (Windows)"
$ok = 0

Write-Host "==> Steam / SteamVR"
$steam = "${env:ProgramFiles(x86)}\Steam\steam.exe"
$steamvr = "${env:ProgramFiles(x86)}\Steam\steamapps\common\SteamVR"
if (Test-Path $steam) {
  Write-Host "[ok] Steam: $steam"
} else {
  Write-Host "[--] Steam not installed — required for SteamVR OpenXR (primary runtime)"
}

if (Test-Path $steamvr) {
  Write-Host "[ok] SteamVR directory present"
} else {
  Write-Host "[--] SteamVR not installed — install from Steam library"
}

Write-Host "==> Active OpenXR runtime (registry)"
$regPath = "HKLM:\SOFTWARE\Khronos\OpenXR\1"
if (Test-Path $regPath) {
  $active = (Get-ItemProperty -Path $regPath -Name ActiveRuntime -ErrorAction SilentlyContinue).ActiveRuntime
  if ($active) {
    Write-Host "[ok] ActiveRuntime = $active"
    if ($active -match "steamvr|SteamVR") {
      Write-Host "     SteamVR OpenXR (Mistspire default)"
    } elseif ($active -match "oculus|meta|Oculus") {
      Write-Host "     Meta/Oculus runtime — OK for Quest Link; use SteamVR for Steam Link"
    } else {
      Write-Host "     Verify this matches your headset guide"
    }
  } else {
    Write-Host "[--] ActiveRuntime not set — set in SteamVR or Meta Horizon Link"
    $ok = 1
  }
} else {
  Write-Host "[--] OpenXR registry key missing — install a PCVR runtime (SteamVR or Meta Link)"
  $ok = 1
}

Write-Host ""
Write-Host "==> Manual checks"
Write-Host "    SteamVR: menu -> Settings -> OpenXR -> Set SteamVR as OpenXR Runtime"
Write-Host "    Meta Quest Link: Settings -> General -> Set Meta Horizon Link as active"
Write-Host "    Headset guides: docs\setup\headsets\README.md"
Write-Host ""

if ($ok -ne 0) { exit 1 }
Write-Host "==> Runtime check finished"
exit 0
