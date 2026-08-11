/*
  DIY Fingerbot — Phase 3 firmware (Siri / Apple Home via HomeSpan)
  ------------------------------------------------------------------
  "Hey Siri, turn on the fan" swings the arm. The pushbutton still works too.

  HomeSpan makes the ESP32 itself a **native HomeKit accessory** — no bridge, no
  hub purchase, no subscription, no cloud account. Apple Home talks straight to
  this board over your LAN.

    pio run -e phase3_homekit -t upload
    pio device monitor

  TWO WAYS TO TRIGGER IT, and they stay in agreement:

    Siri / Home app  ->  update()          -> actuate -> HomeKit already knows
    Pushbutton       ->  loop()            -> actuate -> setVal() TELLS HomeKit

  That `setVal()` is the whole point of having the button here. Without it, you'd
  press the button, the light would change, and Apple Home would still show the
  old state — so the next Siri command would be a no-op and look broken.

  ---------------------------------------------------------------------------
  WIRING IS IDENTICAL TO PHASE 1b/1c. Nothing to re-plug. See WIRING.md.

    servo signal (orange/yellow) -> GPIO 18
    servo V+     (red)           -> +5V of a SEPARATE 5V supply  (NOT the ESP32)
    servo GND    (brown/black)   -> GND of that supply
    ESP32 GND    ---------------- -> GND of that supply   (COMMON GROUND — required)
    button leg 1                 -> GPIO 4     (wire it DIAGONALLY, see WIRING.md)
    button leg 2                 -> ESP32 GND / the `−` rail
  ---------------------------------------------------------------------------

  BEFORE FLASHING: copy include/secrets.h.example to include/secrets.h and put
  your WiFi details in it. That file is gitignored — this repo is public.

  2.4 GHz ONLY. The ESP32 cannot see 5 GHz. A router broadcasting one merged SSID
  with band-steering often fails to associate, and it reads as a firmware bug.

  FIRST BOOT: watch the serial monitor. HomeSpan prints its progress connecting to
  WiFi, then the pairing details. In Apple Home: Add Accessory -> More options...
  -> the Fingerbot appears -> enter the code.

  STILL UNSOLVED: if somebody flips the switch BY HAND, nothing here can know. The
  button path below is not a fix for that — it only covers changes this device
  makes itself. See the KNOWN LIMITATION note in switch_state.h.
*/

#include <Arduino.h>
#include <ESP32Servo.h>
#include "HomeSpan.h"
#include "debounce.h"
#include "switch_state.h"
#include "secrets.h"

Servo fingerArm;
const int SERVO_PIN  = 18;
const int BUTTON_PIN = 4;      // avoid strapping pins (0, 2, 12, 15) for inputs

// ---- Adjustable settings ----
// Same three angles as Phase 1c, and still PLACEHOLDERS. Phase 1d's calibration
// console finds the real ones against the actual switch. REST is neutral, touching
// nothing, so the arm never holds the rocker down.
//
// If Siri turns things the wrong way round, swap ON_ANGLE and OFF_ANGLE. That is
// the whole fix — the logic doesn't care which direction is which.
//
// KEEP THESE IN STEP WITH phase1c_toggle.cpp.
const int REST_ANGLE = 90;
const int ON_ANGLE   = 60;
const int OFF_ANGLE  = 120;

const int HOLD_MS = 500;               // how long to stay at the press angle
const unsigned long DEBOUNCE_MS = 50;  // proven at 14 presses for 14 pushes

Debouncer   button(DEBOUNCE_MS);
SwitchState wallSwitch;   // assumes OFF at boot — matches Characteristic::On(0)

// Move the arm to `angle`, hold, then return to neutral.
void actuate(int angle) {
  fingerArm.write(angle);
  delay(HOLD_MS);
  fingerArm.write(REST_ANGLE);
  delay(200);              // let it settle
}

// Carry out whatever the state machine decided, whoever asked for it.
void applyAction(const char *source, Action action) {
  switch (action) {
    case Action::TurnOn:
      Serial.printf(">> %s: turning ON\n", source);
      actuate(ON_ANGLE);
      break;

    case Action::TurnOff:
      Serial.printf(">> %s: turning OFF\n", source);
      actuate(OFF_ANGLE);
      break;

    case Action::None:
      Serial.printf(">> %s: already in that state, not moving\n", source);
      break;
  }

  Serial.printf("   switch is now %s\n\n", wallSwitch.isOn() ? "ON" : "OFF");
}

