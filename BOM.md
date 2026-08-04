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

### Permanently (Phase 5) — yes, with WiFi light sleep

> **Correction.** An earlier version of this file said battery was impractical and
> quoted ~80 mA idle. That is the ESP32's **untuned default**, not its floor. With
> `esp_wifi_set_ps()` + auto light-sleep, measured idle is **~7.5 mA** at DTIM3.
> That is a ~10× difference and it changes the conclusion. Portable is viable.

**The servo is not the power problem — it never was. Idle radio current is.**

| Load | Draw | Per day |
| --- | --- | --- |
| Servo presses (10/day, ~1 s each) | 750 mA peak | **~10 mWh** |
| ESP32 idle, WiFi + auto light-sleep | ~7.5 mA @ 3.3 V | **~600 mWh** |

The servo is under 2 % of the budget. Runtime is set almost entirely by idle draw.

Light sleep — not deep sleep — is the key. The radio still wakes on every DTIM
beacon, so the accessory stays reachable with sub-second latency. Deep sleep would
break HomeKit; light sleep does not.

**Estimated runtime on one 18650 (3000 mAh ≈ 11 Wh):**

| Configuration | Avg draw | Runtime |
| --- | --- | --- |
| Stock devkit + light sleep | ~20 mA | **~5–7 days** |
| Devkit with power LED + USB-UART chip disabled | ~8 mA | **~2 weeks** |
| ESP32-C6 + Matter over Thread (sleepy end device) | <1 mA | **months** |

> **The dev board is the hidden cost.** A bare ESP32 module hits ~7.5 mA, but a
> DevKitC also runs a CP2102/CH340 USB-serial chip and a power LED that never
> sleep — typically 10–20 mA on their own, more than the ESP32 itself. Cutting the
> LED trace and unpowering the USB chip roughly doubles runtime. **Measure yours
> with the multimeter (B1) before sizing a battery** — board variants differ wildly.

**Verdict: portable works. Budget a recharge every 1–2 weeks.** For months-long
life you need Thread, not WiFi — see Section D.

---

## D. Portable power — order with Phase 5, not now

Three routes. Pick one; don't buy all three.

### D1. Power bank — simplest, zero new circuitry ⭐ start here

- [ ] Any USB power bank + the ESP32's existing USB cable.
- **Why start here:** proves the whole portable idea with no soldering and no new
  failure modes. If a weekly recharge is acceptable, you may never need D2.
- **The one gotcha:** most power banks **auto-shut-off below ~50–100 mA draw**, and
  a sleeping ESP32 pulls far less than that — so the bank switches itself off and
  your accessory dies. You need one advertised as **"low-current mode"**,
  **"always-on"**, or intended for trickle devices. Test before committing.

### D2. 18650 + charger + boost — proper portable build

| # | Item | Spec | Link | ₹ |
| --- | --- | --- | --- | --- |
| D2a | Charger module | TP4056 **with protection** (DW01), Type-C | [Robocraze — TP4056 Type-C w/ protection](https://robocraze.com/products/tp4056-battery-charger-c-type-module-with-protection-1) | 60 |
| D2b | Boost converter | MT3608, 3.7 V → 5 V, 2 A max | [Robu — MT3608](https://robu.in/product/mt3608-2a-max-dc-dc-step-up-power-module-booster-power-module/) | 60 |
| D2c | 18650 cell | 3000 mAh, **genuine** Samsung/LG/Sony | buy locally — see warning | 350 |
| D2d | Cell holder | 1× 18650, wire leads | [Robocraze — 18650 shield](https://robocraze.com/products/18650-lithium-battery-holder-shield-module-micro-usb) | 40 |

**Subtotal D2: ≈ ₹510**

Notes that will save you a bad evening:

- **Get the protected TP4056 (with DW01), not the bare charger.** Li-ion cells
  discharged below ~2.5 V are damaged, and can be dangerous. The protection IC is
  ~₹10 of the price. Non-negotiable.
- **TP4056 modules do not do load-sharing.** Running the circuit while charging
  confuses charge termination. Either power the device off while charging, or buy
  a module that explicitly supports load sharing.
- **MT3608 at 2 A is a headline number.** Boosting 3.7 V → 5 V at 750 mA stall
  means ~1.2 A on the input side, which is near its comfortable limit. **The
  1000 µF capacitor (A3) matters more here than on a wall adapter**, not less.
- **18650 counterfeits are rampant.** A cell claiming 6000 mAh is fake — genuine
  18650s top out around 3500 mAh. Buy from a reputable seller and expect ~₹350 for
  a real 3000 mAh cell.

### D3. Months, not weeks — Matter over Thread

Different board, different protocol, much more firmware work.

- **ESP32-C6** or **ESP32-H2** (the classic ESP32 has no 802.15.4 radio, so your
  current board cannot do this).
- Thread lets the device be a **Sleepy End Device** — sub-1 mA average, months to a
  year on the same battery — while staying reachable to Siri.
- **Requires a Thread border router**: HomePod mini or Apple TV 4K. If you don't
  own one, that's a far bigger cost than the battery.
- Replaces HomeSpan with the Matter SDK. Substantially harder than Phase 3 as
  currently planned.

**Recommendation: do D1 now, D2 if you want a tidy integrated build, and only
consider D3 if weekly charging genuinely fails the use case.**

---

## Measurements needed

Needed before Phase 4 CAD, and they settle whether the MG90S is strong enough at
all. Rough numbers beat no numbers — a kitchen scale is fine.

### Received 2026-08-04

Photos of the target board. **6-gang modular switchboard**, single horizontal row,
rocker-style modules (Anchor/Roma-type). Plenty of clear flat wall above and below —
mounting space is not a constraint.

| Measurement | Value | Confidence |
| --- | --- | --- |
| Module width | ~2 cm | plausible — matches the ~22 mm Indian modular standard |
| Module height | ~7 cm (AR) | **doubtful — see below** |
| Gang count | 6 | confirmed from photo |
| Gang pitch | ~2 cm (≈ module width, modules abut) | inferred, needs confirming |
| Mounting space | ample above and below | confirmed from photo |

> **The 7 cm reading looks wrong.** In the photos the rockers are roughly twice as
> tall as they are wide (~1 : 2). If the width is genuinely 2 cm, the height should
> be ~4–4.5 cm, which also matches the standard Indian modular module (~22 × 45 mm).
> 7 cm would make it 1 : 3.5, which the photo does not show. The AR tape was
> probably measuring plate edge to plate edge rather than the rocker face.
> **Re-measure the rocker face with a ruler.**

### Still needed

#### 1. Actuation force ⭐ still the blocker

- [ ] Press a **kitchen scale** against the rocker until it flips. Record the
      **peak reading in grams**. Three times, keep the highest.
- Nothing about the arm can be finalised without this. Torque is force × distance,
  so it sets the maximum arm length directly.

#### 2. Throw distance

- [ ] How far the rocker end travels between fully-off and fully-on, in **mm**.
- Sets the servo sweep angle.

#### 3. Rocker face — re-measure with a ruler

- [ ] Rocker **height × width** in mm (the AR numbers above are suspect).
- [ ] Confirm gang pitch: **centre-to-centre between two adjacent modules**, mm.

#### 4. Clearance

- [ ] How far the switch plate **protrudes from the wall**, mm.
- [ ] How far the rocker face sits **proud of the plate**, mm.

#### 5. Side-on photo

- [ ] Side view of one switch showing the rocker in **both** positions. Needed to
      see the pivot and how far each end moves.
