$PSDefaultParameterValues['Out-File:Encoding'] = 'utf8'
& "$env:windir\system32\chcp.com" 65001

function Copy-AddonFiles {
    param(
        [string]$AddonName,
        [string]$TargetDirectory,
        [string]$Architecture,
        [bool]$Debug
    )
    if (-not $PSBoundParameters.ContainsKey('Architecture')) {
        $Architecture = (Get-Variable -Scope 1 -Name 'Architecture' -ErrorAction SilentlyContinue).Value
        if (-not $Architecture) { $Architecture = 'x64' }
    }
    if (-not $PSBoundParameters.ContainsKey('Debug')) {
        $d = (Get-Variable -Scope 1 -Name 'Debug' -ErrorAction SilentlyContinue).Value
        $Debug = if ($null -eq $d) { $false } else { [bool]$d }
    }

    $buildDir = if ($Architecture -eq "x86") { "build32" } else { "build" }
    $configDir = if ($Debug) { "Debug" } else { "RelWithDebInfo" }
    $sourceDir = "$buildDir\$configDir"
    $suffix = if ($Architecture -eq "x86") { ".addon32" } else { ".addon64" }

    Write-Host "Copying files from: $sourceDir" -ForegroundColor Cyan

    $devkitFile = "renodx-devkit$suffix"
    if (Test-Path "$sourceDir\$devkitFile") {
        Copy-Item "$sourceDir\$devkitFile" "$TargetDirectory" -Force
        Write-Host "  Copied: $devkitFile" -ForegroundColor Green
    } else {
        Write-Host "  Warning: $devkitFile not found in $sourceDir" -ForegroundColor Yellow
    }

    $addonFile = "renodx-$AddonName$suffix"
    if (Test-Path "$sourceDir\$addonFile") {
        Copy-Item "$sourceDir\$addonFile" "$TargetDirectory" -Force
        Write-Host "  Copied: $addonFile" -ForegroundColor Green
    } else {
        Write-Host "  Warning: $addonFile not found in $sourceDir" -ForegroundColor Yellow
    }
}
