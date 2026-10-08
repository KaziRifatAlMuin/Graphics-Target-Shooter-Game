param([string]$Source='.release-work', [string]$Destination='docs/images', [string]$Pattern='*.ppm')
$ErrorActionPreference='Stop'
Add-Type -AssemblyName System.Drawing
# Compile a small C# helper for fast pixel copying into the Windows bitmap layout.
Add-Type -TypeDefinition @"
public static class PixelCopy {
    // Swap packed PPM RGB into bitmap BGR while honoring the destination row stride (bytes per row).
    public static byte[] Convert(byte[] source, int offset, int width, int height, int stride) {
        byte[] rgb=new byte[stride*height];
        for(int y=0;y<height;y++) for(int x=0;x<width;x++) {
            int from=offset+(y*width+x)*3, to=y*stride+x*3;
            rgb[to]=source[from+2]; rgb[to+1]=source[from+1]; rgb[to+2]=source[from];
        }
        return rgb;
    }
}
"@
New-Item -ItemType Directory -Force $Destination | Out-Null
# Lossless conversion only: no retouching, overlays or synthesized screenshots.
foreach($file in Get-ChildItem -LiteralPath $Source -Filter $Pattern) {
    $bytes=[IO.File]::ReadAllBytes($file.FullName)
    $offset=0; $lines=@()
    for($line=0;$line -lt 3;$line++) {
        $start=$offset
        while($bytes[$offset] -ne 10) { $offset++ }
        $lines += [Text.Encoding]::ASCII.GetString($bytes,$start,$offset-$start)
        $offset++
    }
    if($lines[0] -ne 'P6' -or $lines[2] -ne '255') { throw 'Unsupported PPM' }
    $width,$height=$lines[1].Split(' ')
    $width=[int]$width; $height=[int]$height
    $bitmap=New-Object Drawing.Bitmap($width,$height,[Drawing.Imaging.PixelFormat]::Format24bppRgb)
    # Lock raw bitmap memory for one bulk copy instead of setting pixels one by one.
    $data=$bitmap.LockBits((New-Object Drawing.Rectangle(0,0,$width,$height)),[Drawing.Imaging.ImageLockMode]::WriteOnly,$bitmap.PixelFormat)
    try {
        $rgb=[PixelCopy]::Convert($bytes,$offset,$width,$height,$data.Stride)
        [Runtime.InteropServices.Marshal]::Copy($rgb,0,$data.Scan0,$rgb.Length)
    } finally { $bitmap.UnlockBits($data) }
    try { $bitmap.Save((Join-Path (Resolve-Path $Destination) ($file.BaseName+'.png')),[Drawing.Imaging.ImageFormat]::Png) }
    finally { $bitmap.Dispose() }
    Write-Output $file.BaseName
}
