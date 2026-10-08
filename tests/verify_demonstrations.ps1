param([string]$Root=(Split-Path $PSScriptRoot -Parent))
$ErrorActionPreference='Stop'
Add-Type -AssemblyName System.Drawing
$count=0
$diagramCount=0
# Validate generated image dimensions, document links, and SVG structure before checking lighting coverage.
foreach($folder in @('objects','lighting')) {
    $path=Join-Path $Root $folder
    foreach($file in Get-ChildItem -LiteralPath $path -Recurse -Filter '*.png') {
        $image=[System.Drawing.Image]::FromFile($file.FullName)
        try {
            if($image.Width -ne 480 -or $image.Height -ne 360) { throw "Incorrect dimensions: $($file.FullName)" }
            # Force pixel decode, not just PNG header parsing.
            $bitmap=New-Object System.Drawing.Bitmap($image)
            try { $null=$bitmap.GetPixel(240,180) } finally { $bitmap.Dispose() }
        } finally { $image.Dispose() }
        $count++
    }
    foreach($file in Get-ChildItem -LiteralPath $path -Recurse -Filter '*.md') {
        $content=Get-Content -LiteralPath $file.FullName -Raw
        # Source code is included verbatim; ignore fenced code when checking Markdown links.
        $content=[regex]::Replace($content,'(?s)```.*?```','')
        foreach($match in [regex]::Matches($content,'\]\(([^)]+\.(?:png|svg|md))\)')) {
            $target=Join-Path $file.DirectoryName $match.Groups[1].Value
            if(-not (Test-Path -LiteralPath $target -PathType Leaf)) { throw "Broken link in $($file.FullName): $target" }
        }
    }
    foreach($file in Get-ChildItem -LiteralPath $path -Recurse -Filter '*.svg') {
        $xml=New-Object System.Xml.XmlDocument
        $xml.Load($file.FullName)
        if($xml.DocumentElement.LocalName -ne 'svg') { throw "Invalid diagram: $($file.FullName)" }
        $diagramCount++
    }
}
if($count -lt 1000) { throw "Incomplete demonstration output: $count images" }
# Require every day/night, shading, and light-mask screenshot in the demonstration grid.
foreach($period in @('day','night')) {
    foreach($shading in 0..2) { foreach($mask in 0..15) {
        foreach($suffix in @('','-close')) {
            $file=Join-Path $Root "lighting/$period-$shading-mask-$mask$suffix.png"
            if(-not (Test-Path -LiteralPath $file)) { throw "Missing lighting combination: $file" }
        }
    } }
}
# Inactive source families must not change a pixel: point/spot are off in day,
# and the sun is off at night. This checks the renderer's diagnostic mask path.
foreach($period in @('day','night')) {
    $activeMask=if($period -eq 'day') { 3 } else { 13 }
    foreach($shading in 0..2) { foreach($suffix in @('','-close')) {
        $hashes=@{}
        foreach($mask in 0..15) {
            $file=Join-Path $Root "lighting/$period-$shading-mask-$mask$suffix.png"
            $hashes[$mask]=(Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash
        }
        foreach($mask in 0..15) {
            if($hashes[$mask] -ne $hashes[($mask -band $activeMask)]) {
                throw "Inactive light changed pixels: $period shading=$shading mask=$mask view=$suffix"
            }
        }
        if($hashes[0] -eq $hashes[15]) { throw "Lighting did not affect the image: $period shading=$shading" }
    } }
}
Write-Output "Validated $count PNGs, $diagramCount SVGs, Markdown links, all 192 lighting grid captures, and inactive-source pixel equivalence."
