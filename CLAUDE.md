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

## Building

Always pass `-e`. A bare `pio run` builds only `default_envs` (`phase1_autopress`)
and silently skips Phase 1b while looking like it succeeded.

```
pio run -e phase1_autopress            # compile only
pio run -e phase1_autopress -t upload  # compile + flash
pio run -e phase1b_button   -t upload
```

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
