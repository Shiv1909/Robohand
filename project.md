# DIY Fingerbot — Project Brief

A voice-controlled robot finger that presses a physical wall switch on command
("Hey Siri, turn on the fan"). This is the DIY version of a commercial Fingerbot /
SwitchBot Bot, built as a first hands-on robotics + embedded/IoT project.

If you're Claude Code reading this: this file is the handoff from a planning chat.
It has the full plan and current status so you can pick up mid-project.

## What we're building

An ESP32 drives a small servo motor. The servo has a 3D-printed arm that swings to
press a switch, holds briefly, then retracts. The ESP32 announces itself to Apple
Home (via the HomeSpan library) so Siri can trigger the press — no hub, no
subscription.

## Hardware

- **ESP32 dev board** (WiFi + BLE — needed for the Siri/HomeKit step)
- **MG90S servo** (metal-gear; wall switches need a firm push and metal gears last)
- **Separate 5V supply for the servo** (see SERVO BROWNOUT below — not optional)
- Pushbutton (Phase 1b trigger)
- Breadboard + jumper wires
- USB cable to match the board
- 3D-printed arm + housing (Phase 4 — a printer is available)

## Target

A rectangular multi-gang wall switchboard with several switches. End goal is to
voice-control specific switches on it. **We start with ONE switch, all the way
through Siri, then replicate.** One ESP32 can drive several servos, each exposed to
Apple Home as its own named accessory — so one brain controls the whole board.

## Phased plan

- [x] **Phase 0 — Simulation.** Press logic proven in Wokwi.
- [ ] **Phase 1 — Real circuit.** ESP32 + servo on a breadboard, servo physically
      swinging from firmware over USB.
  - [x] Firmware written and compiling — `src/phase1_autopress.cpp`
  - [ ] Flashed and validated on real hardware ← **blocked: parts in transit**
- [ ] **Phase 1b — Button trigger.** Pushbutton on a GPIO fires the press instead
      of the auto-loop.
  - [x] Firmware written and compiling — `src/phase1b_button.cpp`
  - [ ] Flashed and validated on real hardware
- [ ] ~~**Phase 2 — Wireless trigger.**~~ **SKIPPED.** HomeSpan in Phase 3 subsumes
      it — a native HomeKit accessory already gives phone control *and* Siri from
      the same code, so a standalone web trigger is throwaway work.
- [ ] **Phase 3 — Siri / Apple Home.** HomeSpan makes the ESP32 a native HomeKit
      accessory; "Hey Siri, turn on X" fires the servo.
      **Not written yet — deliberately held** until Phases 1 and 1b are both
      validated on the real board. See "Why incremental" below.
- [ ] **Phase 4 — Mechanical body.** Design + 3D-print the arm and a mount sized to
      the actual switchboard; calibrate press angle/depth.
- [ ] **Phase 5 — Permanent.** Battery power, enclosure, mount on the wall.
- [ ] **Later — Scale.** Add servos 2..N for the other switches on the board.

## Current status

**Nothing has run on real hardware yet.** Parts are in transit.

Phase 0 is done in simulation. Phase 1 and Phase 1b firmware are both written and
**both compile clean for `esp32dev`** (verified 2026-08-02) — but they are unflashed
and entirely unproven on a physical board. Compiling only proves the code is valid
C++ and links against the ESP32 Arduino framework; it says nothing about whether the
servo moves. The first real flash will also be the first real-hardware debug session.

Toolchain: VS Code + PlatformIO Core 6.1.19 + Claude Code. PlatformIO Core is
installed at `~/.platformio` with `pio` on the user PATH.

## Firmware layout

Two PlatformIO environments, one source file each, so both phases stay
independently flashable:

| Environment        | Source file              | What it does                    |
| ------------------ | ------------------------ | ------------------------------- |
| `phase1_autopress` | `phase1_autopress.cpp`   | Presses every 3s. No inputs.    |
| `phase1b_button`   | `phase1b_button.cpp`     | Presses on a debounced button.  |
| `native`           | `test/test_debounce/`    | Debounce unit tests, on the PC. |

```
pio run -e phase1_autopress -t upload
pio run -e phase1b_button   -t upload
```

Always pass `-e`. A bare `pio run` builds only `default_envs` (`phase1_autopress`),
not both — so it will silently skip Phase 1b and look like it succeeded.

## Tests

```
pio test -e native
```

Eight Unity tests over `include/debounce.h`, running on the PC — no ESP32, no
servo, no wiring. Debounce is the only nontrivial algorithm in Phase 1b and the
one most annoying to diagnose on real hardware, so it's pinned down here with a
synthetic clock instead: clean press, bouncing contacts, held button, release,
double press, sub-window glitch, and `millis()` rollover at ~49 days.

The suite is verified non-vacuous — deleting the debounce window from
`Debouncer::update()` makes the glitch and bounce tests fail.

This is why the debounce logic sits in `include/debounce.h` rather than inline in
`phase1b_button.cpp`: it's free of `digitalRead()`/`millis()` so it compiles for
the host. `phase1b_button.cpp` just converts the active-low pin into a bool and
supplies `millis()`.

**Tests prove the logic, not the machine.** A green run says nothing about servo
torque, brownout, or wiring — those need hardware.

