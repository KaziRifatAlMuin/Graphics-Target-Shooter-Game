param([string]$Root=(Split-Path $PSScriptRoot -Parent))
$ErrorActionPreference='Stop'
$utf8=New-Object System.Text.UTF8Encoding($false)
$data=Join-Path $Root 'report/report-data'
New-Item -ItemType Directory -Force -Path $data | Out-Null
# Write generated text as UTF-8 without a byte-order mark.
function Write-Utf8($Path,$Text) { [IO.File]::WriteAllText($Path,$Text,$utf8) }
# Generate a LaTeX image grid with the requested columns and numbered assembly steps.
function Assembly($Name,$Count,$Columns) {
    $width=if($Columns -eq 8) { '.112' } elseif($Columns -eq 6) { '.155' } elseif($Columns -eq 4) { '.237' } else { '.315' }
    $text="\centering\begin{tabular}{*{$Columns}{p{$width\linewidth}}}`n"
    for($i=1;$i -le $Count;$i++) {
        $text+='\pic{objects/'+$Name+'/assembly-'+$i+'.png}{'+$i+'}'
        if($i % $Columns -eq 0) { $text+="\\[3pt]`n" } else { $text+=' & ' }
    }
    if($Count % $Columns -ne 0) {
        for($i=$Count % $Columns;$i -lt $Columns-1;$i++) { $text+=' & ' }
        $text+="\\`n"
    }
    $text+="\end{tabular}`n"
    Write-Utf8 (Join-Path $data ($Name+'-assembly.tex')) $text
}
Assembly 'weapon-pistol' 11 6
Assembly 'weapon-shotgun' 14 8
Assembly 'weapon-rifle' 15 8
Assembly 'target' 38 8
Assembly 'cargo-crate' 4 4
Assembly 'human' 21 8
Assembly 'bird' 10 8

# Selected numeric tables are extracted from the generated guides, not retyped.
foreach($name in @('weapon-pistol','weapon-shotgun','weapon-rifle','cargo-crate')) {
    $guide=[IO.File]::ReadAllText((Join-Path $Root "objects/$name/$name.md"))
    $guide=($guide -split '## Alternate state:')[0]
    $pattern='(?s)### ([^\r\n]+) \u2014 `([^`]+)`\s+.*?Position T = (\([^\r\n]+?\)) m; scale S = (\([^\r\n]+?\)); rotation \(Rx,Ry,Rz\) = (\([^\r\n]+?\)) degrees\.'
    # Extract component names and transform tuples from generated guides instead of duplicating their numbers.
    $rows=[regex]::Matches($guide,$pattern)
    if($rows.Count -eq 0) { throw "No numeric rows found for $name" }
    $text="\begin{center}\normalsize\begin{tabular}{rllll}\toprule`nStep & Component & World center (m) & Scale (m) & Rotation (deg) \\\midrule`n"
    $i=0
    foreach($row in $rows) {
        $i++
        $cells=@()
        foreach($j in 3..5) {
            $values=$row.Groups[$j].Value.Trim('(',')').Split(',')
            $rounded=@($values | ForEach-Object { ([double]::Parse($_,[Globalization.CultureInfo]::InvariantCulture)).ToString('0.####',[Globalization.CultureInfo]::InvariantCulture) })
            $cells+='('+($rounded -join ',')+')'
        }
        $component=$row.Groups[1].Value.Replace('_','\_')
        $text+="$i & $component & $($cells[0]) & $($cells[1]) & $($cells[2]) \\`n"
    }
    $text+="\bottomrule\end{tabular}\end{center}`n"
    Write-Utf8 (Join-Path $data ($name+'-values.tex')) $text
}

$body=Join-Path $Root 'report/report-sections.tex'
$files=@($body)
foreach($inputMatch in [regex]::Matches([IO.File]::ReadAllText($body),'\\input\{([^}]+)\}')) {
    $files+=Join-Path $Root ('report/'+$inputMatch.Groups[1].Value)
}
# Deduplicate referenced picture paths and sort them for a reproducible image manifest.
$selected=New-Object 'System.Collections.Generic.SortedSet[string]'
foreach($file in $files) {
    foreach($match in [regex]::Matches([IO.File]::ReadAllText($file),'\\pic\{([^}]+)\}')) { $null=$selected.Add($match.Groups[1].Value) }
}
$manifest=@()
foreach($relative in $selected) {
    $destination=Join-Path $Root ('report/images/report/'+$relative)
    New-Item -ItemType Directory -Force -Path (Split-Path $destination) | Out-Null
    $source=if($relative.StartsWith('game/')) { 'docs/images/'+$relative.Substring(5) } else { $relative }
    if($relative.StartsWith('textures/')) {
        Add-Type -AssemblyName System.Drawing
        $source='assets/textures/'+[IO.Path]::GetFileNameWithoutExtension($relative)+'.ppm'
        $bytes=[IO.File]::ReadAllBytes((Join-Path $Root $source))
        $header=[Text.Encoding]::ASCII.GetBytes("P6`n256 256`n255`n")
        if($bytes.Length -ne $header.Length+256*256*3) { throw "Unexpected texture format: $source" }
        $bitmap=New-Object Drawing.Bitmap(256,256)
        try {
            for($y=0;$y -lt 256;$y++) { for($x=0;$x -lt 256;$x++) {
                # Packed RGB byte offset = headerBytes + 3*(y*width+x).
                $offset=$header.Length+($y*256+$x)*3
                $bitmap.SetPixel($x,$y,[Drawing.Color]::FromArgb($bytes[$offset],$bytes[$offset+1],$bytes[$offset+2]))
            } }
            $bitmap.Save($destination,[Drawing.Imaging.ImageFormat]::Png)
        } finally { $bitmap.Dispose() }
    } else { Copy-Item -LiteralPath (Join-Path $Root $source) -Destination $destination -Force }
    # Record each copied image's SHA-256 fingerprint so later cleanup can recognize unchanged owned files.
    $manifest+=[pscustomobject]@{Source=$source;ReportImage=('report/images/report/'+$relative);SHA256=(Get-FileHash $destination -Algorithm SHA256).Hash}
}
$manifestPath=Join-Path $Root 'report/report-image-manifest.csv'
if(Test-Path -LiteralPath $manifestPath) {
    $ownedRoot=[IO.Path]::GetFullPath((Join-Path $Root 'report/images/report'))+[IO.Path]::DirectorySeparatorChar
    foreach($old in Import-Csv -LiteralPath $manifestPath) {
        $oldPath=[IO.Path]::GetFullPath((Join-Path $Root $old.ReportImage))
        if(-not $oldPath.StartsWith($ownedRoot,[StringComparison]::OrdinalIgnoreCase)) { throw 'Invalid previous report manifest path.' }
        if($old.ReportImage -notin $manifest.ReportImage -and (Test-Path -LiteralPath $oldPath)) {
            # Remove only unchanged copies owned by the previous generated manifest.
            if((Get-FileHash -LiteralPath $oldPath -Algorithm SHA256).Hash -eq $old.SHA256) { Remove-Item -LiteralPath $oldPath }
        }
    }
}
$manifest | Export-Csv -LiteralPath $manifestPath -NoTypeInformation -Encoding UTF8
Write-Output "Prepared $($selected.Count) report images and source-derived coordinate/assembly tables."
