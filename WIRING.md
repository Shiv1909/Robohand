# Wiring & First Power-On

Read this **before** connecting anything. The riskiest moment in this project is the
first power-up: there's a polarised capacitor, a separate supply, and possibly a
cut-and-stripped adapter lead. Reversed polarity kills the ESP32 instantly and can
make the capacitor vent.

Working through this as a checklist turns day one into a procedure instead of an
improvisation.

---

## The key idea: two power sources, one ground

During bench testing the ESP32 and the servo are powered **separately**:

| What | Powered by | Why |
| --- | --- | --- |
| ESP32 | **USB from your PC** | also carries flashing + serial monitor |
| Servo | **5 V adapter** | stall spikes must never touch the ESP32's rail |
| Both | **shared GND** | required, or the servo can't read the signal |

> ### ⚠️ Do NOT connect the adapter's + to the ESP32's VIN or 5V pin
> With USB also plugged in, that back-feeds one supply into the other. The only wire
> between the two power domains is **ground**.

The shared ground is not optional. The servo's PWM signal is measured relative to
ground; without a common reference the servo sees garbage and either sits still or
twitches — a failure that looks exactly like a code bug.

---

## Layout

```
   5V ADAPTER                          BREADBOARD
   ┌────────┐
   │  5V 2A │  red  (+) ─────────────▶ ( + ) RED RAIL ──────┬─────────┐
   │        │                                               │         │
   │        │  black(−) ─────────────▶ ( − ) BLUE RAIL ─┬───┼────┬────┼──┐
   └────────┘                                           │   │    │    │  │
                                                        │   │    │    │  │
                                    1000 µF ── stripe ──┘   │    │    │  │
                                       ▲                    │    │    │  │
                                       └── + leg ───────────┘    │    │  │
                                                                 │    │  │
                                    0.1 µF ────────────────────  ┘    │  │
                                    (parallel, no polarity)           │  │
                                                                      │  │
   MG90S SERVO                                                        │  │
     red    (V+)  ──────────────────────────────────────────────────  ┘  │
     brown  (GND) ─────────────────────────────────────────────────────  ┘
     orange (PWM) ─────────────────────────▶ ESP32 GPIO 18

   ESP32 DevKitC-32E
     GND  ──────────────────────────────────▶ ( − ) BLUE RAIL   ◀── CRITICAL
     USB  ──────────────────────────────────▶ your PC

   BUTTON (Phase 1b only)
     leg 1 ─────────────────────────────────▶ ESP32 GPIO 4
     leg 2 ─────────────────────────────────▶ ESP32 GND
```

### Connection table

| From | To | Notes |
| --- | --- | --- |
| Adapter + | Breadboard **red** rail | confirm polarity first — see checklist |
| Adapter − | Breadboard **blue** rail | |
| 1000 µF **+** leg | Red rail | **near the servo**, not the far end |
| 1000 µF **stripe** leg | Blue rail | stripe = negative. Backwards it can vent |
| 0.1 µF (either leg) | Red rail | no polarity |
| 0.1 µF (other leg) | Blue rail | |
| Servo red | Red rail | |
| Servo brown/black | Blue rail | |
| Servo orange/yellow | ESP32 **GPIO 18** | signal only |
| **ESP32 GND** | **Blue rail** | **the common ground — do not skip** |
| Button leg 1 | ESP32 **GPIO 4** | Phase 1b |
| Button leg 2 | ESP32 GND | no resistor needed; internal pull-up |

**Capacitor placement matters.** Put the 1000 µF physically close to the servo's
power connection. Its job is to supply the stall current surge locally; at the far
end of a long breadboard run, the wire resistance between it and the servo undoes
most of the benefit.

---

## Pre-flight checklist

Work top to bottom. Don't skip step 2 — it's the one that saves the ESP32.

### Before any connections

- [ ] **1.** Unplug everything. Adapter out of the wall, USB out of the ESP32.
- [ ] **2. Identify adapter polarity with the multimeter.** ⭐
      Plug the adapter into the wall with its output leads free and touching nothing.
      Set the meter to **DC volts (20 V range)**. Red probe on one lead, black on the
      other.
      - Reads **+5 V** → red probe is on **positive**.
      - Reads **−5 V** → red probe is on **negative**; swap your understanding.
      - Reads ~0 V or wildly off → bad adapter, stop.
