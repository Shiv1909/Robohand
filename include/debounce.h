/*
  Debouncer — pure button-debounce logic, no Arduino dependency.

  A mechanical button doesn't switch cleanly: the contacts bounce for a few
  milliseconds, so one physical push looks like several presses. This class
  only trusts a reading once it has held steady for `debounceMs`.

  Deliberately free of digitalRead()/millis() so it compiles and runs on a PC.
  That's what lets test/test_debounce verify it natively, with no ESP32 —
  see `pio test -e native`. The caller supplies the reading and the clock.
*/

#pragma once

class Debouncer {
public:
  explicit Debouncer(unsigned long debounceMs) : _debounceMs(debounceMs) {}

  // Feed one raw sample. `rawPressed` is the button's electrical state right
  // now (already normalised — true means pressed, whatever the wiring).
  // Returns true exactly once per debounced press, on the release->press edge.
  bool update(bool rawPressed, unsigned long nowMs) {
    // Raw sample changed — restart the settling clock.
    if (rawPressed != _lastReading) {
      _lastReading  = rawPressed;
      _lastChangeMs = nowMs;
    }

    // Held steady long enough to be real, and it's a genuine change of state.
    // Unsigned subtraction, so this stays correct across millis() rollover.
    if (nowMs - _lastChangeMs > _debounceMs && rawPressed != _stableState) {
      _stableState = rawPressed;
      return _stableState;   // true only on the edge INTO pressed
    }

    return false;
  }

  bool isPressed() const { return _stableState; }

private:
  unsigned long _debounceMs;
  bool          _lastReading  = false;   // most recent raw sample
  bool          _stableState  = false;   // debounced state we act on
  unsigned long _lastChangeMs = 0;       // when the raw sample last flipped
};
