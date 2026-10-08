param([string]$PdfLatex='')
# Compatibility entry point; report/build.bat is the documented command.
# Forward the selected pdfLaTeX path to the maintained report build script.
& (Join-Path (Split-Path $PSScriptRoot -Parent) 'report/build.ps1') -PdfLatex $PdfLatex
