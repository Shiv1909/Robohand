# Bill of Materials — DIY Fingerbot

Single consolidated order from **[Robu.in](https://robu.in/)** — everything below is
in stock there, so it ships as one parcel with one delivery. Robu ships free above
₹500, so shipping is effectively ₹0 on this order.

> **Prices are estimates (₹, Aug 2026).** Only the MG90S (₹239) and the capacitors
> were confirmed on vendor pages; the rest are typical current street prices.
> Treat totals as a budget, not a quote.

---

## ORDER THIS NOW

Everything needed to build and finish **Phase 1 + Phase 1b**. Nothing here is
speculative; nothing for Phase 4/5 is included.

Tick these off as you order / as they arrive.

**Core — skip any already ordered**

- [ ] **1. ESP32 DevKit, 38-pin** — ₹500 —
      [Robu](https://robu.in/product/esp32-38pin-development-board-wifibluetooth-ultra-low-power-consumption-dual-core/)
- [ ] **2. MG90S servo — must say 180°** — ₹239 —
      [Robu](https://robu.in/product/towerpro-mg90s-mini-digital-servo-motor-180-rotation-standard-quality/)
- [ ] **3. Breadboard, 830 points** — ₹110 —
      [Robu](https://robu.in/product/transparent-830-points-solderless-breadboard/)
- [ ] **4. Jumper wires, M-M + M-F** (2 sets) — ₹160 —
      [Robu](https://robu.in/product-category/dupont-cable/)

**Power + brownout fix + Phase 1b**

- [ ] **5. 5 V 3 A adapter**, 5.5×2.1 mm — ₹350 —
      [Robu](https://robu.in/product/orange-5v-3a-power-supply-adapter-charger-with-5-5mm-dc-plug/)
- [ ] **6. DC jack, female with wires** — ₹40 —
      [Robu](https://robu.in/product/dc-jack-connector-female-2-1mm-x-5-5mm-with-wire/)
- [ ] **7. 1000 µF 16 V leaded ×3** — ₹45 —
      [Robu](https://robu.in/product/16zlh1000mhfc10x16-rubycon-1000uf-16v-%C2%B120-plugind10xl16mm-aluminum-electrolytic-capacitors-leaded-rohs/)
- [ ] **8. Ceramic capacitor kit** — ₹150 —
      [Robu](https://robu.in/product/ceramic-capacitor-assorted-kit-30-kinds-from-2pf-0-1uf/)
- [ ] **9. Tactile pushbuttons ×20** — ₹75 —
      [Robu](https://robu.in/product/6x6x5mm-tactile-push-button-switch-pack-of-20/)

**Tools — not parts; droppable to save ₹510**

- [ ] **10. Multimeter (MAS830L)** — ₹450 —
      [Robu](https://robu.in/product/haoyue-mas830l-digital-handheld-multimeter-600-vac-dc/)
- [ ] **11. Logic level converter, 4-ch** — ₹60 —
      [Robu](https://robu.in/product/level-converter-4-channel/)

### If something is out of stock — verified alternates

**ESP32 DevKit**

- [Espressif ESP32-DevKitC-32E](https://robu.in/product/espressif-esp32-devkitc-esp32-wroom-3-2e-development-board/) — official Espressif, PCB antenna
- [ESP32-WROVER-E DevKitC](https://robu.in/product/esp32-wroom-e-esp32-devkitc-core-board-for-arduino/) — adds PSRAM, harmless here

> ⚠️ **Avoid the "U" and "UE" variants** (ESP32-WROOM-32**U**, 32**UE**). Those have
> a u.FL connector *instead of* a PCB antenna — no antenna in the box, so WiFi
> effectively won't work until you buy one. Easy to order by mistake; the listings
> look almost identical.

**MG90S servo**

- [MG90S 9g Servo Metal Gear](https://robu.in/product/mg90s-9g-servo-metal-gear/)
- [TowardPro MG90S 180°](https://robu.in/product/towardpro-mg90s-mini-digital-servo-2-2kg-0-08sec-13g/)
- [ACEBOTT MG90 180°](https://robu.in/product/acebott-mg90-metal-gear-micro-servo-motor-180-degrees-compatible-with-esp32-arduino/)

> ⚠️ Never the **360° continuous rotation** version — it cannot hold an angle.

**Breadboard**

- [MB102 830 points](https://robu.in/product/mb102-830-points-solderless-prototype-pcb-breadboard-high-quality/)
- [ZY-201 830 points](https://robu.in/product/zy-201-830-points-solderless-breadboard/)

**5 V adapter**

- [Orange 5V 3A, BIS approved](https://robu.in/product/orange-5v-3a-power-adapter-with-dc-plug-bis-approved)
- [Orange 5V 2A, dual pin DC](https://robu.in/product/orange-5v-2a-power-supply-with-dual-pin-dc-plug-adapter/) — 2 A is
  fine for one servo; only limits you when scaling to several

> ⚠️ **The plug must be 5.5 × 2.1 mm.** Robu also stocks a
> [Pro-Range 5V 3A with a 5.5 × **2.5** mm plug](https://robu.in/product/pro-range-5v-3a-power-adapter/) —
> that will not seat properly in the 2.1 mm pigtail jack (item 6) and gives an
> intermittent connection, which looks exactly like a brownout. Do not mix sizes.

### **Total: ₹2,179** — shipping free (Robu, above ₹500)

**Skip items 1–4 if already ordered → ₹1,170.**
**Skip 10–11 as well → ₹660 absolute minimum.**

Three checks before paying:

1. **MG90S must say "180° Rotation".** The 360° continuous-rotation version cannot
   hold an angle and would be useless — see the warning below.
2. **Capacitors must be leaded / through-hole**, not SMD. SMD won't fit a breadboard.
3. **Items 10 and 11 are tools/insurance, not parts.** Drop them to save ₹510 if you
   want the minimum — but read why the multimeter earns its place first.

---

## Group 1 — core parts

`project.md` records these as already ordered and in transit. **Skip any you've
already got**; listed here so the order is complete either way.

| Item | Spec | Qty | Link | ₹ |
| --- | --- | --- | --- | --- |
| ESP32 DevKit | ESP32-WROOM-32, 38-pin | 1 | [Robu](https://robu.in/product/esp32-38pin-development-board-wifibluetooth-ultra-low-power-consumption-dual-core/) | 500 |
| MG90S servo | metal gear, **180° — NOT 360°** | 1 | [Robu — TowerPro MG90S 180°](https://robu.in/product/towerpro-mg90s-mini-digital-servo-motor-180-rotation-standard-quality/) | 239 |
| Breadboard | 830 points, solderless | 1 | [Robu](https://robu.in/product/transparent-830-points-solderless-breadboard/) | 110 |
| Jumper wires | M-M **and** M-F, 40-pin sets | 2 | [Robu — Dupont cables](https://robu.in/product-category/dupont-cable/) | 160 |

**Subtotal: ₹1,009**

> ### ⚠️ Buy the 180° MG90S, not the 360° one
> Robu sells both, and the listings look nearly identical. The **360° continuous
> rotation** version cannot hold a position — it spins like a motor and takes speed
> commands, not angles. Every line of firmware in this project assumes a positional
> servo. The 360° version would be silently useless.

---

## Group 2 — power, brownout fix, and Phase 1b

| Item | Spec | Qty | Link | ₹ |
| --- | --- | --- | --- | --- |
| 5 V power adapter | 5 V **3 A**, 5.5×2.1 mm plug | 1 | [Robu — Orange 5V 3A](https://robu.in/product/orange-5v-3a-power-supply-adapter-charger-with-5-5mm-dc-plug/) | 350 |
| DC jack w/ pigtail | female 5.5×2.1 mm, wire leads | 1 | [Robu](https://robu.in/product/dc-jack-connector-female-2-1mm-x-5-5mm-with-wire/) | 40 |
| Bulk capacitor | **1000 µF 16 V, leaded** | 3 | [Robu — Rubycon 1000 µF 16 V](https://robu.in/product/16zlh1000mhfc10x16-rubycon-1000uf-16v-%C2%B120-plugind10xl16mm-aluminum-electrolytic-capacitors-leaded-rohs/) | 45 |
| Ceramic capacitors | assorted kit, incl. 0.1 µF | 1 | [Robu — 30-value kit](https://robu.in/product/ceramic-capacitor-assorted-kit-30-kinds-from-2pf-0-1uf/) | 150 |
| Tactile pushbuttons | 6×6×5 mm, 4-pin | 20-pk | [Robu — pack of 20](https://robu.in/product/6x6x5mm-tactile-push-button-switch-pack-of-20/) | 75 |

**Subtotal: ₹660**

### Why these, specifically

- **3 A, not 2 A.** MG90S stall is **750 mA ±10 % @ 6 V**, so 2 A covers one servo.
  But inrush spikes above steady stall, and the plan scales to several servos on one
  board. 3 A covers ~three with headroom. 5 V (not 6 V) keeps one rail voltage and
  sits safely inside the servo's 4.8–6.0 V range.
- **The DC jack has a wire pigtail, not screw terminals** — bare wires push straight
  into the breadboard rails, one less thing to assemble.
- **Capacitors must be LEADED, not SMD.** Most search hits for both are surface-mount,
  which you cannot use on a breadboard. Both links above are through-hole. Fit the
  1000 µF **across the servo's V+ and GND, close to the servo** — at the far end of a
  long wire it does much less good. **Polarity matters:** the stripe is negative;
  backwards, it can vent.
- **The ceramic kit** covers the 0.1 µF that pairs with the bulk cap (electrolytic
  handles slow bulk sag, ceramic handles high-frequency noise it can't) and gives you
  a drawer of values for everything after.
- **Buttons need no resistor** — the ESP32's internal pull-up handles it (GPIO 4).

> **Do NOT substitute an MB102 breadboard power module.** It's the obvious-looking
> choice and it will fail here — it sources only a few hundred mA and browns out
> exactly when the servo stalls, which is the precise failure you're engineering out.

---

## Group 3 — tools

| Item | Spec | Qty | Link | ₹ |
| --- | --- | --- | --- | --- |
| Digital multimeter | DC volts, DC current, continuity | 1 | [Robu — Haoyue MAS830L](https://robu.in/product/haoyue-mas830l-digital-handheld-multimeter-600-vac-dc/) | 450 |
| Logic level converter | 4-channel bidirectional 3.3 ↔ 5 V | 1 | [Robu — 4-channel](https://robu.in/product/level-converter-4-channel/) | 60 |

**Subtotal: ₹510**

- **The multimeter matters more than it sounds.** The brownout looks *identical* to a
  firmware bug — the board reboots mid-press and you'll be tempted to debug code. A
  meter on the 5 V rail settles it. It's also how you'll measure idle current before
  sizing a battery (see Section D). A [DT830D at ~₹200](https://robu.in/product/digital-multimeter-small-yellow-color-lcd-ac-dc-measuring-voltage-current/)
  works if you want to halve this line.
- **The level converter is insurance, hopefully unused.** The ESP32 drives 3.3 V logic
  while the MG90S expects ~5 V. Most MG90S units read 3.3 V PWM fine — but it's a
  marginal case, and "servo twitches or ignores commands" is what it looks like when
  it isn't. ₹60 against a confusing evening.

---

## Order total

| Group | ₹ |
| --- | --- |
| 1 — core (skip if already ordered) | 1,009 |
| 2 — power + Phase 1b | 660 |
| 3 — tools | 510 |
| Shipping (free above ₹500) | 0 |
| **Everything** | **₹2,179** |
| **If core already ordered** | **₹1,170** |

---

## What Phase 3 (Siri) needs — no extra hardware

HomeSpan makes the ESP32 itself a native HomeKit accessory. No bridge, no hub
purchase, no subscription. **The parts list above is already complete for Siri.**

What it does need, none of it purchasable:

- **A 2.4 GHz WiFi band.** The ESP32 cannot see 5 GHz at all. Routers that broadcast
  one merged SSID with band-steering frequently fail to pair, in a way that reads as
  a firmware bug. Be ready to expose a 2.4 GHz-only SSID temporarily.
- **An iPhone or iPad** with the Home app, to pair the accessory.
- **WiFi credentials in `include/secrets.h`** — gitignored, because this repo is public.

**A HomeKit hub (Apple TV / HomePod) is optional.** "Hey Siri, turn on the fan"
works from your iPhone with no hub while you're on the same WiFi. A hub is only
required for control from *outside* the house, and for automations. Don't buy one
for this project unless you specifically want those.

---

## Later — do NOT order yet

Each depends on a measurement or decision not yet made; buying now means buying twice.

- **3D printing filament** — **PETG, not PLA.** PLA creeps under sustained load and
  softens in a warm room, and this part holds force against a wall for months. Wait
  until the arm is designed (Phase 4).
- **Adhesive tab + tether material** — for the push-pull rocker mechanism (see
  `project.md`). Depends on rocker dimensions.
- **Servos 2..N** — one per extra switch. Order after one switch works end-to-end
  through Siri.
- **[PCA9685 16-channel servo driver](https://robu.in/product/16-channel-12-bit-pwmservo-driver-i2c-interface-pca9685-arduino-raspberry-pi/)**
  (~₹300) — for the multi-gang build. Drives 16 servos over 2 I2C pins with its own
  PWM hardware, and gives a clean place to feed servo power separately from logic.
  **This is what makes the board choice stop mattering** for servo count — see below.
- **A smaller board for the final build** — tempting, but do NOT buy one yet:
  - [ESP32-C3 SuperMini](https://robu.in/product/esp32-c3-development-board-esp32-supermini/)
    is 22.5 × 18 mm and would shrink a single-switch build to roughly SwitchBot size.
  - **But the C3 has only 6 LEDC PWM channels.** Six servos = exactly six, zero
    margin, before ESP32Servo reserves timers. The classic ESP32 has 16. The board
    that shrinks the product is the one that makes scaling harder — unless you use
    the PCA9685 above, which sidesteps it entirely.
  - **If you go Thread/Matter (D3), you need a C6, not a C3** — the C3 has no
    802.15.4 radio. Buying a C3 now silently commits you to WiFi.
  - The SuperMini form factor is widely reported to have **weak WiFi range** from its
    antenna layout, especially in clones. Risky for a device that must stay connected
    behind a switchboard.

  Order of operations: one switch working on the DevKitC → PCA9685 when scaling to
  six → pick a small board last, once WiFi-vs-Thread is decided.
- **Battery parts** — see Section D. Do the power-bank test first.

---

## Can we run it on cells?

Fine for bench testing, viable for the permanent install with WiFi light sleep.

### On the bench (Phase 1/1b) — works

Use **4 × AA NiMH** (Eneloop-type), *not* alkaline.

| | NiMH ×4 | Alkaline ×4 |
| --- | --- | --- |
| Pack voltage | 4.8 V (inside MG90S 4.8–6.0 V range) | 6.0 V nominal, ~6.4 V fresh |
| Internal resistance | ~20–50 mΩ/cell | ~150–300 mΩ/cell, rising as they drain |
| Sag at 750 mA stall | small | **0.5–1 V** |

Alkalines look better on the voltage spec and are the wrong answer — their internal
resistance recreates the exact brownout the separate supply exists to prevent.

Even so, prefer the adapter during bring-up: a pack whose voltage droops as it
discharges is a moving variable, and Phase 1 is exactly when the power supply should
be the one thing that *isn't* a suspect.

### Permanently (Phase 5) — yes, with WiFi light sleep

> **Correction.** An earlier version of this file said battery was impractical and
> quoted ~80 mA idle. That is the ESP32's **untuned default**, not its floor. With
> `esp_wifi_set_ps()` + auto light-sleep, measured idle is **~7.5 mA** at DTIM3 — a
> ~10× difference that changes the conclusion.

**The servo is not the power problem. Idle radio current is.**

| Load | Draw | Per day |
| --- | --- | --- |
| Servo presses (10/day, ~1 s each) | 750 mA peak | **~10 mWh** |
| ESP32 idle, WiFi + auto light-sleep | ~7.5 mA @ 3.3 V | **~600 mWh** |

Light sleep — not deep sleep — is the key. The radio still wakes on every DTIM
beacon, so the accessory stays reachable with sub-second latency. Deep sleep would
break HomeKit; light sleep does not.

**Estimated runtime on one 18650 (3000 mAh ≈ 11 Wh):**

| Configuration | Avg draw | Runtime |
| --- | --- | --- |
| Stock devkit + light sleep | ~20 mA | **~5–7 days** |
| Devkit with power LED + USB-UART disabled | ~8 mA | **~2 weeks** |
| ESP32-C6 + Matter over Thread (sleepy end device) | <1 mA | **months** |

> **The dev board is the hidden cost.** A bare ESP32 module hits ~7.5 mA, but a
> DevKitC also runs a CP2102/CH340 USB-serial chip and a power LED that never sleep —
> typically 10–20 mA on their own, more than the ESP32 itself. **Measure yours with
> the multimeter before sizing a battery.**

For comparison, a commercial SwitchBot Bot gets **600 days on a CR2** — about 55 µA
average. That is only possible over Bluetooth. No WiFi device comes close; it's a
physics problem, not a tuning problem.

---

## D. Portable power — order with Phase 5, not now

### D1. Power bank — simplest, zero new circuitry ⭐ start here

- [ ] Any USB power bank + the ESP32's existing USB cable.
- **The one gotcha:** most power banks **auto-shut-off below ~50–100 mA**, and a
  sleeping ESP32 pulls far less — so the bank switches itself off and your accessory
  dies. You need one advertised as **"low-current mode"** or **"always-on"**. Test
  before committing.

### D2. 18650 + charger + boost — proper portable build

| Item | Spec | Link | ₹ |
| --- | --- | --- | --- |
| Charger module | TP4056 **with protection** (DW01), Type-C | [Robu — TP4056 Type-C](https://robu.in/product/tp4056-1a-li-ion-lithium-battery-charging-module-with-current-protection-type-c/) | 60 |
| Boost converter | MT3608, 3.7 V → 5 V | [Robu — MT3608](https://robu.in/product/mt3608-2a-max-dc-dc-step-up-power-module-booster-power-module/) | 60 |
| 18650 cell | 3000 mAh, **genuine** Samsung/LG | buy from a reputable seller | 350 |

**Subtotal D2: ≈ ₹470**

- **Get the protected TP4056 (with DW01).** Li-ion below ~2.5 V is damaged and can be
  dangerous. The protection IC is ~₹10 of the price. Non-negotiable.
- **TP4056 modules don't load-share.** Running the circuit while charging confuses
  charge termination. Power the device off while charging.
- **MT3608's "2 A" is a headline number.** Boosting 3.7 V → 5 V at 750 mA stall means
  ~1.2 A on the input side, near its comfortable limit. The 1000 µF cap matters
  *more* here, not less.
- **18650 counterfeits are rampant.** Anything claiming 6000 mAh is fake — genuine
  cells top out near 3500 mAh.

### D3. Months, not weeks — Matter over Thread

- **ESP32-C6** or **ESP32-H2** (the classic ESP32 has no 802.15.4 radio).
- Thread allows a **Sleepy End Device** — sub-1 mA, months on a battery — while
  staying reachable to Siri.
- **Requires a Thread border router**: HomePod mini or Apple TV 4K. If you don't own
  one, that dwarfs the battery cost.
- Replaces HomeSpan with the Matter SDK. Substantially harder than Phase 3 as planned.

**Recommendation: D1 now, D2 for a tidy build, D3 only if weekly charging genuinely
fails the use case.**

---

## Measurements

### Received 2026-08-04

Photos of the target board. **6-gang modular switchboard**, single horizontal row,
rocker-style modules. Ample clear wall above and below — mounting space is not a
constraint.

| Measurement | Value | Confidence |
| --- | --- | --- |
| Module width | ~2 cm | plausible — matches the ~22 mm Indian modular standard |
| Module height | ~7 cm (AR) | **doubtful — see below** |
| Gang count | 6 | confirmed from photo |
| Gang pitch | ~2 cm (modules abut) | inferred, needs confirming |
| Mounting space | ample above and below | confirmed from photo |

> **The 7 cm reading looks wrong.** In the photos the rockers are roughly twice as
> tall as wide (~1 : 2). If width is 2 cm, height should be ~4–4.5 cm — which also
> matches the standard Indian module (~22 × 45 mm). The AR tape was probably
> measuring plate edge to plate edge. **Re-measure with a ruler.**

### Actuation force — NOT a blocker

The design space is one variable (arm length) over a narrow range (20–30 mm), so this
can be settled empirically.

**Print three arms and test:**

- [ ] Print arms at **20 mm, 25 mm, 30 mm**.
- [ ] Try **20 mm first** — shortest arm, maximum force (~9.8 N @ 6 V, above
      SwitchBot's 8 N).
- [ ] Go longer only if you need more travel at the rocker.

If even the 20 mm arm won't flip it, the servo is undersized and no geometry fixes
that — the answer is a stronger servo (MG996R). That outcome is itself the measurement.

Indian modular rockers are typically light (~2–4 N), so a 20 mm MG90S arm should have
healthy margin.

**How to measure it, if you want a real number anyway.**

*Method A — hanging weight. No equipment to buy, and it measures PULL force, which is
what the tether design needs.*

1. Stick a tab of strong tape (folded to leave a loop) on the rocker's lower half.
2. Tie a string to it and hang an empty sealed bottle.
3. Add water gradually — take the bottle down to fill, cap it, rehang.
4. When the rocker flips, measure the water in a jug. **1 ml = 1 g.** Add the
   bottle's own weight.

> ⚠️ Fill away from the board and keep the bottle capped. That's mains wiring behind
> the plate.

*Method B — kitchen scale + video.* Hold it sideways in the pressing position, tare to
zero, start a video of the display, press it into the rocker until it clicks, then
scrub back to the click frame. The video works around scales not holding a peak.

*Method C — luggage/fish scale (~₹200).* Hook onto the taped tab and pull.

**Converting:** newtons = grams ÷ 102.

| Reading | Force | Verdict |
| --- | --- | --- |
| < 400 g | < 3.9 N | easy — any arm 20–30 mm works |
| 400–800 g | 3.9–7.8 N | use a 20–25 mm arm |
| > 800 g | > 7.8 N | MG90S marginal — consider MG996R |

### Still useful

- [ ] Rocker **height × width** in mm, with a ruler.
- [ ] **Gang pitch** — centre-to-centre between adjacent modules, mm.
- [ ] Rocker **throw** — how far the end travels off→on, mm.
- [ ] How far the plate **protrudes from the wall**, mm.
- [ ] **Side-on photo** of one switch showing the rocker in both positions.
