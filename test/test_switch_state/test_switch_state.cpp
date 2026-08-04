/*
  Native unit tests for the switch state machine.

      pio test -e native

  The case that actually matters is idempotence: Apple Home re-sends state
  freely, and acting on a redundant "turn on" would physically turn the light
  OFF. That bug would be maddening to diagnose on real hardware and is trivial
  to pin down here.
*/

#include <unity.h>
#include "switch_state.h"

void setUp(void) {}
void tearDown(void) {}

// A fresh bot assumes the switch is off.
void test_starts_off(void) {
  SwitchState s;
  TEST_ASSERT_FALSE(s.isOn());
}

void test_can_start_on(void) {
  SwitchState s(true);
  TEST_ASSERT_TRUE(s.isOn());
}

// ---- toggle(): the button path ----

void test_toggle_from_off_turns_on(void) {
  SwitchState s;
  TEST_ASSERT_TRUE(s.toggle() == Action::TurnOn);
  TEST_ASSERT_TRUE(s.isOn());
}

void test_toggle_from_on_turns_off(void) {
  SwitchState s(true);
  TEST_ASSERT_TRUE(s.toggle() == Action::TurnOff);
  TEST_ASSERT_FALSE(s.isOn());
}

// A button press is always an explicit "change it" — toggle must never no-op.
void test_toggle_always_actuates(void) {
  SwitchState s;
  for (int i = 0; i < 10; ++i) {
    TEST_ASSERT_FALSE(s.toggle() == Action::None);
  }
}

void test_toggle_alternates(void) {
  SwitchState s;
  TEST_ASSERT_TRUE(s.toggle() == Action::TurnOn);
  TEST_ASSERT_TRUE(s.toggle() == Action::TurnOff);
  TEST_ASSERT_TRUE(s.toggle() == Action::TurnOn);
  TEST_ASSERT_TRUE(s.toggle() == Action::TurnOff);
  TEST_ASSERT_FALSE(s.isOn());
}

// ---- requestOn(): the HomeKit path ----

void test_request_on_from_off_actuates(void) {
  SwitchState s;
  TEST_ASSERT_TRUE(s.requestOn(true) == Action::TurnOn);
  TEST_ASSERT_TRUE(s.isOn());
}

void test_request_off_from_on_actuates(void) {
  SwitchState s(true);
  TEST_ASSERT_TRUE(s.requestOn(false) == Action::TurnOff);
  TEST_ASSERT_FALSE(s.isOn());
}

// ⭐ The one that matters. Turning ON something already ON must NOT move the arm —
// moving it would press the switch again and physically turn the light off.
void test_request_on_when_already_on_does_nothing(void) {
  SwitchState s(true);
  TEST_ASSERT_TRUE(s.requestOn(true) == Action::None);
  TEST_ASSERT_TRUE(s.isOn());   // and it stays on
}

void test_request_off_when_already_off_does_nothing(void) {
  SwitchState s(false);
  TEST_ASSERT_TRUE(s.requestOn(false) == Action::None);
  TEST_ASSERT_FALSE(s.isOn());
}

// Apple Home can deliver the same command repeatedly. Only the first should move.
void test_repeated_requests_actuate_once(void) {
  SwitchState s;
  int moves = 0;
  for (int i = 0; i < 5; ++i) {
    if (s.requestOn(true) != Action::None) {
      moves++;
    }
  }
  TEST_ASSERT_EQUAL_INT(1, moves);
  TEST_ASSERT_TRUE(s.isOn());
}

// ---- assumeOn(): re-syncing after a human flips the switch ----

void test_assume_on_changes_state_without_actuating(void) {
  SwitchState s;
  s.assumeOn(true);
  TEST_ASSERT_TRUE(s.isOn());
  // Now believing it's on, a request to turn on should be a no-op.
  TEST_ASSERT_TRUE(s.requestOn(true) == Action::None);
}

// After re-syncing, the next real command must still work.
void test_after_assume_the_opposite_request_still_actuates(void) {
  SwitchState s;
  s.assumeOn(true);
  TEST_ASSERT_TRUE(s.requestOn(false) == Action::TurnOff);
  TEST_ASSERT_FALSE(s.isOn());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_starts_off);
  RUN_TEST(test_can_start_on);
  RUN_TEST(test_toggle_from_off_turns_on);
  RUN_TEST(test_toggle_from_on_turns_off);
  RUN_TEST(test_toggle_always_actuates);
  RUN_TEST(test_toggle_alternates);
  RUN_TEST(test_request_on_from_off_actuates);
  RUN_TEST(test_request_off_from_on_actuates);
  RUN_TEST(test_request_on_when_already_on_does_nothing);
  RUN_TEST(test_request_off_when_already_off_does_nothing);
  RUN_TEST(test_repeated_requests_actuate_once);
  RUN_TEST(test_assume_on_changes_state_without_actuating);
  RUN_TEST(test_after_assume_the_opposite_request_still_actuates);
  return UNITY_END();
}
