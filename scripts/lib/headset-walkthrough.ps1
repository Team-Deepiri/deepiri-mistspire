# Mistspire - interactive VR headset walkthrough (Windows).
# Dot-source from setup.ps1. Expects $Root to be set to the repo root.
#
# Invoke-MistspireHeadsetWalkthrough [-NonInteractive]
# Returns $true if user intends VR; $false otherwise.
# Also sets $env:MISTSPIRE_VR_MODE to "1" or "0".

function Read-MistspireYesNo {
  param([string]$Prompt)
  while ($true) {
    $ans = Read-Host "$Prompt [y/n]"
    if ($null -eq $ans) { return $false }
    switch -Regex ($ans.Trim().ToLowerInvariant()) {
      '^(y|yes)$' { return $true }
      '^(n|no)$' { return $false }
      default { Write-Host "   Please answer y or n." }
    }
  }
}

function Test-MistspireSteamPresent {
  $candidates = @(
    "${env:ProgramFiles(x86)}\Steam\steam.exe",
    "$env:ProgramFiles\Steam\steam.exe"
  )
  foreach ($c in $candidates) {
    if (Test-Path $c) { return $true }
  }
  return $false
}

function Install-MistspireSteamOptional {
  if (Test-MistspireSteamPresent) {
    Write-Host "   [ok] Steam already present."
    return
  }
  Write-Host ""
  Write-Host "   Steam was not found. Most PCVR paths need Steam + SteamVR OpenXR."
  if (-not (Read-MistspireYesNo "   Install Steam now (winget) or open the download page?")) {
    Write-Host "   Skipping Steam install - see your headset guide."
    return
  }
  $winget = Get-Command winget -ErrorAction SilentlyContinue
  if ($winget) {
    Write-Host "   Installing Steam via winget (Valve.Steam)..."
    & winget install -e --id Valve.Steam --accept-package-agreements --accept-source-agreements
    if (Test-MistspireSteamPresent) {
      Write-Host "   [ok] Steam install looks present."
      return
    }
    Write-Host "   [--] winget finished but Steam not detected yet - may need a reboot or manual launch."
  }
  Write-Host "   Opening Steam download page..."
  Start-Process "https://store.steampowered.com/about/"
  Write-Host "   Install Steam, then continue."
}

function Show-MistspireHeadsetGuide {
  param([string]$Key)
  $streaming = $false
  $label = ""
  $rel = ""
  switch ($Key) {
    "1" {
      $label = "Meta Quest / Oculus Rift"
      $rel = "docs\setup\headsets\meta_quest.md"
      $streaming = $true
    }
    "2" {
      $label = "Valve Index / HTC Vive (Lighthouse)"
      $rel = "docs\setup\headsets\steamvr_lighthouse.md"
    }
    "3" {
      $label = "HTC Vive Focus / XR Elite"
      $rel = "docs\setup\headsets\htc_vive_focus.md"
      $streaming = $true
    }
    "4" {
      $label = "Windows MR / HP Reverb"
      $rel = "docs\setup\headsets\windows_mixed_reality.md"
    }
    "5" {
      $label = "Pico"
      $rel = "docs\setup\headsets\pico.md"
      $streaming = $true
    }
    "6" {
      $label = "Steam Frame"
      $rel = "docs\setup\headsets\steam_frame.md"
    }
    "7" {
      $label = "Other / unsure"
      $rel = "docs\setup\headsets\generic.md"
      $streaming = $true
    }
    default { return $false }
  }
  Write-Host ""
  Write-Host "   Headset: $label"
  Write-Host "   Guide:   $Root\$rel"
  Write-Host "   OpenXR:  $Root\docs\setup\OPENXR_DEV_COMMON.md"
  if ($streaming) {
    Write-Host "   Stream:  $Root\docs\setup\headsets\streaming_options.md"
  }
  Write-Host "   Index:   $Root\docs\setup\headsets\README.md"
  Write-Host ""
  Write-Host "   Follow the guide, then set SteamVR (or the path-specific runtime) as the"
  Write-Host "   active OpenXR runtime before VR Preview."
  return $true
}

function Select-MistspireHeadset {
  Write-Host ""
  Write-Host "   Which headset do you have?"
  Write-Host "     1) Meta Quest / Oculus Rift"
  Write-Host "     2) Valve Index / HTC Vive (Lighthouse)"
  Write-Host "     3) HTC Vive Focus / XR Elite"
  Write-Host "     4) Windows MR / HP Reverb"
  Write-Host "     5) Pico"
  Write-Host "     6) Steam Frame"
  Write-Host "     7) Other / unsure"
  while ($true) {
    $choice = Read-Host "   Choice [1-7]"
    if ($choice -match '^[1-7]$') {
      [void](Show-MistspireHeadsetGuide -Key $choice)
      return
    }
    Write-Host "   Enter a number from 1 to 7."
  }
}

function Invoke-MistspireHeadsetWalkthrough {
  param([switch]$NonInteractive)

  Write-Host ""
  Write-Host "==> Headset / VR walkthrough"

  $isTty = $true
  try {
    if ([Console]::IsInputRedirected) { $isTty = $false }
  } catch {
    $isTty = $true
  }

  if ($NonInteractive -or -not $isTty) {
    Write-Host "   Non-interactive (or no TTY) - skipping headset prompts."
    Write-Host "   Guides: $Root\docs\setup\headsets\README.md"
    $env:MISTSPIRE_VR_MODE = "0"
    return $false
  }

  if (-not (Read-MistspireYesNo "   Do you have a VR headset for Mistspire PCVR?")) {
    Write-Host "   No headset - skipping VR walkthrough (non-VR / desktop path later)."
    Write-Host "   You can still open the editor; VR Preview is optional for now."
    $env:MISTSPIRE_VR_MODE = "0"
    return $false
  }

  $env:MISTSPIRE_VR_MODE = "1"

  if (Read-MistspireYesNo "   Is your headset already connected and ready on this PC?") {
    Write-Host "   Great - continuing to dependency install. OpenXR check runs later."
    return $true
  }

  Write-Host ""
  Write-Host "   We will point you at the right headset guide before installing deps."
  Select-MistspireHeadset
  Install-MistspireSteamOptional

  Write-Host ""
  Read-Host "   Press Enter when you have completed the headset guide steps (or want to continue anyway)"
  Write-Host "   Continuing to dependency install..."
  return $true
}
