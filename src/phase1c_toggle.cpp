/*
  DIY Fingerbot — Phase 1c firmware (ESP32 + servo + button, two-position)
  ------------------------------------------------------------------------
  Adds exactly ONE new thing over Phase 1b: the arm now moves to TWO different
  angles depending on whether it's turning the switch on or off, and it remembers
  which state it last set.

  This is the shape the real thing needs. A wall rocker isn't a button — you press
  the top for ON and the bottom for OFF — so "press" was never enough. Phase 3
  swaps the button for HomeKit and otherwise looks like this.

    pio run -e phase1c_toggle -t upload

  Each button press flips the switch the other way. Watch the serial log: it
  reports the state it believes the switch is in after every move.

  Wiring is IDENTICAL to Phase 1b — no new parts, nothing to re-plug.
  See WIRING.md.

    servo signal (orange/yellow) -> GPIO 18
    servo V+     (red)           -> +5V of a SEPARATE 5V supply  (NOT the ESP32)
    servo GND    (brown/black)   -> GND of that supply
    ESP32 GND    ---------------- -> GND of that supply   (COMMON GROUND — required)
    button leg 1                 -> GPIO 4
    button leg 2                 -> ESP32 GND

  Do NOT run the servo off the ESP32's 5V/VIN pin. See the SERVO BROWNOUT note in
  project.md.
*/

#include <Arduino.h>
#include <ESP32Servo.h>
#include "debounce.h"
#include "switch_state.h"

Servo fingerArm;
const int SERVO_PIN  = 18;
const int BUTTON_PIN = 4;      // avoid strapping pins (0, 2, 12, 15) for inputs

// ---- Adjustable settings ----
// REST is where the arm lives between actuations: neutral, touching nothing, so
// it never holds the rocker down. ON and OFF sit either side of it.
//
// Which angle means "on" depends entirely on how you mount the servo relative to
// the switch. If the first real test turns things the wrong way round, swap these
// two numbers — that's the whole fix, no logic changes.
const int REST_ANGLE = 90;     // neutral, mid-travel
const int ON_ANGLE   = 60;     // swings one way
const int OFF_ANGLE  = 120;    // swings the other way

const int HOLD_MS = 500;       // how long to stay at the press angle
const unsigned long DEBOUNCE_MS = 50;

Debouncer  button(DEBOUNCE_MS);
SwitchState wallSwitch;        // assumes OFF at boot — see switch_state.h

// Move the arm to `angle`, hold, then return to neutral.
void actuate(int angle) {
  fingerArm.write(angle);
  delay(HOLD_MS);
  fingerArm.write(REST_ANGLE);
  delay(200);                  // let it settle before reporting
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

  Serial.println("Fingerbot Phase 1c ready. Press the button to toggle.");
  Serial.println("Assuming the switch starts OFF.\n");
}

void loop() {
  // Active low: internal pull-up holds the pin HIGH, pressing shorts it to GND.
  bool pressed = (digitalRead(BUTTON_PIN) == LOW);

  if (button.update(pressed, millis())) {
    Action action = wallSwitch.toggle();

    switch (action) {
      case Action::TurnOn:
        Serial.println(">> Turning ON");
        actuate(ON_ANGLE);
        break;

      case Action::TurnOff:
        Serial.println(">> Turning OFF");
        actuate(OFF_ANGLE);
        break;

      case Action::None:
        // toggle() never returns this, but handle it so the compiler is happy
        // and so this switch stays correct when Phase 3 calls requestOn().
        Serial.println(">> Already in that state, not moving");
        break;
    }

    Serial.printf("   switch is now %s\n\n", wallSwitch.isOn() ? "ON" : "OFF");
  }
}
