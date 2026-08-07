# STATUS — read this first after a break

Snapshot of the whole project as of **2026-08-07**. Written as a handoff: if you
(or a fresh Claude session) come back cold, start here.

**Everything is committed and pushed.** 27 commits on `main` at
<https://github.com/Shiv1909/Robohand>. Nothing lives only on this laptop except
two untracked screenshots.

---

## Where we are in one line

All firmware and CAD is written, compiling, and unit-tested — **nothing has ever
run on real hardware**, because the parts have only just been ordered.

| Phase | State |
| --- | --- |
| 0 — Wokwi simulation | ✅ done, on an Arduino Uno |
| 1 — Servo moves | firmware ✅ compiling · **not flashed** |
| 1b — Debounced button | firmware ✅ · 8 tests ✅ · **not flashed** |
| 1c — Two-position on/off | firmware ✅ · 13 tests ✅ · **not flashed** |
| 1d — Calibration console | firmware ✅ · 20 tests ✅ · **not flashed** |
| 2 — WiFi trigger | ⏭️ **skipped** — HomeSpan subsumes it |
| 3 — Siri / HomeSpan | ⛔ **deliberately not started** |
| 4 — Printed parts | CAD ✅ rendering · **not printed, not test-fitted** |
| 5 — Permanent install | not started |

**41 native tests pass. 4 firmware environments build clean.**

---

## What's actually blocked

1. **Hardware in transit.** Everything downstream waits on it.
2. **Switch measurements not taken.** Blocks final arm length and mount position.
3. **Wokwi can't simulate ESP32** here. Dead end — don't retry, see below.

---

## Next actions, in order

Do these in sequence. Each adds exactly one new thing to debug.

1. **Print** one cradle + one `arm20` in **PETG** (`cad/stl/`). Can be done now,
   *if* your filament is PETG — PLA creeps under sustained load and is only good
   for test-fitting.
2. **When the box arrives:** read `WIRING.md` and follow the checklist. **Do not
   skip step 2** — metering the adapter polarity before connecting anything is
   what saves the ESP32.
3. `pio run -e phase1_autopress -t upload` → arm should press every 3 s.
4. Add the button on GPIO 4 → `pio run -e phase1b_button -t upload`.
5. `pio run -e phase1c_toggle -t upload` → each press alternates on/off.
6. Test-fit the cradle and arm against the real servo; correct the parameter
   block in `cad/fingerbot.scad` and re-run `cad\render.ps1`.
7. `pio run -e phase1d_calibrate -t upload` → find the three angles by typing,
   `save`, then paste them into `phase1c_toggle.cpp`.
8. **Only then** write Phase 3 (HomeSpan).

---

## Toolchain installed on this machine

All PATH entries are in the **User** environment, so they survive a restart. Open
a **new** terminal after rebooting — and restart VS Code fully, not just its
integrated terminal, since it inherits the PATH of whatever launched it.

| Tool | Version | Location / notes |
| --- | --- | --- |
| PlatformIO Core | 6.1.19 | `~\.platformio`, `penv\Scripts` on User PATH |
| MinGW-w64 GCC | 16.1.0 | winget WinLibs UCRT, on User PATH. **Required for `pio test`** |
| OpenSCAD | 2021.01 | `C:\Program Files\OpenSCAD` — *not* on PATH; `render.ps1` finds it |
| wokwi-cli | 0.26.1 | `%LOCALAPPDATA%\Programs\wokwi-cli`, on User PATH |
| Node / npm | 24.14.0 / 11.9.0 | |
| gh | 2.95.0 | authenticated as `Shiv1909` |