- [ ] **3. Mark the positive lead** with tape or a marker. Do it now, while you know.
- [ ] **4.** Unplug the adapter from the wall again.

### Build the circuit (unpowered)

- [ ] **5.** Adapter **+** → red rail. Adapter **−** → blue rail.
- [ ] **6.** 1000 µF across the rails near where the servo will sit.
      **Stripe leg to the blue (negative) rail.** Double-check this one.
- [ ] **7.** 0.1 µF across the same rails, either way round.
- [ ] **8.** Servo red → red rail, brown → blue rail.
- [ ] **9.** Servo orange → ESP32 GPIO 18.
- [ ] **10. ESP32 GND → blue rail.** The single most-forgotten wire.
- [ ] **11.** Confirm the adapter's **+ is not** connected to ESP32 VIN, 5V, or 3V3.

### Verify before powering (still unpowered)

- [ ] **12.** Meter on **continuity**. Probe ESP32 GND and the blue rail — should beep.
- [ ] **13.** Probe red rail and blue rail — should **NOT** beep. A beep means a short;
      find it before applying power.
- [ ] **14.** Probe red rail against ESP32 3V3 and 5V pins — should **NOT** beep.

### Power up, in this order

- [ ] **15.** Plug in the **adapter only**. ESP32 still unplugged.
- [ ] **16.** Meter on DC volts across the rails. Expect **4.8 – 5.2 V**, correct
      polarity. Wrong or missing → stop and recheck.
- [ ] **17.** Now plug the **USB** into the ESP32.
- [ ] **18.** Flash the baseline:
      ```
      pio run -e phase1_autopress -t upload
      pio device monitor
      ```
- [ ] **19.** Expect `Fingerbot Phase 1 ready. Auto-pressing every 3s...` and the arm
      sweeping every 3 seconds.

---

## If it doesn't work

| Symptom | Likely cause | Check |
| --- | --- | --- |
| Nothing on serial | wrong baud / wrong port | monitor is 115200; check the COM port |
| Serial fine, servo still | missing common ground | step 10 — the most common miss |
| Serial fine, servo still | 3.3 V signal too weak | fit the level converter |
| **ESP32 reboots exactly when the arm pushes hardest** | **brownout** | see below |
| Servo twitches constantly | noisy/insufficient supply | check the 1000 µF is fitted and close to the servo |
| Servo spins continuously, never holds | **you have the 360° servo** | wrong part — must be the 180° version |

### The brownout, specifically

Reboot at the moment of maximum push is *the* signature failure of this project. It
looks exactly like a firmware bug and no amount of code reading will fix it.

Diagnose it: meter on DC volts across the rails **while the servo presses**. If the
reading dips well below 5 V at the moment of stall, that's it.

Fixes in order: confirm the 1000 µF is actually fitted and near the servo → confirm
the servo is on the adapter and not the ESP32's 5V pin → move to a higher-current
adapter.

Cheap meters update only 2–3 times a second and may miss a brief dip. The reboot
itself is the more reliable symptom.

---

## Phase 1b addition

Once Phase 1 works, add the button — **one new thing**:

- [ ] Button leg 1 → GPIO 4, leg 2 → ESP32 GND.
- [ ] `pio run -e phase1b_button -t upload`
- [ ] Expect `Fingerbot Phase 1b ready.` and no motion until you press.

If the servo stops working at this point, flash `phase1_autopress` again. If that
still works, the fault is in the button wiring, not the servo — which is exactly why
the baseline is kept around.

---

## Phase 1c addition

**No wiring change at all** — identical circuit to 1b. Purely firmware.

- [ ] `pio run -e phase1c_toggle -t upload`
- [ ] Expect `Fingerbot Phase 1c ready. Press the button to toggle.`
- [ ] Each press alternates: `>> Turning ON` / `>> Turning OFF`, with the arm
      swinging to a *different* angle each way and returning to neutral between.

This is where you calibrate the three angles against the real switch. In
`src/phase1c_toggle.cpp`:

| Constant | Default | What to tune it for |
| --- | --- | --- |
| `REST_ANGLE` | 90 | arm clear of the rocker, touching nothing |
| `ON_ANGLE` | 60 | far enough to flip it one way |
| `OFF_ANGLE` | 120 | far enough to flip it the other way |

**If it turns on when you expected off, swap `ON_ANGLE` and `OFF_ANGLE`.** That's
the entire fix — the logic doesn't care which direction is which.
