# CLAUDE.md — DIY Fingerbot (ESP32)

**Read `project.md` first.** It is the source of truth for the plan, phase status,
hardware wiring, and the electrical/firmware gotchas (servo brownout, ESP32Servo
timer allocation, strapping pins). Keep it updated as phases complete — it is the
handoff document. This file covers only the local toolchain facts that live
outside `project.md`.

## Toolchain

PlatformIO Core 6.1.19 in its own venv at `C:\Users\ShivanshMital\.platformio\penv`.
`penv\Scripts` was added to the **User** PATH (no admin needed).

Bare `pio` only resolves in a terminal started *after* that PATH edit. VS Code
inherits the PATH of whatever shell launched it, so a stale VS Code needs a full
restart, not just a new integrated terminal. When in doubt use the full path:

```
& "$HOME\.platformio\penv\Scripts\pio.exe" --version
```

## Serial port — the ESP32 is COM3

The DevKitC-32E's USB bridge is a **CP2102N**, and Windows 11 has no driver for it.
**Windows Update does not carry one either.** Until the Silicon Labs CP210x driver
is installed, no COM port exists at all — `pio device list` is silently empty and
uploads fail with `Please specify upload_port`, which looks exactly like a dead
board or a charge-only cable.

Installed here 2026-08-11 as `oem186.inf` via `pnputil /add-driver silabser.inf
/install` from an elevated shell. Full trail in `WIRING.md`. Only relevant if this
machine is rebuilt.

Reading serial from an agent session: **don't** use `pio device monitor`, it's
interactive and blocks. Open the port directly instead, and leave DTR/RTS alone
unless you *want* to reset the board:

```powershell
$sp = New-Object System.IO.Ports.SerialPort 'COM3',115200,'None',8,'one'
$sp.DtrEnable = $false; $sp.RtsEnable = $false   # no reset on open
$sp.Open()
```

Pulsing `RtsEnable` true→false resets the ESP32, which is how to catch the boot
banner. Only one process can hold the port — a background monitor will block the
next `-t upload`.

## Building

Always pass `-e`. A bare `pio run` builds only `default_envs` (`phase1_autopress`)
and silently skips Phase 1b while looking like it succeeded.

```
pio run -e phase1_autopress            # compile only
pio run -e phase1_autopress -t upload  # compile + flash
pio run -e phase1b_button   -t upload
```

### HomeSpan is pinned to 1.9.x, and must stay that way

HomeSpan 2.x hard-`#error`s unless the arduino-esp32 core is ≥ 3.3.0. This project
runs `platform = espressif32 @ 7.0.1`, which ships core **2.0.17**. Getting 3.x
means switching to the pioarduino platform fork — which rebuilds *every*
environment, including the Phase 1/1b/1c binaries that are validated on hardware.

So `platformio.ini` pins `homespan/HomeSpan@^1.9.1`, and adds it to
`[env:phase3_homekit]` **only** rather than to `[esp32_base]`. Adding it to the
shared base would drag the HomeKit + WiFi stack into the baseline builds. With this
scoping the four Phase 1 binaries stay byte-identical — verified 2026-08-11.

Phase 3 also needs `board_build.partitions = huge_app.csv`; at 1.24 MB it would
technically squeeze into the default 1.31 MB app partition, but at 94 % full with
no room to grow.

**Set a short `TEMP` before invoking pio from an agent session:**

```powershell
$env:TMP = "$env:LOCALAPPDATA\Temp"; $env:TEMP = $env:TMP
```

The agent scratchpad TEMP is ~150 chars deep. pip/pio unpack wheels under TEMP and
blow past the Windows `MAX_PATH` 260-char limit. The failure surfaces as
`[Errno 2] No such file or directory ... pip-unpack-.../*.whl.metadata` and the
installer's own error text blames antivirus — that is a red herring, it is purely
path length.

Baseline compile results (2026-08-02, both clean): `phase1_autopress` 21.5% flash /
6.6% RAM; `phase1b_button` similar. A first build on a clean machine pulls the ESP32
toolchain (several hundred MB) and takes minutes.

## Testing

```
pio test -e native
```

Runs on the host PC, needs a native `g++` — MinGW-w64 (WinLibs UCRT, GCC 16.1.0)
is installed at
`%LOCALAPPDATA%\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_*\mingw64\bin`
and is on the User PATH. Visual Studio 18 is present on this machine but has no
C++ workload, so MSVC is not an option.

`[env:native]` sets `build_src_filter = -<*>` because everything in `src/` is
Arduino-only and will not compile for the host. Tests exercise
`include/debounce.h`, which is deliberately Arduino-free.

Note the config uses `[esp32_base]` + `extends` rather than a shared `[env]`
section. `[env]` applies to *every* environment, which would force
`board = esp32dev` onto the native env and break it.

## Simulation (Wokwi)

`wokwi-cli` 0.26.1 is installed at
`%LOCALAPPDATA%\Programs\wokwi-cli\wokwi-cli.exe` and is on the User PATH. It was
installed by downloading the release binary from GitHub rather than the documented
`iwr https://wokwi.com/ci/install.ps1 | iex`, because piping a remote script into
`iex` is blocked here.

Needs `WOKWI_CLI_TOKEN`. **Never commit the token — this repo is public.** It
lives in the User environment; regenerate at https://wokwi.com/dashboard/ci.

ESP32 simulation does not currently work (no serial, chip never executes) while
AVR does. Full elimination trail is in the comment block in `wokwi.toml`. Don't
re-derive it from scratch — start from the VS Code extension instead.

Useful while debugging a diagram:

```
wokwi-cli lint .
wokwi-cli . --timeout 10000 --serial-log-file serial.log
```

Note the built-in linter flags every ESP32 board as an "undocumented type" — that
is noise, every ESP32 entry in its registry has `documented: false`.

## Git

Remote: https://github.com/Shiv1909/Robohand (**public** — no credentials in
source). `include/secrets.h` is gitignored for the Phase 3 WiFi details.

Work on `main` until hardware arrives. Once a phase is validated on the real
board, tag it (`git tag phase1-validated`) so there's a known-physically-working
commit to fall back to while chasing gremlins.

## If PlatformIO ever needs reinstalling

Do **not** run `python get-platformio.py`. On this machine bare `python` resolves to
another app's virtualenv (`AppData\Local\hermes\hermes-agent\venv\Scripts\python.exe`),
and the installer uses the *invoking* interpreter as the base for PlatformIO's venv —
following the docs literally hangs PlatformIO off that unrelated app. Use the Windows
launcher with an explicit real interpreter instead: `py -3.13 get-platformio.py`.

If the installer creates `penv` but fails on package install (the MAX_PATH issue
above), the venv is fine — just re-run the install step alone with a short TEMP:

```powershell
& "$HOME\.platformio\penv\Scripts\python.exe" -m pip install -U platformio
```

## Build output is deliberately outside this folder

`workspace_dir` in `platformio.ini` sends the whole `.pio` workspace to
`%LOCALAPPDATA%\pio-workspace\fingerbot`, because this project sits in a
OneDrive-synced tree and `.pio` was 24.6 MB and churning on every compile. If a
`.pio\` folder ever reappears next to `src\`, that redirect got lost — restore it
and delete the local copy.
