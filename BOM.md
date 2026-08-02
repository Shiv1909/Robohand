# Bill of Materials — DIY Fingerbot

Shopping list and build inventory. Tick items as they arrive.

**About the links:** these are *search* links, not specific product pages. Listings
go dead and stock changes, and a specific item I can't verify is worse than a
search that always shows what's actually available. Match the **spec**, not a
particular seller. Swap the domain for your region.

---

## Already ordered / in transit

Per `project.md`. Don't re-order.

- [ ] ESP32 dev board (WiFi + BLE — needed for Siri in Phase 3)
- [ ] MG90S servo (metal gear)
- [ ] Breadboard
- [ ] Jumper wires (male-male + male-female)
- [ ] USB cable matching the ESP32 board

---

## A. Order now — these block Phase 1

Phase 1 is not safe to run without A1–A3. Everything else can wait.

### A1. 5 V DC regulated power supply — 3 A

- [ ] **Spec:** 5 V DC output, **≥ 2 A, 3 A recommended**. Regulated, not a raw
      "unregulated wall wart". Barrel plug **5.5 × 2.1 mm centre-positive** is the
      most common and matches A2.
- **Why 3 A when the servo needs less:** the MG90S datasheet lists stall current
  at **750 mA ±10 % @ 6 V**, but startup inrush spikes well above the steady
  figure, and the endgame is several servos on one switchboard. 3 A covers about
  three servos with headroom. A 2 A unit is fine for one servo today.
- **Why 5 V and not 6 V:** the MG90S is rated 4.8–6.0 V. 6 V gives slightly more
  torque (2.0 vs ~1.8 kg·cm) but 5 V is the safer, more available option and
  keeps everything on one rail voltage.
