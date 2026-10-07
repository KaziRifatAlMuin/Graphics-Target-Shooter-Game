param([string]$PdfLatex='')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
& (Join-Path $PSScriptRoot 'prepare_report.ps1') -Root $root
if(-not $PdfLatex) {
    $command=Get-Command pdflatex -ErrorAction SilentlyContinue
    if($command) { $PdfLatex=$command.Source }
    else {
        $candidate=Join-Path $env:LOCALAPPDATA 'Programs/MiKTeX/miktex/bin/x64/pdflatex.exe'
        if(Test-Path -LiteralPath $candidate) { $PdfLatex=$candidate }
        else { throw 'Install MiKTeX or TeX Live, or pass -PdfLatex with the full path to pdflatex.' }
    }
}
$buildDirectory=Join-Path $root '.report-build/pdf'
New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
Push-Location (Join-Path $root 'docs')
try {
    for($pass=1;$pass -le 2;$pass++) {
        & $PdfLatex -jobname=report-built "-output-directory=$buildDirectory" -interaction=nonstopmode -halt-on-error -file-line-error report.tex
        if($LASTEXITCODE -ne 0) { throw "pdfLaTeX pass $pass failed; inspect .report-build/pdf/report-built.log." }
    }
    $log=[IO.File]::ReadAllText((Join-Path $buildDirectory 'report-built.log'))
    $match=[regex]::Match($log,'(?s)Output written on .*?report-built\.pdf"?\s+\((\d+) pages?')
    if(-not $match.Success -or [int]$match.Groups[1].Value -ne 20) { throw 'The report must contain exactly 20 physical pages, including the cover and references.' }
    if($log -match 'undefined references|Citation .+ undefined|Reference .+ undefined|Overfull \\[hv]box') {
        throw 'Unresolved references or overflowing content found; inspect .report-build/pdf/report-built.log.'
    }
    try { Copy-Item -LiteralPath (Join-Path $buildDirectory 'report-built.pdf') -Destination (Join-Path $root 'docs/report-final.pdf') -Force }
    catch { throw 'The validated PDF is at .report-build/pdf/report-built.pdf. Close any viewer locking docs/report-final.pdf, then run this command again to replace it.' }
    try { Copy-Item -LiteralPath (Join-Path $buildDirectory 'report-built.pdf') -Destination (Join-Path $root 'docs/report.pdf') -Force }
    catch { Write-Warning 'docs/report.pdf is open in another application. The completed report is available as docs/report-final.pdf; rerun after closing the viewer to refresh both copies.' }
    Write-Output 'Built docs/report-final.pdf: exactly 20 pages, with resolved references and no overfull boxes.'
} finally { Pop-Location }
