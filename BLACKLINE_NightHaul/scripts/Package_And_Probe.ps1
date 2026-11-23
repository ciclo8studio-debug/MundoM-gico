<#
.SYNOPSIS
  Empaqueta (Development), abre la build, manda W por el sistema y muestra la telemetria del furgon del log.
  Requiere todos los editores cerrados. Ej: .\scripts\Package_And_Probe.ps1 -Version 0.1.5
#>
param([Parameter(Mandatory)][string]$Version, [int]$HoldSeconds = 12)
$ErrorActionPreference = 'Stop'
$proj = Split-Path -Parent $PSScriptRoot
$busy = Get-Process UnrealEditor*, Blackline, LiveCodingConsole -ErrorAction SilentlyContinue
if ($busy) { throw "Cierra antes: $(($busy | % ProcessName) -join ', ')" }

& "$proj\scripts\Build.ps1" -Config Development -Version $Version
$exe = Join-Path $proj "Build\v$Version-Development\Windows\Blackline.exe"
if (-not (Test-Path $exe)) { throw "No existe $exe" }
if (Get-Process UnrealEditor* -ErrorAction SilentlyContinue) { throw "Se abrio un editor: no lanzo la prueba" }

Start-Process $exe -ArgumentList '-dx11','-windowed','-ResX=1280','-ResY=720','-log'
Start-Sleep 45
Add-Type @"
using System; using System.Runtime.InteropServices;
public class KP { [DllImport("user32.dll")] public static extern void keybd_event(byte b, byte s, uint f, UIntPtr e);
 [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h); }
"@
$p = Get-Process Blackline | ? { $_.MainWindowHandle -ne 0 } | Select -First 1
[KP]::SetForegroundWindow($p.MainWindowHandle) | Out-Null; Start-Sleep 2
[KP]::keybd_event(0x57,0,0,[UIntPtr]::Zero); Start-Sleep $HoldSeconds; [KP]::keybd_event(0x57,0,2,[UIntPtr]::Zero); Start-Sleep 3
$log = Join-Path $proj "Build\v$Version-Development\Windows\Blackline\Saved\Logs\Blackline.log"
Select-String -Path $log -Pattern 'Telemetria' | Select -Last 16 | % { $_.Line.Substring($_.Line.IndexOf('Telemetria')) }
