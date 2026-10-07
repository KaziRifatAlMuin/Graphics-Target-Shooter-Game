param([string]$Root=(Split-Path $PSScriptRoot -Parent))
# Compatibility entry point for existing automation.
& (Join-Path $Root 'report/prepare.ps1') -Root $Root