**Build output lives outside this folder** — `workspace_dir` in `platformio.ini`
sends `.pio` to `%LOCALAPPDATA%\pio-workspace\fingerbot`, because this project
sits in a OneDrive-synced tree. If a `.pio\` folder ever appears next to `src\`,
that redirect got lost.

### Two machine-specific traps

- **Never run `python get-platformio.py`.** Bare `python` here resolves to another
  app's virtualenv (`AppData\Local\hermes\hermes-agent\venv`). Use `py -3.13`.
- **Set a short TEMP before running pio from an agent session.** The scratchpad
  path is ~150 chars and pip/pio blow past Windows' 260-char `MAX_PATH`. The error
  blames antivirus; that's a red herring.
  ```powershell
  $env:TMP = "$env:LOCALAPPDATA\Temp"; $env:TEMP = $env:TMP
  ```

`WOKWI_CLI_TOKEN` is stored in the User environment. **The repo is public — never
commit it.** Remove with:
```powershell
[Environment]::SetEnvironmentVariable("WOKWI_CLI_TOKEN", $null, "User")
```

---

## Decisions already made — don't re-litigate

Each of these cost real time to work out. Full reasoning is in `project.md`.

| Decision | Why |
| --- | --- |
| **Separate 5 V supply, common ground, 1000 µF near the servo** | Stall current browns out the ESP32. Board reboots exactly when the arm pushes hardest — looks like a code bug, isn't |
| **`ESP32PWM::allocateTimer()` before `attach()`** | Otherwise the servo silently never moves |
| **Phase 1 kept as a permanent baseline** | Flash it to prove servo+wiring+power still work before debugging anything else |
| **Phase 2 skipped** | HomeSpan gives phone control *and* Siri from the same code |
| **Phase 3 deferred until 1/1b run on hardware** | One new variable per flash. HomeSpan first would give a dead servo six possible causes instead of one |
| **Rocker needs two press points** | Top = ON, bottom = OFF. Solved with a push-pull tether to an adhesive tab, as SwitchBot does |
| **State changes must be idempotent** | Apple Home re-sends state; acting on a redundant "turn on" would physically turn the light off |
| **Arm 20–25 mm, not 40** | SwitchBot's 8 N target ÷ MG90S's 19.6 N·cm. A 40 mm arm gives only 4.9 N |
| **Wokwi ESP32 is dead here** | AVR works with the same CLI and token; ESP32 never executes. Both CLI and VS Code extension. Probably why Phase 0 used an Uno |
| **Portable IS viable** | ~7.5 mA with WiFi light sleep, not the 80 mA default. ~2 weeks per 18650. Idle radio dominates; the servo is <2 % |
| **6 servos need staggered mounting** | Cradle is 28.1 mm wide, gang pitch ~22 mm. Alternate above/below the rocker line |
| **Don't buy a small board yet** | ESP32-C3 has only 6 PWM channels; Thread would need a C6. A PCA9685 makes the board choice irrelevant |

---

## Open questions for you

- [ ] **Is your red/black filament PETG or PLA?** PLA is fine for test-fitting,
      not for the permanent build.
- [ ] **Rocker dimensions with a ruler.** The AR measurement said 2 × 7 cm, but the
      photos show a ~1:2 ratio, so the height is probably ~4–4.5 cm.
- [ ] **Gang pitch**, centre-to-centre. Decides how bad the staggering has to be.
- [ ] **Actuation force** (optional — see `BOM.md` for three ways to measure it
      without a proper scale). Not a blocker; printing 20/25/30 mm arms settles it.

---

## Offered but not built

- **CI on GitHub Actions** — would run the 41 tests + 4 builds on every push.
- **Electronics enclosure** — the ESP32 box; DevKitC is 54.4 × 27.9 × 13 mm.
- **A phone app** — recommended against. Apple Home *is* the app.

---

## Document index

| File | What's in it |
| --- | --- |
| `README.md` | Project overview, quick start, repo layout |
| `project.md` | The plan, phase status, every gotcha learned |
| `BOM.md` | Parts list with Robu links, power analysis, measurements |
| `WIRING.md` | Wiring diagram + first-power-on checklist. **Read before connecting anything** |
| `CLAUDE.md` | Toolchain notes for this machine |
| `STATUS.md` | This file |

### Commands worth remembering

```bash
pio run -e phase1_autopress -t upload    # always pass -e; bare `pio run` skips envs
pio test -e native                       # 41 tests, no hardware needed
cd cad; .\render.ps1                     # regenerate STLs + previews
```
