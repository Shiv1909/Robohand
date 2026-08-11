# STATUS — read this first after a break

Snapshot of the whole project as of **2026-08-11**. Written as a handoff: if you
(or a fresh Claude session) come back cold, start here.

**Everything is committed and pushed** to `main` at
<https://github.com/Shiv1909/Robohand>. Bench photos on the laptop are untracked
and not needed.

---

## Where we are in one line

**Siri works.** "Hey Siri, turn on Fingerbot" swings the arm. Phases 1, 1b, 1c and
**3** are all validated on real hardware (2026-08-11).

The electronics and firmware side of this project is essentially done. What remains
is mechanical: printing an arm, mounting it, and calibrating the angles against the
real switchboard.

| Phase | State |
| --- | --- |
| 0 — Wokwi simulation | ✅ done, on an Arduino Uno |
| 1 — Servo moves | ✅ **validated on hardware 2026-08-11** |
| 1b — Debounced button | ✅ **validated on hardware 2026-08-11**, real switch |
| 1c — Two-position on/off | ✅ **validated on hardware 2026-08-11** |
| 1d — Calibration console | firmware ✅ · 20 tests ✅ · **not flashed** |
| 2 — WiFi trigger | ⏭️ **skipped** — HomeSpan subsumes it |
| 3 — Siri / HomeSpan | ✅ **validated on hardware 2026-08-11** |
| 4 — Printed parts | CAD ✅ rendering · **not printed, not test-fitted** |
| 5 — Permanent install | not started |

**41 native tests pass. 4 firmware environments build clean.**

---

## What the first hardware session actually proved (2026-08-11)

Measured, not assumed:

- **No brownout.** 22 s of continuous auto-pressing, six cycles, and the boot
  banner never reappeared. The 5 V 2 A adapter + 1000 µF near the servo is
  sufficient for one MG90S. This was the project's single biggest risk.
- **Timing is exact.** 0.8 s hold, 3.0 s gap, 3.8 s period — matches
  `phase1_autopress.cpp` to the tenth of a second.
- **The debouncer works against real bounce.** Two separate runs: eight jumper taps
  on GPIO 4 gave eight presses, and fourteen pushes of the real tactile switch gave
  fourteen presses. **No two events closer than 0.9 s in either run** — contact
  bounce settles in single-digit milliseconds, so zero double-fires got through.
  `include/debounce.h` needs no tuning; `DEBOUNCE_MS = 50` is right.
- **The two-position state machine alternates correctly.** Eight presses under
  `phase1c_toggle` gave strict ON/OFF/ON/OFF with no repeats, each move taking
  exactly 0.7 s (`HOLD_MS` 500 + 200 ms settle). `include/switch_state.h` behaves on
  hardware as its 13 tests said it would.
- **The idempotency guard fired for real, within 100 s of the first Siri session.**
  HomeKit sent "turn on" while the switch was already ON, and `requestOn()` returned
  `Action::None` so the arm did not move. With a plain bool it would have pressed an
  already-on rocker and turned the light OFF. This is the single most valuable thing
  `include/switch_state.h` does, it was predicted in that file's header comment
  before any hardware existed, and it is now observed rather than theoretical.
- **3.3 V PWM drives the MG90S fine.** The logic level converter (item 16919) was
  bought as insurance and is not needed.
- **HomeSpan must be pinned to 1.9.x.** HomeSpan 2.x hard-`#error`s unless the
  arduino-esp32 core is ≥ 3.3.0, and `platform = espressif32 @ 7.0.1` ships 2.0.17.
  Pinned in `platformio.ini`, scoped to the Phase 3 env only — see below.
- **Wire tactile switches diagonally.** Straight across a row gave a permanent
  closed circuit; the same switch worked immediately when rewired diagonally. See
  `WIRING.md` — this cost an hour.

---

## What's actually blocked

1. **Switch measurements not taken.** Blocks final arm length and mount position.
2. **Wokwi can't simulate ESP32** here. Dead end — don't retry, see below.

Nothing else. Phases 1 and 1b are done on hardware.

---

## Next actions, in order

Do these in sequence. Each adds exactly one new thing to debug.

**Everything left is mechanical.** No firmware is blocked.

1. **Measure the switchboard** — rocker height × width, gang pitch, throw. Needs
   only a ruler, and it is the main blocker: it decides arm length and mount
   position for Phase 4.
2. **Print** one cradle + one `arm20` in **PETG** (`cad/stl/`) — *if* your filament
   is PETG. PLA creeps under sustained load and is only good for test-fitting.
3. Test-fit the cradle and arm against the real servo; correct the parameter
   block in `cad/fingerbot.scad` and re-run `cad\render.ps1`.
4. Mount it on the switchboard with the push-pull tether (see `project.md`, "The
   rocker problem").
5. `pio run -e phase1d_calibrate -t upload` → find the three angles by typing,
   `save`, then paste them into **both** `phase1c_toggle.cpp` and
   `phase3_homekit.cpp`. Needs the printed arm and a real mount first.
6. Then scale: servos 2..N for the other switches on the board.

The angles in `phase1c_toggle.cpp` and `phase3_homekit.cpp` (REST 90, ON 60,
OFF 120) are placeholders. They are *not* calibrated against anything — step 5's
job. Both files carry the same three constants; keep them in step.

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
| CP210x VCP driver | Silicon Labs | installed 2026-08-11, `oem186.inf`. **Windows Update does not have it** |

### The ESP32 shows up as COM3

The DevKitC-32E uses a **CP2102N** USB-UART bridge. Windows 11 has no driver for
it and Windows Update will not find one — Device Manager reports
`CM_PROB_FAILED_INSTALL` (Code 28) and **no COM port appears at all**, so
`pio device list` is silently empty. The board is fine; it's just unreachable.

Fixed by installing the Silicon Labs CP210x Universal Windows Driver
(`silabser.inf`) with `pnputil /add-driver ... /install` from an elevated shell.
Already done on this machine; only relevant if it's ever rebuilt.

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
