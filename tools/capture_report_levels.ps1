$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
Push-Location $root
try {
    New-Item -ItemType Directory -Force -Path '.report-build' | Out-Null
    $coreLine=(Get-Content Makefile | Where-Object { $_ -match '^CORE = ' })
    $core=($coreLine -replace '^CORE = ','').Split(' ',[StringSplitOptions]::RemoveEmptyEntries)
    $arguments=@('-std=c++17','-O2','-Wall','-Wextra','-Iinclude','-Isrc','-Itools',
        'tools/capture_report_levels.cpp','tools/Demonstration.cpp')+$core+@(
        'src/rendering/Renderer.cpp','src/rendering/TextureCache.cpp','src/third_party/glad.c',
        '-Llib','-lglfw3','-lopengl32','-lgdi32','-lwinmm','-o','.report-build/capture_report_levels.exe')
    & g++ @arguments
    if($LASTEXITCODE -ne 0){throw 'Level photograph compilation failed.'}
    & '.\.report-build\capture_report_levels.exe' $root
    if($LASTEXITCODE -ne 0){throw 'Level photographs failed.'}
} finally { Pop-Location }
