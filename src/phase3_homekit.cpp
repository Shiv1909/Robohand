/*
  DIY Fingerbot — Phase 3 firmware (Siri / Apple Home via HomeSpan)
  ------------------------------------------------------------------
  "Hey Siri, turn on the fan" swings the arm.

  HomeSpan makes the ESP32 itself a **native HomeKit accessory** — no bridge, no
  hub purchase, no subscription, no cloud account. Apple Home talks straight to
  this board over your LAN.

    pio run -e phase3_homekit -t upload
    pio device monitor

  Adds exactly ONE new thing over Phase 1c: the trigger is now HomeKit instead of
  a pushbutton. The servo code and the state machine are unchanged — deliberately,
  so that if the arm misbehaves here, it is the network and not the mechanism.

  ---------------------------------------------------------------------------
  WIRING IS IDENTICAL TO PHASE 1b/1c. Nothing to re-plug. The button is simply
  unused — Siri replaces it. See WIRING.md.

    servo signal (orange/yellow) -> GPIO 18
    servo V+     (red)           -> +5V of a SEPARATE 5V supply  (NOT the ESP32)
    servo GND    (brown/black)   -> GND of that supply
    ESP32 GND    ---------------- -> GND of that supply   (COMMON GROUND — required)
  ---------------------------------------------------------------------------

  BEFORE FLASHING: copy include/secrets.h.example to include/secrets.h and put
  your WiFi details in it. That file is gitignored — this repo is public.

  2.4 GHz ONLY. The ESP32 cannot see 5 GHz. A router broadcasting one merged SSID
  with band-steering often fails to associate, and it reads as a firmware bug.

  FIRST BOOT: watch the serial monitor. HomeSpan prints its progress connecting to
  WiFi, then a pairing QR code and the setup code. In Apple Home: Add Accessory ->
  More options... -> the Fingerbot appears -> enter the code.
*/

#include <Arduino.h>
#include <ESP32Servo.h>
#include "HomeSpan.h"
#include "switch_state.h"
#include "secrets.h"

Servo fingerArm;
const int SERVO_PIN = 18;

// ---- Adjustable settings ----
// Same three angles as Phase 1c, and still PLACEHOLDERS. Phase 1d's calibration
// console finds the real ones against the actual switch. REST is neutral, touching
// nothing, so the arm never holds the rocker down.
//
// If Siri turns things the wrong way round, swap ON_ANGLE and OFF_ANGLE. That is
// the whole fix — the logic doesn't care which direction is which.
const int REST_ANGLE = 90;
const int ON_ANGLE   = 60;
const int OFF_ANGLE  = 120;

const int HOLD_MS = 500;   // how long to stay at the press angle

// Assumes the switch is OFF at boot, matching Characteristic::On(0) below.
// See the KNOWN LIMITATION note in switch_state.h about drift.
SwitchState wallSwitch;

// Move the arm to `angle`, hold, then return to neutral.
void actuate(int angle) {
  fingerArm.write(angle);
  delay(HOLD_MS);
  fingerArm.write(REST_ANGLE);
  delay(200);              // let it settle
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
    bool wantOn = power->getNewVal();

    // THIS is why switch_state.h is a class and not a bool. Home re-sends state
    // freely: opening the app, a scene firing, or two commands in quick
    // succession can all deliver "turn on" when it is already on. Actuating on a
    // redundant request would press an already-on rocker and turn the light OFF.
    // requestOn() returns Action::None in that case and we don't move.
    Action action = wallSwitch.requestOn(wantOn);

    switch (action) {
      case Action::TurnOn:
        Serial.println(">> Siri: turning ON");
        actuate(ON_ANGLE);
        break;

      case Action::TurnOff:
        Serial.println(">> Siri: turning OFF");
        actuate(OFF_ANGLE);
        break;

      case Action::None:
        Serial.println(">> Already in that state, not moving");
        break;
    }

    Serial.printf("   switch is now %s\n\n", wallSwitch.isOn() ? "ON" : "OFF");

    // NOTE: actuate() blocks for ~700 ms, so homeSpan.poll() pauses for that long.
    // Comfortably inside HomeKit's response window and fine for one accessory.
    // Revisit if this ever drives several servos.
    return true;
  }
};

void setup() {
  Serial.begin(115200);

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
  // Left as the placeholder, we say nothing and let HomeSpan's own setup take
  // over: type `W` in the serial monitor and it SCANS and lists the networks it
  // can see, then prompts for the password and stores it in flash.
  //
  // That scan is also the definitive 2.4 GHz test — the ESP32 has no 5 GHz radio,
  // so anything appearing in that list is by definition a network it can use.
  //
  // The guard matters: setWifiCredentials() writes to flash on every boot, so
  // calling it unconditionally with placeholders would wipe whatever `W` stored.
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
    new DEV_Fingerbot();
}

void loop() {
  // Handles WiFi, mDNS discovery, pairing and all HomeKit traffic. Must be called
  // continuously — never put a long delay() in loop() here.
  homeSpan.poll();
}
