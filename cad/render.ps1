# Render every Fingerbot part to cad/stl/.
#
#   .\render.ps1              # default arm lengths 20/25/30
#   .\render.ps1 -Arms 22,24  # custom lengths, after you've measured the switch
#
# OpenSCAD is not on PATH by default on Windows; this finds it either way.

param([int[]]$Arms = @(20, 25, 30))

$openscad = (Get-Command openscad -ErrorAction SilentlyContinue).Source
if (-not $openscad) {
    $openscad = @(
        "C:\Program Files\OpenSCAD\openscad.exe",
        "$env:LOCALAPPDATA\Programs\OpenSCAD\openscad.exe"
    ) | Where-Object { Test-Path $_ } | Select-Object -First 1
}
if (-not $openscad) {
    Write-Error "OpenSCAD not found. Install it: winget install -e --id OpenSCAD.OpenSCAD"
    exit 1
}

$src = Join-Path $PSScriptRoot "fingerbot.scad"
$out = Join-Path $PSScriptRoot "stl"
if (-not (Test-Path $out)) { New-Item -ItemType Directory -Force $out | Out-Null }

foreach ($p in @("cradle", "tab")) {
    & $openscad -D "part=`"$p`"" -o (Join-Path $out "$p.stl") $src
    "rendered $p.stl"
}

foreach ($len in $Arms) {
    & $openscad -D 'part="arm"' -D "arm_len=$len" -o (Join-Path $out "arm$len.stl") $src
    "rendered arm$len.stl"
}
