param([string]$PdfLatex='')
# Compatibility entry point; report/build.bat is the documented command.
& (Join-Path (Split-Path $PSScriptRoot -Parent) 'report/build.ps1') -PdfLatex $PdfLatex
