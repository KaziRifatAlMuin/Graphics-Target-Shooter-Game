param([string]$Root=(Split-Path $PSScriptRoot -Parent))
# Compatibility entry point for existing automation.
# Forward the project root to the maintained report image/table preparation script.
& (Join-Path $Root 'report/prepare.ps1') -Root $Root
