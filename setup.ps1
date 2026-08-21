# Mistspire - one-command setup on Windows: headset walkthrough, hooks, LFS, checks, launch UE.
#Requires -Version 5.1
param(
  [switch]$NonInteractive,
  [Alias("y")]
  [switch]$Yes
)
$ErrorActionPreference = "Continue"

if ($Yes) { $NonInteractive = $true }

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $Root

. "$Root\scripts\lib\headset-walkthrough.ps1"

Write-Host "============================================"
Write-Host "  Mistspire - Full Setup (Windows)"
Write-Host "============================================"

# -- 0. Headset / VR walkthrough --
$VrMode = Invoke-MistspireHeadsetWalkthrough -NonInteractive:$NonInteractive

Write-Host ""
Write-Host "==> [1/6] Configuring git hooks..."
if (Test-Path ".git-hooks") {
  git config core.hooksPath .git-hooks
  Write-Host "   Git hooks configured (core.hooksPath = .git-hooks)"
} else {
  Write-Host "   No .git-hooks directory found, skipping"
}

Write-Host ""
Write-Host "==> [2/6] Windows toolchain check..."
& powershell -NoProfile -File "$Root\scripts\setup-windows-deps.ps1"
if ($LASTEXITCODE -ne 0) {
  Write-Host "   Warning: some Windows checks failed (non-fatal for opening editor)"
}

Write-Host ""
Write-Host "==> [3/6] Pulling Git LFS assets..."
if (Get-Command git-lfs -ErrorAction SilentlyContinue) {
  git lfs pull 2>$null
  if ($LASTEXITCODE -eq 0) { Write-Host "   LFS pulled." } else { Write-Host "   LFS pull skipped or empty." }
} else {
  Write-Host "   git-lfs not installed - skip LFS pull"
}

Write-Host ""
Write-Host "==> [4/6] Locating Unreal Engine 5.8..."
$UEEditor = $null
$candidates = @(
  "$env:ProgramFiles\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe",
  "C:\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe",
  "$env:LOCALAPPDATA\Programs\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
)
if ($env:UE_ROOT) {
  $candidates = @(
    (Join-Path $env:UE_ROOT "Engine\Binaries\Win64\UnrealEditor.exe")
  ) + $candidates
}
foreach ($c in $candidates) {
  if (Test-Path $c) {
    $UEEditor = $c
    break
  }
}

$SkipLaunch = $true
if ($UEEditor) {
  Write-Host "   Found: $UEEditor"
  $SkipLaunch = $false
} else {
  Write-Host "!! Could not find Unreal Engine 5.8."
  Write-Host "   Set UE_ROOT or install via Epic Launcher."
  Write-Host "   Open manually: game\Mistspire.uproject"
}

Write-Host ""
Write-Host "==> [5/6] Verifying OpenXR runtime..."
if (-not $VrMode) {
  Write-Host "   Skipped VR walkthrough - OpenXR check is informational only."
}
& powershell -NoProfile -File "$Root\scripts\verify-openxr-runtime.ps1" 2>$null
Write-Host "   (OpenXR check non-fatal)"

$UProject = Join-Path $Root "game\Mistspire.uproject"

Write-Host ""
Write-Host "==> [6/6] Launching Unreal Editor..."
if (-not $SkipLaunch) {
  Write-Host "   Opening Mistspire.uproject..."
  Write-Host "   First launch may compile Mistspire + MistspireOpenXRNative in the editor."
  # Quote project path - default Epic install is under "Program Files".
  Start-Process -FilePath $UEEditor -ArgumentList @("`"$UProject`"")
  if ($VrMode) {
    Write-Host "   Once loaded: Play -> VR Preview (see docs\setup\headsets\)"
  } else {
    Write-Host "   VR Preview optional - no headset walkthrough this run."
  }
} else {
  Write-Host "   Open manually: $UProject"
}

Write-Host ""
Write-Host "============================================"
Write-Host "  Setup complete."
Write-Host "============================================"
Write-Host ""
Write-Host "  .\run.ps1              Launch editor again"
Write-Host "  docs\setup\DEV_BOOTSTRAP.md   Full walkthrough"
if ($VrMode) {
  Write-Host "  mistspire.TeleportUp 5000     (in-game console)"
} else {
  Write-Host "  Headset guides (later): docs\setup\headsets\README.md"
}
Write-Host ""

exit 0