- 🔗 [Amazon search](https://www.amazon.com/s?k=5V+3A+regulated+power+supply+5.5x2.1mm+barrel)
  · [Robu.in](https://robu.in/?s=5v+3a+power+adapter)
  · [Adafruit](https://www.adafruit.com/?q=5V+3A+power+supply)

> **Do NOT substitute an MB102 breadboard power module.** They're the obvious-looking
> choice and they will not work here — they source only a few hundred mA and brown
> out exactly when the servo stalls, which is the precise failure you're trying to
> engineer out.

### A2. Barrel jack → screw terminal adapter

- [ ] **Spec:** female DC barrel jack, **5.5 × 2.1 mm**, screw terminal breakout.
      Must match the plug on A1.
- **Why:** gets you from the adapter's barrel plug to bare wires you can land on
  the breadboard rails. Buy 2 — they're cents and easy to lose.
- 🔗 [Amazon search](https://www.amazon.com/s?k=5.5x2.1mm+female+barrel+jack+screw+terminal+adapter)
  · [Robu.in](https://robu.in/?s=dc+jack+screw+terminal)

> **Alternative to A1 + A2:** a 5 V 3 A USB charger you already own, plus a
> **USB-A breakout board**. Same result, one less thing to buy, slightly messier.
> 🔗 [USB breakout search](https://www.amazon.com/s?k=USB+A+female+breakout+board+screw+terminal)

### A3. Bulk capacitor — 1000 µF, 16 V electrolytic

- [ ] **Spec:** 1000 µF (470 µF acceptable), **16 V or higher**, aluminium
      electrolytic, low-ESR preferred. Radial/through-hole.
- **Why:** absorbs the current spike when the servo stalls against the switch,
  so the spike is served by the capacitor instead of dragging the whole rail
  down. Fit it **across the servo's V+ and GND, physically close to the servo** —
  at the far end of a long wire it does much less good.
- **Polarity matters.** The stripe is the negative leg. Backwards, it can vent.
- Cheapest route is an assortment kit, which also covers A4.
- 🔗 [Amazon search](https://www.amazon.com/s?k=1000uF+16V+electrolytic+capacitor)
  · [Assortment kit](https://www.amazon.com/s?k=electrolytic+capacitor+assortment+kit)

### A4. Ceramic capacitor — 0.1 µF (100 nF)

- [ ] **Spec:** 0.1 µF / 100 nF ceramic, any voltage ≥ 16 V. Usually marked `104`.
- **Why:** sits in parallel with A3. The big electrolytic is slow and handles the
  bulk sag; this one handles high-frequency switching noise the electrolytic
  can't. Standard practice, costs nothing.
- Comes in any assortment kit.
- 🔗 [Amazon search](https://www.amazon.com/s?k=0.1uF+100nF+ceramic+capacitor+kit)

### A5. Tactile pushbuttons — Phase 1b trigger

- [ ] **Spec:** 6 × 6 mm through-hole tactile switch, 4-pin, breadboard-friendly.
      Buy a pack of 20+.
- **Why:** Phase 1b (`phase1b_button` firmware) needs a physical button on GPIO 4.
  No resistor needed — the ESP32's internal pull-up handles it.
- 🔗 [Amazon search](https://www.amazon.com/s?k=6x6mm+tactile+push+button+switch+kit)
  · [Robu.in](https://robu.in/?s=tactile+switch+6x6)

---

## B. Order now — cheap, and saves a second shipment

### B1. Digital multimeter

- [ ] **Spec:** auto-ranging, DC voltage, DC current (10 A range), continuity beep.
      An entry-level unit is completely fine.
- **Why this matters more than it sounds:** the brownout failure looks identical
  to a firmware bug — the board reboots mid-press and you'll be tempted to debug
  code. A multimeter across the 5 V rail during a press settles it in ten seconds
  by showing the voltage sag. Without one you're guessing.
- 🔗 [Amazon search](https://www.amazon.com/s?k=digital+multimeter+auto+ranging)

### B2. Logic level shifter — contingency, buy but hopefully don't need

- [ ] **Spec:** 4-channel bidirectional level converter module (3.3 V ↔ 5 V), or a
      **74AHCT125** if you want the technically-correct part.
- **Why:** the ESP32 drives 3.3 V logic, but the MG90S expects a ~5 V signal.
  Most MG90S units read 3.3 V PWM fine, so this probably won't be needed — but
  it's a marginal case, and "servo twitches or ignores commands" is exactly what
  it looks like when it isn't. Cheap insurance against a confusing evening.
- 🔗 [Amazon search](https://www.amazon.com/s?k=4+channel+logic+level+converter+3.3v+5v)

---

## C. Later — do NOT order yet

Deliberately deferred. Each depends on a measurement or decision that hasn't been
made, and buying now means buying twice.

- **3D printing filament** — **PETG, not PLA.** PLA creeps under sustained load
  and softens in a warm room, and this is a part that holds force against a wall
  for months. Wait until the arm is designed (Phase 4) so you know how much you
  need.
- **Mounting adhesive** — 3M VHB tape or Command strips. Depends on the housing
  size, which depends on the measurements below.
- **Battery + charging (Phase 5)** — 18650 + TP4056 + boost converter, *or* just
  a USB wall adapter. Genuinely undecided: a servo stalling against a switch is
  power-hungry, and permanent USB power may beat battery swaps. Decide after
  Phase 4.
- **Servos 2..N** — one per additional switch. Order after one switch works
  end-to-end through Siri.

---

## Measurements needed

I need these before Phase 4 CAD, and they also settle whether the MG90S is
strong enough at all. Rough numbers beat no numbers — a kitchen scale is fine.

### 1. Actuation force ⭐ most important

- [ ] Press a **kitchen scale** against the switch rocker until it flips. Record
      the **peak reading in grams**. Do it three times, keep the highest.
- Determines the maximum usable arm length. Torque is force × distance, so a long
  arm is a weak arm — too long and it stalls instead of flipping.

### 2. Throw distance

- [ ] How far the rocker tip travels between fully-off and fully-on, in **mm**.
      Ruler is fine, calipers better.
- Sets the servo sweep angle.

### 3. Rocker geometry

- [ ] Rocker face **height × width** in mm.
- [ ] Which end you press for ON (top or bottom).

### 4. Gang pitch

- [ ] **Centre-to-centre distance between adjacent switches**, in mm.
- Needed for the multi-servo housing later. Cheap to measure now while you're there.

### 5. Clearance

- [ ] How far the switch plate **protrudes from the wall** (mm).
- [ ] Flat wall space **above / below / beside** the board available for mounting (mm).
- [ ] Anything in the way — door frame, furniture, trim?

### 6. Photos ⭐ nearly as useful as the numbers

- [ ] Straight-on shot of the switchboard **with a ruler in frame**.
- [ ] Side-on shot showing the rocker in both positions.

Drop the photos in this folder and I'll work from them.
