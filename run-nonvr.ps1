# Mistspire — non-VR launch (keyboard/mouse)
param()

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $Root

Write-Host "============================================"
Write-Host "  Mistspire — Non-VR Mode"
Write-Host "============================================"
Write-Host ""
Write-Host "Once the editor loads, click Play (NOT VR Preview)."
Write-Host "Controls: docs/setup/NONVR_MODE.md"
Write-Host ""

$UEEditor = $null
$Candidates = @(
    "$env:UE_ROOT\Engine\Binaries\Win64\UnrealEditor.exe",
    "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe",
    "C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe",
    "$env:USERPROFILE\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"
)

foreach ($cand in $Candidates) {
    if ($cand -and (Test-Path -LiteralPath $cand)) {
        $UEEditor = $cand
        break
    }
}

if (-not $UEEditor) {
    Write-Host "!! Unreal Editor not found. Set UE_ROOT or open game\Mistspire.uproject manually."
    exit 1
}

$UProject = Join-Path $Root "game\Mistspire.uproject"
& $UEEditor $UProject -nonvr
