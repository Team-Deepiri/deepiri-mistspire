# Mistspire — launch editor or packaged Win64 build on Windows.
#Requires -Version 5.1
param(
  [ValidateSet("editor", "packaged")]
  [string]$Mode = "editor"
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $Root

Write-Host "============================================"
Write-Host "  Mistspire — Launch (Windows)"
Write-Host "============================================"

Write-Host ""
Write-Host "==> Checking OpenXR runtime..."
& powershell -NoProfile -File "$Root\scripts\verify-openxr-runtime.ps1" 2>$null

$UEEditor = $null
$candidates = @(
  "$env:ProgramFiles\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe",
  "C:\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
)
if ($env:UE_ROOT) {
  $candidates = @((Join-Path $env:UE_ROOT "Engine\Binaries\Win64\UnrealEditor.exe")) + $candidates
}
foreach ($c in $candidates) {
  if (Test-Path $c) { $UEEditor = $c; break }
}

$UProject = Join-Path $Root "game\Mistspire.uproject"

if ($Mode -eq "editor") {
  if (-not $UEEditor) {
    Write-Host "!! Unreal Engine 5.8 not found. Set UE_ROOT or install via Epic Launcher."
    exit 1
  }
  Write-Host ""
  Write-Host "==> Launching UE Editor..."
  Write-Host "   Once loaded: Play -> VR Preview"
  & $UEEditor $UProject
} elseif ($Mode -eq "packaged") {
  $packageCandidates = @(
    (Join-Path $Root "game\Package\Win64\Mistspire.exe"),
    (Join-Path $Root "game\Package\Win64\Mistspire\Binaries\Win64\Mistspire.exe"),
    (Join-Path $Root "game\Package\Win64\Windows\Mistspire\Binaries\Win64\Mistspire.exe")
  )
  $exe = $null
  foreach ($p in $packageCandidates) {
    if (Test-Path $p) { $exe = $p; break }
  }
  if ($exe) {
    Write-Host ""
    Write-Host "==> Launching packaged build: $exe"
    & $exe
  } else {
    Write-Host "!! No Win64 package found under game\Package\Win64"
    Write-Host "   Package first — see scripts\package_win64.sh and tools\pipeline\step_17_README.md"
    exit 1
  }
}