## Simulation (Wokwi) — set up, but BLOCKED

```
pio run -e phase1b_button
wokwi-cli . --scenario test/wokwi/phase1b_button.scenario.yaml --timeout 30000
```

`diagram.json` models the ESP32 + servo + pushbutton and passes `wokwi-cli lint`.
The button has bounce enabled, so the simulated firmware would face genuinely
dirty edges — a real integration test of the debouncer, covering what the unit
tests can't (ESP32Servo timer allocation, `attach()`, the GPIO pull-up).

**It does not run.** ESP32 simulations produce no serial and never execute. The
cause is outside this repo — full elimination trail is in `wokwi.toml`, but in
short: the token is valid, an AVR sketch simulates correctly with the same CLI
and token, and a bare `Serial.println` ESP32 sketch is equally silent. Flash
format, board type, and timeout were all ruled out.

**The VS Code extension fails the same way** (2026-08-02): simulator starts, the
servo never moves, serial stays empty. So this is not a wokwi-cli bug — both
front ends hit it, and the common factor is ESP32 itself.

**AVR works, ESP32 does not.** This is almost certainly why Phase 0 was proven on
an Arduino Uno rather than an ESP32 — the same wall, hit earlier. Worth
remembering before anyone sinks time into "simulate it first" again.

The decisive test not yet run: open an ESP32 project on wokwi.com in a browser.
Wokwi compiles server-side there, so it bypasses this machine's firmware
entirely. If that works, the problem is local; if it fails too, ESP32 simulation
simply isn't available here and the answer is to stop trying and wait for
hardware.

Treat this as unproven — the setup can currently only fail, never pass.

`phase1_autopress` is the default env and **the known-good baseline** — keep it
working. When a later phase misbehaves, flash it to prove the servo, wiring, and
power are still fine before debugging anything else. The two files duplicate their
servo code on purpose; sharing a header would mean a bug hunt in one phase could
silently change the baseline you fall back to.

## Why incremental (don't jump to HomeSpan)

Each flash should add exactly ONE new thing to debug:

> servo motion (Phase 1) → GPIO input (Phase 1b) → networking/pairing (Phase 3)

If the first flash were HomeSpan, a servo that doesn't move would have six possible
causes — wiring, power, the timer fix, WiFi credentials, HomeKit pairing, accessory
code — instead of one. HomeSpan is also best written when it can be flashed and
tested live, so WiFi and pairing bugs get caught in the moment rather than sitting
in week-old code.

## Key decisions & gotchas (learned the hard way)

- **SERVO BROWNOUT — wire around this from day one.** An MG90S stalling against a
  stiff wall switch pulls a current spike big enough to dip the voltage through the
  ESP32 dev board's onboard regulator and *reset the board mid-press*.
  **Symptom:** the ESP32 reboots at exactly the moment the finger pushes hardest.
  It looks like a code bug. It is not a code bug — no amount of firmware debugging
  will fix it.
  **Fix:** power the servo from a **separate 5V supply**, and tie the grounds
  together (ESP32 GND ↔ supply GND — common ground, required, or the servo won't
  see a valid signal). Do NOT run the servo off the ESP32's 5V/VIN pin. Wire it
  this way from the start rather than "fixing" it later.

- **Build output lives outside this folder, on purpose.** This project sits in a
  OneDrive-synced tree. `workspace_dir` in `platformio.ini` redirects the whole
  `.pio` workspace to `%LOCALAPPDATA%\pio-workspace\fingerbot` (~50 MB and growing)
  so OneDrive isn't syncing build artifacts or locking files mid-compile. If you
  ever see a `.pio` folder appear next to `src/`, that redirect got lost.

- **Use ESP32, not Arduino Uno, for the real build** — the Uno was only used to
  prove the mechanism in simulation. The real build needs WiFi/BLE for Siri.

- **ESP32Servo needs its PWM timers allocated BEFORE `attach()`** or the servo
  silently never moves. The `ESP32PWM::allocateTimer(...)` lines in both firmware
  files are that fix — keep them.

- **Avoid ESP32 strapping pins for inputs** (GPIO 0, 2, 12, 15) — they're read at
  boot and a held button can stop the board from starting. The Phase 1b button is
  on GPIO 4.

- Build ONE switch end-to-end before scaling to the multi-gang board.

- **Wokwi CLI cannot currently simulate ESP32** on this setup — see "Simulation"
  above. AVR simulates fine with the same CLI and token, so the tooling and
  account are good; it's ESP32-specific. (This supersedes the older note here
  that Wokwi "isn't needed" — that was written assuming hardware was days away.
  With hardware delayed, simulating the real compiled binary is worth having;
  it just doesn't work yet.)

## Next actions

Done already: PlatformIO Core installed, ESP32 toolchain + ESP32Servo pulled, and
both environments compiled clean.

1. Install the PlatformIO **extension** in VS Code — the CLI is already installed;
   the extension just adds the toolbar and environment switcher.
2. When the ESP32 arrives: wire the servo to its **own 5V supply with common
   ground**, then `pio run -e phase1_autopress -t upload` and watch the arm press
   every 3s.
3. Then add the button on GPIO 4 and `pio run -e phase1b_button -t upload`.
4. Only once both are validated on real hardware, write Phase 3 (HomeSpan).
