/*
  SwitchState — tracks which way the wall switch is currently set, and decides
  whether a physical arm movement is actually needed.

  Why this exists: a rocker switch has two press points (top = ON, bottom = OFF),
  so the arm has to know *which way* to move, not just "press". And it must not
  move when it's already in the requested state — actuating an ON switch again
  would physically turn it OFF.

  That last point is the whole reason this is a class rather than a bool. Apple
  Home re-sends state freely: opening the app, a scene firing, or two commands in
  quick succession can all deliver "turn on" when it's already on. Acting on those
  would flip the light off, which reads as a haunted house.

  Pure logic, no Arduino calls, so it runs under `pio test -e native`.

  KNOWN LIMITATION: the ESP32 cannot see the switch. If someone flips it by hand,
  this class's idea of the state is wrong until something re-syncs it. `assumeOn()`
  exists for that. Decide how to handle drift before Phase 3.
*/

#pragma once

// What the arm should physically do about a request.
enum class Action {
  None,     // already in the requested state — do NOT move
  TurnOn,   // move to the ON angle
  TurnOff,  // move to the OFF angle
};

class SwitchState {
public:
  // Cold boot: we assume the switch is off. See assumeOn() to correct that.
  explicit SwitchState(bool initiallyOn = false) : _isOn(initiallyOn) {}

  // Ask for a specific state. This is what Phase 3 / HomeKit will call.
  // Idempotent: returns Action::None when already there.
  Action requestOn(bool wantOn) {
    if (wantOn == _isOn) {
      return Action::None;
    }
    _isOn = wantOn;
    return wantOn ? Action::TurnOn : Action::TurnOff;
  }

  // Flip to the other state. This is what the Phase 1b button calls.
  // Always actuates — a button press is an explicit "change it".
  Action toggle() { return requestOn(!_isOn); }

  bool isOn() const { return _isOn; }

  // Correct our belief WITHOUT moving the arm. For when we learn the real state
  // some other way (someone flipped it by hand, or a sensor told us).
  void assumeOn(bool actuallyOn) { _isOn = actuallyOn; }

private:
  bool _isOn;
};
