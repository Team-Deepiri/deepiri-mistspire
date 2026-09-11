# Launch packaged Win64 Mistspire in non-VR mode.
param(
    [string]$PackageRoot = "",
    [switch]$ForceWindowed
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
if (-not $PackageRoot) {
    $PackageRoot = Join-Path $Root "game\Package\Win64"
}

$ExeCandidates = @(
    (Join-Path $PackageRoot "Mistspire\Binaries\Win64\Mistspire.exe"),
    (Join-Path $PackageRoot "Windows\Mistspire\Binaries\Win64\Mistspire.exe"),
    (Join-Path $PackageRoot "Mistspire.exe")
)

$Exe = $null
foreach ($cand in $ExeCandidates) {
    if (Test-Path -LiteralPath $cand) {
        $Exe = $cand
        break
    }
}

if (-not $Exe) {
    Write-Host "!! Packaged executable not found under $PackageRoot"
    Write-Host "   Cook/package first — see docs/setup/NONVR_MODE.md"
    exit 1
}

$LaunchArgs = @("-nonvr")
if ($ForceWindowed) {
    $LaunchArgs += "-windowed"
}

Write-Host "Launching: $Exe $($LaunchArgs -join ' ')"
& $Exe @LaunchArgs
