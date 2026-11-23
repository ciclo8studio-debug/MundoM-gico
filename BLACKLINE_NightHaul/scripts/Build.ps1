<#
.SYNOPSIS
  Build de BLACKLINE: NIGHT HAUL (UE 5.8.4, Windows) via UnrealBuildTool / BuildCookRun.
.EXAMPLE
  .\scripts\Build.ps1 -Config Development
  .\scripts\Build.ps1 -Config Shipping -Version 0.1.0
  .\scripts\Build.ps1 -Config Development -CompileOnly
#>
[CmdletBinding()]
param(
    [ValidateSet('Development','Shipping')][string]$Config = 'Development',
    [string]$EngineRoot = $(if ($env:UE_ROOT) { $env:UE_ROOT } else { 'C:\Program Files\Epic Games\UE_5.8' }),
    [string]$Version = '0.0.0',
    [switch]$CompileOnly,   # solo compila (UBT), sin cook/stage/pak
    [switch]$Clean          # borra Intermediate/Binaries antes de compilar
)
$ErrorActionPreference = 'Stop'

$ProjectDir = Split-Path -Parent $PSScriptRoot
$Uproject   = Join-Path $ProjectDir 'Blackline.uproject'
$OutDir     = Join-Path $ProjectDir "Build\v$Version-$Config"
$LogDir     = Join-Path $ProjectDir 'Saved\BuildLogs'
$RunUAT     = Join-Path $EngineRoot 'Engine\Build\BatchFiles\RunUAT.bat'
$Build      = Join-Path $EngineRoot 'Engine\Build\BatchFiles\Build.bat'

if (-not (Test-Path $Uproject)) { throw "No existe $Uproject" }
if (-not (Test-Path $RunUAT))   { throw "UE no encontrado en $EngineRoot (define UE_ROOT o -EngineRoot)" }

# Verifica version exacta del motor (5.8.4)
$verFile = Join-Path $EngineRoot 'Engine\Build\Build.version'
if (Test-Path $verFile) {
    $v = Get-Content $verFile -Raw | ConvertFrom-Json
    $found = "$($v.MajorVersion).$($v.MinorVersion).$($v.PatchVersion)"
    if ($found -ne '5.8.4') { Write-Warning "Motor $found, se esperaba 5.8.4" }
}

New-Item -ItemType Directory -Force -Path $LogDir | Out-Null
$Log = Join-Path $LogDir ("build_{0}_{1:yyyyMMdd_HHmmss}.log" -f $Config, (Get-Date))

if ($Clean) {
    foreach ($d in 'Intermediate','Binaries') {
        $p = Join-Path $ProjectDir $d
        if (Test-Path $p) { Remove-Item $p -Recurse -Force }
    }
}

function Invoke-Step($exe, [string[]]$argList) {
    Write-Host ">> $exe $($argList -join ' ')"
    & $exe @argList 2>&1 | Tee-Object -FilePath $Log -Append
    if ($LASTEXITCODE -ne 0) { throw "Fallo (codigo $LASTEXITCODE). Log: $Log" }
}

if ($CompileOnly) {
    # Target de juego (Blackline) para Win64
    Invoke-Step $Build @('Blackline','Win64',$Config,"-Project=$Uproject",'-WaitMutex','-FromMsBuild','-NoHotReload')
    Write-Host "OK compilacion $Config. Log: $Log"
    return
}

$uat = @(
    'BuildCookRun',
    "-project=$Uproject",
    '-platform=Win64',
    "-clientconfig=$Config",
    '-build','-cook','-stage','-pak','-archive','-compressed',
    "-archivedirectory=$OutDir",
    '-utf8output','-unattended','-nop4','-nosplash','-noP4',
    '-prereqs','-WaitForUATMutex'
)
if ($Config -eq 'Shipping') { $uat += '-nodebuginfo' }   # sin consola ni stat en Shipping

Invoke-Step $RunUAT $uat
Write-Host "OK $Config v$Version -> $OutDir"
Write-Host "Log: $Log"
