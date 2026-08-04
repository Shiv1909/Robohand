# Robohand — DIY Fingerbot

A voice-controlled robot finger that physically presses a wall switch on command.
Say *"Hey Siri, turn on the fan"* and a servo swings an arm into the switch.

The homemade version of a commercial SwitchBot Bot — built as a first hands-on
robotics / embedded project, and aiming to beat the commercial one on a specific
axis: **density**. A SwitchBot is 43 × 37 mm and physically cannot fit two adjacent
switches on a 22 mm-pitch Indian modular board. An MG90S can.

## Status

Hardware is on order. Nothing has run on a real board yet.

| Phase | State |
| --- | --- |
| 0 — Simulation | ✅ done (Arduino Uno, in Wokwi) |
| 1 — Servo moves on real hardware | firmware written + compiling, **not yet flashed** |
| 1b — Debounced button trigger | firmware written + compiling + unit-tested, **not yet flashed** |
| 1c — Two-position on/off toggle | firmware written + compiling + unit-tested, **not yet flashed** |
| 2 — WiFi trigger | ⏭️ skipped — HomeSpan subsumes it |
| 3 — Siri / Apple Home | deliberately deferred until 1 and 1b are validated on hardware |
| 4 — 3D-printed arm and mount | blocked on switch measurements |
| 5 — Permanent install | not started |

## Hardware

ESP32 + MG90S metal-gear servo, driven from a **separate 5 V supply** with a common
ground. Full parts list with links and prices: **[BOM.md](BOM.md)**.

## Quick start

```bash
# compile
pio run -e phase1_autopress          # servo presses every 3s, no inputs
pio run -e phase1b_button            # servo presses on a debounced button
pio run -e phase1c_toggle            # button toggles between on/off angles

# run logic tests on your PC — no ESP32 needed (21 tests)
pio test -e native

# flash
pio run -e phase1_autopress -t upload
```

Always pass `-e`. A bare `pio run` builds only the default environment and silently
skips the other.

**Before wiring anything, read [WIRING.md](WIRING.md).** It has the layout and a
pre-flight checklist — reversed polarity on the servo supply will kill the ESP32.

## Repo layout

```
src/phase1_autopress.cpp    Phase 1 firmware. Known-good baseline; keep it working.
src/phase1b_button.cpp      Phase 1b firmware. Standalone by design.
src/phase1c_toggle.cpp      Phase 1c firmware. Two-position on/off with state.
include/debounce.h          Debounce logic, free of Arduino calls so it unit-tests.
include/switch_state.h      On/off state machine. Idempotent — see below.
test/test_debounce/         8 native Unity tests for the debouncer.
test/test_switch_state/     13 native Unity tests for the state machine.
test/wokwi/                 Wokwi scenario (blocked — see project.md).
platformio.ini              Four environments: three firmware, one native test.
project.md                  Plan, phase status, and every gotcha learned so far.
BOM.md                      Parts list, prices, power analysis, measurements.
WIRING.md                   Wiring diagram and first-power-on checklist.
CLAUDE.md                   Toolchain notes for this machine.
```

Each firmware environment compiles exactly one source file, so both can live in
`src/` without their `setup()`/`loop()` colliding. Phase 1 is preserved permanently
as a fallback for isolating hardware faults.

## Things learned the hard way

Full list in [project.md](project.md). The ones that cost the most time:

- **Servo brownout.** An MG90S stalling against a stiff switch browns out the ESP32
  through its onboard regulator. The board reboots at exactly the moment the finger
  pushes hardest — it looks like a code bug and isn't. Separate 5 V supply, common
  ground, bulk capacitor near the servo.
- **ESP32Servo needs its PWM timers allocated before `attach()`**, or the servo
  silently never moves.
- **Rockers need two press points.** Press the top for ON, the bottom for OFF. A
  single-point presser can only ever switch one way — solved with a push-pull tether,
  the same approach SwitchBot uses.
- **State changes must be idempotent.** Apple Home re-sends state freely. Actuating
  on a redundant "turn on" would physically press the switch again and turn the light
  *off*. `switch_state.h` returns `Action::None` in that case, and it's unit-tested.
- **Wokwi cannot simulate ESP32 here.** AVR works fine with the same CLI and token;
  ESP32 produces no serial and never executes. Ruled out across both the CLI and the
  VS Code extension. Probably why Phase 0 was done on an Uno.
- **Battery life is set by idle WiFi, not the servo.** The servo is under 2 % of the
  energy budget. Light sleep (not deep sleep) gets ~7.5 mA and makes portable viable.

## License

Personal project, no license granted. Ask if you want to reuse something.
