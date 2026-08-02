/*
  DIY Fingerbot — Phase 1b firmware (ESP32 + servo + pushbutton)
  --------------------------------------------------------------
  Adds exactly ONE new thing over Phase 1: a physical button as the trigger.
  The 3-second auto-loop is gone — the arm only moves when you press the button.

  Flash Phase 1 FIRST and confirm the servo moves. Only then flash this. That
  way, if the arm stops moving here, the servo half is already ruled out and the
  new variable is the GPIO input.

    pio run -e phase1b_button -t upload

  This file is deliberately standalone — it duplicates the servo code from
  phase1_autopress.cpp rather than sharing a header. That's on purpose: editing
  shared code while chasing a bug here would silently change the baseline you
  fall back to.

  Wiring:
    servo signal (orange/yellow) -> GPIO 18
    servo V+     (red)           -> +5V of a SEPARATE 5V supply  (NOT the ESP32)
    servo GND    (brown/black)   -> GND of that supply
    ESP32 GND    ---------------- -> GND of that supply   (COMMON GROUND — required)

    button leg 1                 -> GPIO 4
    button leg 2                 -> ESP32 GND

  The button needs no resistor — GPIO 4's internal pull-up holds the pin HIGH,
  and pressing the button shorts it to GND. So the pin reads LOW when pressed.
  That's "active low", and it's why the check below is `== LOW`.

  Do NOT run the servo off the ESP32's 5V/VIN pin. An MG90S stalling against a
  stiff switch spikes current hard enough to brown out the board mid-press.
  See the SERVO BROWNOUT note in project.md.
*/

#include <Arduino.h>
#include <ESP32Servo.h>
#include "debounce.h"

Servo fingerArm;
const int SERVO_PIN  = 18;
const int BUTTON_PIN = 4;      // avoid strapping pins (0, 2, 12, 15) for inputs

// ---- Adjustable settings ----
const int REST_ANGLE  = 0;     // arm retracted, not touching the switch
const int PRESS_ANGLE = 90;    // how far the finger swings to press
const int HOLD_MS     = 800;   // how long to hold the press

const unsigned long DEBOUNCE_MS = 50;  // raise if one push fires two presses

// The debounce logic itself lives in include/debounce.h, deliberately free of
// any Arduino calls, so it can be unit-tested on a PC with no ESP32 attached:
//   pio test -e native
Debouncer button(DEBOUNCE_MS);

void pressSwitch() {
  Serial.println(">> Pressing");
  fingerArm.write(PRESS_ANGLE);
  delay(HOLD_MS);
  Serial.println(">> Releasing");
  fingerArm.write(REST_ANGLE);
  Serial.println(">> Done.\n");
}

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

  Serial.println("Fingerbot Phase 1b ready. Press the button to fire.\n");
}

void loop() {
  // Active low: the internal pull-up holds the pin HIGH, and pressing the button
  // shorts it to GND. Normalise that to a plain "is it pressed" boolean here, so
  // the Debouncer never has to care about wiring polarity.
  bool pressed = (digitalRead(BUTTON_PIN) == LOW);

  if (button.update(pressed, millis())) {
    pressSwitch();
  }

  // Note: pressSwitch() blocks for HOLD_MS, so button pushes during a press are
  // ignored rather than queued. That's intended — one push, one press.
}
