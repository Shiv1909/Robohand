/*
  DIY Fingerbot — Phase 1 firmware (ESP32 + servo)
  ------------------------------------------------
  Goal of this phase: confirm the servo physically presses and retracts
  on REAL hardware. It auto-presses every 3 seconds so there's nothing to
  click — just plug in, upload, and watch the arm move.

  THIS IS THE KNOWN-GOOD BASELINE. When something breaks in a later phase,
  flash this to prove the servo + wiring + power still work before debugging
  anything else. Keep it dumb. Do not add features here.

    pio run -e phase1_autopress -t upload

  Wiring:
    servo signal (orange/yellow) -> GPIO 18
    servo V+     (red)           -> +5V of a SEPARATE 5V supply  (NOT the ESP32)
    servo GND    (brown/black)   -> GND of that supply
    ESP32 GND    ---------------- -> GND of that supply   (COMMON GROUND — required)

  Do NOT run the servo off the ESP32's 5V/VIN pin. An MG90S stalling against a
  stiff switch spikes current hard enough to brown out the board mid-press.
  See the SERVO BROWNOUT note in project.md.
*/

#include <Arduino.h>
#include <ESP32Servo.h>

Servo fingerArm;
const int SERVO_PIN = 18;

// ---- Adjustable settings ----
const int REST_ANGLE  = 0;     // arm retracted, not touching the switch
const int PRESS_ANGLE = 90;    // how far the finger swings to press
const int HOLD_MS     = 800;   // how long to hold the press

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

  // IMPORTANT: ESP32Servo needs its PWM timers allocated BEFORE attach(),
  // or the servo silently never moves. Do not remove these four lines.
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  fingerArm.setPeriodHertz(50);            // standard 50 Hz servo
  fingerArm.attach(SERVO_PIN, 500, 2400);  // min/max pulse width (µs)
  fingerArm.write(REST_ANGLE);

  Serial.println("Fingerbot Phase 1 ready. Auto-pressing every 3s...\n");
}

void loop() {
  delay(3000);
  pressSwitch();
}
