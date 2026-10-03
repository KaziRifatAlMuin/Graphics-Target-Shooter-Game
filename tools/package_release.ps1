param([string]$Executable='main.exe')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$destination=Join-Path $root 'build/release'
New-Item -ItemType Directory -Force $destination | Out-Null
Copy-Item -LiteralPath (Join-Path $root $Executable) -Destination (Join-Path $destination 'TargetShooter.exe') -Force
Copy-Item -LiteralPath (Join-Path $root 'shaders') -Destination $destination -Recurse -Force
Copy-Item -LiteralPath (Join-Path $root 'assets') -Destination $destination -Recurse -Force
Write-Output "Portable release: $destination/TargetShooter.exe"
