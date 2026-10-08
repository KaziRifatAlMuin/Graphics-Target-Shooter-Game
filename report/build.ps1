param([string]$PdfLatex='')
$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
& (Join-Path $PSScriptRoot 'prepare.ps1') -Root $root
# Find pdfLaTeX on PATH or in the usual per-user MiKTeX installation.
if(-not $PdfLatex) {
    $command=Get-Command pdflatex -ErrorAction SilentlyContinue
    if($command) { $PdfLatex=$command.Source }
    else {
        $candidate=Join-Path $env:LOCALAPPDATA 'Programs/MiKTeX/miktex/bin/x64/pdflatex.exe'
        if(Test-Path -LiteralPath $candidate) { $PdfLatex=$candidate }
        else { throw 'Install MiKTeX or TeX Live, or pass -PdfLatex with the full path to pdflatex.' }
    }
}
$version=& $PdfLatex --version
if($LASTEXITCODE -ne 0) { throw 'pdfLaTeX is installed but could not start.' }
$installerArgs=@()
if(($version -join ' ') -match 'MiKTeX') { $installerArgs=@('--enable-installer') }
$buildDirectory=Join-Path $root '.report-build/pdf'
New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null
Push-Location $PSScriptRoot
try {
    # Compile twice so references and page numbers resolve using the first pass's auxiliary files.
    for($pass=1;$pass -le 2;$pass++) {
        & $PdfLatex @installerArgs -jobname=report-built "-output-directory=$buildDirectory" -interaction=nonstopmode -halt-on-error -file-line-error report.tex
        if($LASTEXITCODE -ne 0) { throw "pdfLaTeX pass $pass failed; inspect .report-build/pdf/report-built.log." }
    }
    $log=[IO.File]::ReadAllText((Join-Path $buildDirectory 'report-built.log'))
    # Extract the physical page count from the log and enforce the report's 20-page requirement.
    $match=[regex]::Match($log,'(?s)Output written on .*?report-built\.pdf"?\s+\((\d+) pages?')
    if(-not $match.Success -or [int]$match.Groups[1].Value -ne 20) { throw 'The report must contain exactly 20 physical pages, including the cover and references.' }
    if($log -match 'undefined references|Citation .+ undefined|Reference .+ undefined|Overfull \\[hv]box') {
        throw 'Unresolved references or overflowing content found; inspect .report-build/pdf/report-built.log.'
    }
    $outputPdf=Join-Path $root 'report/report.pdf'
    try { Copy-Item -LiteralPath (Join-Path $buildDirectory 'report-built.pdf') -Destination $outputPdf -Force }
    catch { throw 'Close the viewer locking report/report.pdf and rerun. The validated PDF remains at .report-build/pdf/report-built.pdf.' }
    # Refresh alternate PDF filenames from the same validated report output.
    foreach($alias in @('report-revised.pdf','report-final.pdf','report-formatted.pdf','report-presentation.pdf')) {
        try { Copy-Item -LiteralPath (Join-Path $buildDirectory 'report-built.pdf') -Destination (Join-Path $root ('report/'+$alias)) -Force }
        catch { Write-Warning "report/$alias could not be replaced: $($_.Exception.Message) Use report/report.pdf; rerun after closing the old viewer to refresh aliases." }
    }
    Write-Output 'Built report/report.pdf: exactly 20 pages, with resolved references and no overfull boxes.'
} finally { Pop-Location }
