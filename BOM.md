# Bill of Materials — DIY Fingerbot

Shopping list and build inventory for a build in **India**. Tick items as they arrive.

> **Prices are estimates (₹, Aug 2026) and shipping is extra.** Only the capacitor
> price below was confirmed on the vendor page; the rest are typical current
> street prices for these exact parts. Verify at checkout — treat the totals as a
> budget, not a quote.

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

| # | Item | Spec | Qty | Link | ₹ |
| --- | --- | --- | --- | --- | --- |
| A1 | 5 V power adapter | 5 V **3 A**, regulated, 5.5×2.1 mm plug | 1 | [Robu — Orange 5V 3A](https://robu.in/product/orange-5v-3a-power-supply-adapter-charger-with-5-5mm-dc-plug/) | 350 |
| A2 | Barrel → screw terminal | Female 5.5×2.1 mm to screw terminal | 1 | [Sunrom #5908](https://www.sunrom.com/p/female-dc-power-adapter-55x21mm-plug-to-screw-terminal) | 30 |
| A3 | Bulk capacitor | **1000 µF 16 V, leaded** (not SMD) | 3 | [Sunrom — Samwha 10 mm](https://www.sunrom.com/p/1000uf-16v-10mm-samwha) | 34 |
| A4 | Ceramic capacitor | 0.1 µF / 104, through-hole | 5-pk | [ElectronicsComp — 0.1 µF 104](https://www.electronicscomp.com/0.1uf-104-ceramic-capacitor) | 10 |
| A5 | Tactile pushbuttons | 6×6×5 mm, 4-pin, breadboard pitch | 20-pk | [Robu — pack of 20](https://robu.in/product/6x6x5mm-tactile-push-button-switch-pack-of-20/) | 75 |

**Subtotal A: ₹499**

### Why these, specifically

- **A1 — 3 A, not 2 A.** The MG90S datasheet lists stall at **750 mA ±10 % @ 6 V**,
  so 2 A would cover one servo. But inrush spikes above steady stall, and the plan
  scales to several servos on one switchboard. 3 A covers ~three with headroom.
  5 V (not 6 V) keeps everything on one rail and stays safely inside the servo's
  4.8–6.0 V range.
- **A3 — buy 3, and make sure they're LEADED.** Most search hits for this part are
  SMD, which you cannot use on a breadboard. The linked Samwha is a radial
  through-hole part. Fit it **across the servo's V+ and GND, close to the servo** —
  at the far end of a long wire it does much less good. **Polarity matters:** the
  stripe is negative; backwards, it can vent.
- **A4 — sits in parallel with A3.** The electrolytic is slow and handles bulk sag;
  this one handles high-frequency noise it can't. Standard practice, costs nothing.
- **A5 — no resistor needed.** The ESP32's internal pull-up handles it
  (`phase1b_button` uses GPIO 4).

> **Do NOT substitute an MB102 breadboard power module.** It's the obvious-looking
> choice and it will fail here — it sources only a few hundred mA and browns out
> exactly when the servo stalls, which is the precise failure you're engineering out.

---

## B. Order now — cheap, and saves a second shipment

| # | Item | Spec | Qty | Link | ₹ |
| --- | --- | --- | --- | --- | --- |
| B1 | Digital multimeter | DC volts, DC current, continuity buzzer | 1 | [Robocraze — Mastech MAS830L](https://robocraze.com/products/original-mastech-mas830l-digital-pocket-multimeter-with-probes) | 550 |
| B2 | Logic level converter | 4-channel bidirectional 3.3 V ↔ 5 V | 1 | [Robu — 4-channel](https://robu.in/product/level-converter-4-channel/) | 60 |

**Subtotal B: ₹610**

- **B1 matters more than it sounds.** The brownout looks *identical* to a firmware
  bug — the board reboots mid-press and you'll be tempted to debug code. A meter on
  the 5 V rail settles it. A basic DT830D at ~₹200 also works if you want to halve
  this line; the Mastech is just more pleasant and more trustworthy long-term.
- **B2 is insurance, hopefully unused.** The ESP32 drives 3.3 V logic while the
  MG90S expects ~5 V. Most MG90S units read 3.3 V PWM fine, so you probably won't
  need it — but it's a marginal case, and "servo twitches or ignores commands" is
  exactly what it looks like when it isn't. ₹60 against a confusing evening.

---

## Cost summary

| Group | ₹ |
| --- | --- |
| A — blocks Phase 1 | 499 |
| B — recommended | 610 |
| **Parts total** | **1,109** |
| Shipping (see below) | 150–300 |
| **Estimated all-in** | **≈ ₹1,250 – 1,400** |

### Shipping — worth 2 minutes of your time

The list above is "best part per item", which spreads across **four vendors**
(Robu, Sunrom, ElectronicsComp, Robocraze) — so you'd pay shipping four times, and
that's a big fraction of a ₹1,100 order.

Cheaper in practice: **consolidate onto Robu.in**, which stocks equivalents of
everything here and ships free above ₹500. You'd trade the exact Sunrom capacitor
for their [Rubycon 1000 µF 16 V leaded](https://robu.in/product/16zlh1000mhfc10x16-rubycon-1000uf-16v-%C2%B120-plugind10xl16mm-aluminum-electrolytic-capacitors-leaded-rohs/)
(a better capacitor, slightly dearer) and pick their multimeter and 0.1 µF caps.
Likely lands in the same ₹1,100–1,300 range all-in, with one delivery.

---

## C. Later — do NOT order yet

Each depends on a measurement or decision not yet made; buying now means buying twice.

- **3D printing filament** — **PETG, not PLA.** PLA creeps under sustained load and
  softens in a warm room, and this part holds force against a wall for months. Wait
  until the arm is designed (Phase 4) so you know how much you need.
- **Mounting adhesive** — 3M VHB or Command strips. Depends on housing size, which
  depends on the measurements below.
- **Battery + charging (Phase 5)** — **probably don't. Use mains USB.** See below;
  the arithmetic is not close.
- **Servos 2..N** — one per extra switch. Order after one switch works end-to-end
  through Siri.

---

## Can we run it on cells?

Fine for bench testing, bad for the permanent install — and not for the expected reason.

### On the bench (Phase 1/1b) — works

Use **4 × AA NiMH** (Eneloop-type), *not* alkaline. Holder:
[Robocraze 4-cell AA holder](https://robocraze.com/products/aa-4-cell-battery-holder-hard-plastic-6v-battery-case) (~₹50).

| | NiMH ×4 | Alkaline ×4 |
| --- | --- | --- |
| Pack voltage | 4.8 V (inside MG90S 4.8–6.0 V range) | 6.0 V nominal, ~6.4 V fresh |
| Internal resistance | ~20–50 mΩ/cell | ~150–300 mΩ/cell, rising as they drain |
| Sag at 750 mA stall | small | **0.5–1 V** |

Alkalines look better on the voltage spec and are the wrong answer — their internal
resistance recreates the exact brownout the separate supply exists to prevent, and
it worsens as the cells age.

Even so, prefer the adapter during bring-up: a pack whose voltage droops as it
discharges is a moving variable, and Phase 1 is exactly when the power supply should
be the one thing that *isn't* a suspect.

### Permanently (Phase 5) — no

**The servo is not the power problem. The always-on WiFi is.**

| Load | Draw | Per day |
| --- | --- | --- |
| Servo presses (10/day, ~1 s each) | 750 mA peak | **~2 mAh** |
| ESP32 idle, WiFi connected | ~80 mA continuous | **~1900 mAh** |

The servo is ~0.1 % of the energy budget. On 4 × AA NiMH (2000 mAh) that's **about
one day** per charge. Aggressive modem-sleep/DTIM tuning might average ~25 mA and
reach ~3 days — still a device you service twice a week.

Deep sleep is not an escape: HomeKit requires the accessory to stay reachable, so
the radio stays up.

**Conclusion: run the permanent install from mains USB.** If a wire to the board is
genuinely unacceptable, that's a reason to reconsider the approach (a BLE/Thread
accessory that can sleep), not to add batteries.

---

## Measurements needed

Needed before Phase 4 CAD, and they settle whether the MG90S is strong enough at
all. Rough numbers beat no numbers — a kitchen scale is fine.

### 1. Actuation force ⭐ most important

- [ ] Press a **kitchen scale** against the switch rocker until it flips. Record the
      **peak reading in grams**. Three times, keep the highest.
- Sets the maximum usable arm length. Torque is force × distance, so a long arm is a
  weak arm — too long and it stalls instead of flipping.

### 2. Throw distance

- [ ] How far the rocker tip travels between fully-off and fully-on, in **mm**.
- Sets the servo sweep angle.

### 3. Rocker geometry

- [ ] Rocker face **height × width** in mm.
- [ ] Which end you press for ON (top or bottom).

### 4. Gang pitch

- [ ] **Centre-to-centre distance between adjacent switches**, in mm.
- Needed for the multi-servo housing later. Cheap to measure now.

### 5. Clearance

- [ ] How far the switch plate **protrudes from the wall** (mm).
- [ ] Flat wall space **above / below / beside** the board (mm).
- [ ] Anything in the way — door frame, furniture, trim?

### 6. Photos ⭐ nearly as useful as the numbers

- [ ] Straight-on shot of the switchboard **with a ruler in frame**.
- [ ] Side-on shot showing the rocker in both positions.

Drop the photos in this folder and I'll work from them.
