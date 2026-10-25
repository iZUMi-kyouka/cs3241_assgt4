[CmdletBinding()]
param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug',
    [ValidateSet('x64', 'x86', 'Win32')]
    [string]$Platform = 'x64',
    [switch]$Strict
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path $PSScriptRoot -Parent
$solutionPlatform = if ($Platform -eq 'Win32') { 'x86' } else { $Platform }
$projectPlatform = if ($solutionPlatform -eq 'x86') { 'Win32' } else { 'x64' }
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path $vswhere)) {
    throw 'Install Visual Studio 2022 with Desktop development with C++, v143, and a Windows 10/11 SDK.'
}
$installation = & $vswhere -latest -version '[17.0,18.0)' -products '*' `
    -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $installation) {
    throw 'Visual Studio 2022 with the v143 x86/x64 C++ tools was not found.'
}
$msbuild = Join-Path $installation 'MSBuild\Current\Bin\amd64\MSBuild.exe'
$logDirectory = Join-Path $repoRoot "build\$projectPlatform\$Configuration"
New-Item -ItemType Directory -Path $logDirectory -Force | Out-Null
$logPath = Join-Path $logDirectory 'msbuild.log'
$binlogPath = Join-Path $logDirectory 'msbuild.binlog'

Push-Location $repoRoot
try {
    # Rebuild so a strict check cannot skip diagnostics hidden by incremental builds.
    # No compiler, linker, SDK, language-standard, or toolset overrides are passed.
    & $msbuild 'visual_studio/CS3241 Assignment 4 Bezier.sln' /nologo /m /t:Rebuild `
        "/p:Configuration=$Configuration" "/p:Platform=$solutionPlatform" `
        /verbosity:normal /fl "/flp:LogFile=$logPath;Verbosity=normal;Encoding=UTF-8" `
        "/bl:$binlogPath"
    if ($LASTEXITCODE -ne 0) {
        throw "MSBuild failed with exit code $LASTEXITCODE. See $logPath"
    }
    if ($Strict) {
        # Enforce zero warnings without changing the professor's /W3 to /W4 or /WX.
        $diagnostics = Select-String -Path $logPath -Pattern '\bwarning\s+[A-Za-z]+\d+\s*:'
        if ($diagnostics) {
            $diagnostics | ForEach-Object { Write-Host $_.Line }
            throw "Build emitted warnings. See $logPath"
        }
        Write-Host 'Verified: MSBuild succeeded with zero compiler/linker/MSBuild warnings.'
    }
} finally {
    Pop-Location
}
