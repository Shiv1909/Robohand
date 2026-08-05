# Render every Fingerbot part to cad/stl/, plus preview images to cad/preview/.
#
#   .\render.ps1              # default arm lengths 20/25/30
#   .\render.ps1 -Arms 22,24  # custom lengths, after you've measured the switch
#
# OpenSCAD is not on PATH by default on Windows; this finds it either way.

param([int[]]$Arms = @(20, 25, 30))

# NOTE: deliberately NOT setting $ErrorActionPreference = "Stop". OpenSCAD writes
# its progress chatter to stderr, and PowerShell 5.1 turns native-command stderr
# into terminating errors under Stop even when the exe exits 0. The size check in
# Render() is the real safety net.

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

$src  = Join-Path $PSScriptRoot "fingerbot.scad"
$out  = Join-Path $PSScriptRoot "stl"
$prev = Join-Path $PSScriptRoot "preview"
foreach ($d in @($out, $prev)) {
    if (-not (Test-Path $d)) { New-Item -ItemType Directory -Force $d | Out-Null }
}

# IMPORTANT: string values passed via -D need BACKSLASH-escaped quotes.
# PowerShell strips plain and backtick-escaped quotes when handing arguments to a
# native exe, so `-D "part=`"cradle`""` arrives as part=cradle — OpenSCAD then
# treats it as an undefined variable, matches no branch, and silently writes an
# EMPTY file while still exiting 0. Hence the size check below.
function Render($partName, $outFile, $extra) {
    $args = @("-D", "part=\`"$partName\`"")
    if ($extra) { $args += $extra }
    $args += @("-o", $outFile, $src)
    & $openscad @args 2>$null | Out-Null

    if (-not (Test-Path $outFile)) {
        Write-Host "FAILED: $outFile was not created" -ForegroundColor Red
        exit 1
    }
    # An empty OpenSCAD export is a few hundred bytes of header and nothing else.
    $size = (Get-Item $outFile).Length
    if ($size -lt 1000) {
        Write-Host "FAILED: $outFile is only $size bytes - the model came out empty" -ForegroundColor Red
        exit 1
    }
    "{0,-18} {1,8:N0} bytes" -f (Split-Path $outFile -Leaf), $size
}

"--- STL ---"
Render "cradle" (Join-Path $out "cradle.stl") $null
Render "tab"    (Join-Path $out "tab.stl")    $null
foreach ($len in $Arms) {
    Render "arm" (Join-Path $out "arm$len.stl") @("-D", "arm_len=$len")
}

"--- previews ---"
$view = @("--autocenter", "--viewall", "--imgsize=900,700", "--colorscheme=Tomorrow")
Render "cradle" (Join-Path $prev "cradle.png") $view
Render "arm"    (Join-Path $prev "arm.png")    $view
Render "tab"    (Join-Path $prev "tab.png")    $view

# Underside view of the arm. Worth keeping reproducible: the horn pocket and
# blade slot are only visible from below, and looking at this view is what
# caught the slot running the full length of the beam.
Render "arm" (Join-Path $prev "arm_underside.png") ($view + @("--camera=0,0,0,120,0,25,90"))

""
"Done. STLs in cad\stl\, preview images in cad\preview\"