// The HomeKit accessory itself. Appears in Apple Home as a switch you can tap,
// ask Siri about, and put in scenes and automations.
struct DEV_Fingerbot : Service::Switch {
  SpanCharacteristic *power;

  DEV_Fingerbot() : Service::Switch() {
    // Initial value 0 = off, so HomeKit's idea of the state matches
    // SwitchState's cold-boot assumption. Keep these two in step.
    power = new Characteristic::On(0);
  }

  // Called whenever Home asks for a change — a tap, Siri, a scene, an automation.
  boolean update() {
    // THIS is why switch_state.h is a class and not a bool. Home re-sends state
    // freely: opening the app, a scene firing, or two commands in quick
    // succession can all deliver "turn on" when it is already on. Actuating on a
    // redundant request would press an already-on rocker and turn the light OFF.
    // requestOn() returns Action::None in that case and we don't move.
    //
    // Observed in the wild 2026-08-11, within 100 s of first pairing.
    applyAction("HomeKit", wallSwitch.requestOn(power->getNewVal()));

    // NOTE: actuate() blocks for ~700 ms, so homeSpan.poll() pauses for that long.
    // Comfortably inside HomeKit's response window and fine for one accessory.
    // Revisit if this ever drives several servos.
    return true;
  }

  // Called from loop() on a debounced physical button press.
  void toggleFromButton() {
    applyAction("Button", wallSwitch.toggle());

    // Push the new state up to HomeKit so Apple Home reflects reality. Skip this
    // and Home goes stale: it would still believe the old state, so the next Siri
    // command would resolve to Action::None and do nothing, which looks broken.
    power->setVal(wallSwitch.isOn());
  }
};

DEV_Fingerbot *fingerbot = nullptr;

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // IMPORTANT: ESP32Servo needs its PWM timers allocated BEFORE attach(),
  // or the servo silently never moves. Do not remove these four lines.
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  fingerArm.setPeriodHertz(50);            // standard 50 Hz servo
  fingerArm.attach(SERVO_PIN, 500, 2400);  // min/max pulse width (µs)
  fingerArm.write(REST_ANGLE);

  // Credentials and pairing code must be set BEFORE begin().
  //
  // Only push the hardcoded SSID if secrets.h has actually been filled in.
  // Left as placeholders, we say nothing and let HomeSpan's own setup take over:
  // type `W` in the serial monitor and it SCANS and lists the networks it can
  // see, then prompts for the password and stores it in flash.
  //
  // That scan is also the definitive 2.4 GHz test — the ESP32 has no 5 GHz radio,
  // so anything appearing in that list is by definition a network it can use.
  //
  // Both must be filled in — a real SSID with a placeholder password would fail
  // to associate AND overwrite anything `W` had stored, which is a nasty loop.
  if (strcmp(WIFI_SSID, "PUT-YOUR-2.4GHZ-SSID-HERE") != 0 &&
      strcmp(WIFI_PASSWORD, "PUT-YOUR-WIFI-PASSWORD-HERE") != 0) {
    homeSpan.setWifiCredentials(WIFI_SSID, WIFI_PASSWORD);
  }

  homeSpan.setPairingCode(HOMEKIT_PAIRING_CODE);

  homeSpan.begin(Category::Switches, "Fingerbot");

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Manufacturer("DIY");
      new Characteristic::Model("Fingerbot MG90S");
    fingerbot = new DEV_Fingerbot();
}

void loop() {
  // Handles WiFi, mDNS discovery, pairing and all HomeKit traffic. Must be called
  // continuously — never put a long delay() in loop() here.
  homeSpan.poll();

  // Active low: internal pull-up holds the pin HIGH, pressing shorts it to GND.
  // Same Debouncer as Phase 1b/1c, proven on this hardware at 14 for 14.
  if (button.update(digitalRead(BUTTON_PIN) == LOW, millis())) {
    fingerbot->toggleFromButton();
  }
}
