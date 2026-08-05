/*
  DIY Fingerbot — Phase 1d firmware (serial calibration console)
  ---------------------------------------------------------------
  A TOOL, not part of the product. Its whole job is to find the three angles
  that work against your actual switch, without a compile-and-flash cycle for
  every guess.

    pio run -e phase1d_calibrate -t upload
    pio device monitor

  Then type commands. `go` probes an angle without committing; once one works,
  `on`/`off`/`rest` stores it. `test` runs a full cycle. `save` writes to flash.

    > go 70
    > go 62          <- rocker clicks
    > on 62
    > off 118
    > test
    > save

  When you're done, `show` prints the numbers — paste them into
  phase1c_toggle.cpp as the new defaults.

  Values persist in NVS because brownout resets are EXPECTED during this phase:
  the arm stalling against a stiff switch is exactly what triggers them, and
  losing your calibration on every reset would be maddening.

  Wiring is identical to Phase 1b/1c, minus the button (unused here).
  See WIRING.md.
*/

#include <Arduino.h>
#include <ESP32Servo.h>
#include <Preferences.h>
#include "command_parser.h"

Servo fingerArm;
Preferences prefs;

const int SERVO_PIN = 18;

// Defaults, overwritten by whatever was last saved.
int restAngle = 90;
int onAngle   = 60;
int offAngle  = 120;
int holdMs    = 500;

char   lineBuf[64];
size_t lineLen = 0;

void moveTo(int angle) {
  fingerArm.write(angle);
  delay(400);  // let it actually get there before the next command
}

void showSettings() {
  Serial.printf("   REST %d   ON %d   OFF %d   HOLD %dms\n",
                restAngle, onAngle, offAngle, holdMs);
}

void showHelp() {
  Serial.println("   go <0-180>     move there, don't store it");
  Serial.println("   on <0-180>     store the ON angle and move there");
  Serial.println("   off <0-180>    store the OFF angle and move there");
  Serial.println("   rest <0-180>   store the REST angle and move there");
  Serial.println("   hold <0-5000>  how long to stay at an angle, ms");
  Serial.println("   test           run a full rest-on-rest-off-rest cycle");
  Serial.println("   show           print current values");
  Serial.println("   save           persist to flash");
}

void runTest() {
  Serial.printf("   rest %d -> on %d -> rest %d -> off %d -> rest %d\n",
                restAngle, onAngle, restAngle, offAngle, restAngle);
  moveTo(restAngle);
  fingerArm.write(onAngle);  delay(holdMs);
  moveTo(restAngle);
  fingerArm.write(offAngle); delay(holdMs);
  moveTo(restAngle);
  Serial.println("   done");
}

void saveSettings() {
  prefs.putInt("rest", restAngle);
  prefs.putInt("on",   onAngle);
  prefs.putInt("off",  offAngle);
  prefs.putInt("hold", holdMs);
  Serial.println("   saved to flash");
}

void handleLine(const char *line) {
  Command c = parseCommand(line);

  switch (c.cmd) {
    case Cmd::None:
      break;

    case Cmd::Unknown:
      Serial.println("   ? unknown command - type 'help'");
      break;

    case Cmd::BadArg:
      Serial.printf("   ? bad argument - angles %d-%d, hold %d-%d\n",
                    ANGLE_MIN, ANGLE_MAX, HOLD_MIN, HOLD_MAX);
      break;

    case Cmd::Help:  showHelp();     break;
    case Cmd::Show:  showSettings(); break;
    case Cmd::Save:  saveSettings(); break;
    case Cmd::Test:  runTest();      break;

    case Cmd::Go:
      Serial.printf("   moved to %d\n", c.value);
      moveTo(c.value);
      break;

    case Cmd::SetOn:
      onAngle = c.value;
      Serial.printf("   ON_ANGLE = %d\n", onAngle);
      moveTo(onAngle);
      break;

    case Cmd::SetOff:
      offAngle = c.value;
      Serial.printf("   OFF_ANGLE = %d\n", offAngle);
      moveTo(offAngle);
      break;

    case Cmd::SetRest:
      restAngle = c.value;
      Serial.printf("   REST_ANGLE = %d\n", restAngle);
      moveTo(restAngle);
      break;

    case Cmd::SetHold:
      holdMs = c.value;
      Serial.printf("   HOLD = %dms\n", holdMs);
      break;
  }
}

void setup() {
  Serial.begin(115200);

  // read-write namespace; falls back to the defaults above on a fresh chip
  prefs.begin("fingerbot", false);
  restAngle = prefs.getInt("rest", restAngle);
  onAngle   = prefs.getInt("on",   onAngle);
  offAngle  = prefs.getInt("off",  offAngle);
  holdMs    = prefs.getInt("hold", holdMs);

  // IMPORTANT: ESP32Servo needs its PWM timers allocated BEFORE attach(),
  // or the servo silently never moves. Do not remove these four lines.
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  fingerArm.setPeriodHertz(50);
  fingerArm.attach(SERVO_PIN, 500, 2400);
  fingerArm.write(restAngle);

  Serial.println("\nFingerbot calibration. Type 'help'.");
  showSettings();
  Serial.print("> ");
}

void loop() {
  while (Serial.available()) {
    char ch = (char)Serial.read();

    if (ch == '\n' || ch == '\r') {
      if (lineLen > 0) {
        lineBuf[lineLen] = '\0';
        Serial.println();
        handleLine(lineBuf);
        lineLen = 0;
      }
      Serial.print("> ");
      continue;
    }

    // Silently drop anything past the buffer rather than overflowing it.
    if (lineLen < sizeof(lineBuf) - 1) {
      lineBuf[lineLen++] = ch;
      Serial.print(ch);  // echo, so typing is visible in the monitor
    }
  }
}
