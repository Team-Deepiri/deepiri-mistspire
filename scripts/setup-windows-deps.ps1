# Mistspire — Windows development dependency checks (verify, do not silently install).
#Requires -Version 5.1
$ErrorActionPreference = "Continue"

$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Write-Host "==> Mistspire Windows dev checks (root: $Root)"
Write-Host ""

$ok = 0

function Test-CommandExists($name) {
  $null -ne (Get-Command $name -ErrorAction SilentlyContinue)
}

Write-Host "==> Git"
if (Test-CommandExists git) {
  Write-Host "[ok] git: $(git --version)"
} else {
  Write-Host "[!!] git not found — install Git for Windows"
  $ok = 1
}

Write-Host "==> Git LFS"
if (Test-CommandExists git-lfs) {
  Write-Host "[ok] git-lfs present"
  git lfs install 2>$null
} elseif (Test-CommandExists git) {
  $lfsVer = git lfs version 2>$null
  if ($lfsVer) { Write-Host "[ok] $lfsVer" } else {
    Write-Host "[!!] git-lfs not found — install Git LFS"
    $ok = 1
  }
}

Write-Host "==> Visual Studio 2022 (C++ toolchain)"
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vswhere) {
  $vsPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2>$null
  if ($vsPath) {
    Write-Host "[ok] VS with C++ tools: $vsPath"
  } else {
    Write-Host "[!!] Visual Studio found but C++ workload may be missing"
    Write-Host "     Install: Desktop development with C++ + Windows SDK"
    $ok = 1
  }
} else {
  Write-Host "[--] vswhere not found — install Visual Studio 2022 with C++ workload manually"
  Write-Host "     Required for Win64 compile from game/Mistspire.uproject"
}

Write-Host "==> Unreal Engine 5.8"
$ueCandidates = @(
  "$env:ProgramFiles\Epic Games\UE_5.8",
  "C:\UE_5.8",
  "$env:LOCALAPPDATA\Programs\Epic Games\UE_5.8"
)
if ($env:UE_ROOT) { $ueCandidates = @($env:UE_ROOT) + $ueCandidates }
$foundUe = $false
foreach ($c in $ueCandidates) {
  $editor = Join-Path $c "Engine\Binaries\Win64\UnrealEditor.exe"
  if (Test-Path $editor) {
    Write-Host "[ok] UnrealEditor: $editor"
    $foundUe = $true
    break
  }
}
if (-not $foundUe) {
  Write-Host "[!!] UE 5.8 not found — install via Epic Launcher or set UE_ROOT"
  $ok = 1
}

Write-Host "==> Steam / SteamVR (PCVR)"
$steam = "${env:ProgramFiles(x86)}\Steam\steam.exe"
if (Test-Path $steam) {
  Write-Host "[ok] Steam installed"
} else {
  Write-Host "[--] Steam not found — needed for SteamVR / Steam Link / Oasis"
}

Write-Host ""
Write-Host "==> Next steps"
Write-Host "    Headset: $Root\docs\setup\headsets\README.md"
Write-Host "    OpenXR:  powershell -File $Root\scripts\verify-openxr-runtime.ps1"
Write-Host "    Bootstrap: $Root\docs\setup\DEV_BOOTSTRAP.md"
Write-Host ""

if ($ok -ne 0) { exit 1 }
exit 0
