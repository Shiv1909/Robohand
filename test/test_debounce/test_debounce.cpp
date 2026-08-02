/*
  Native unit tests for the button debounce logic.

  Runs on your PC — no ESP32, no servo, no wiring:
      pio test -e native

  Debounce is the only nontrivial algorithm in Phase 1b, and it's the kind of
  thing that's miserable to diagnose with a multimeter but trivial to pin down
  with fake timestamps. Every test below drives Debouncer with a synthetic clock.
*/

#include <unity.h>
#include <limits.h>
#include "debounce.h"

static const unsigned long DEBOUNCE_MS = 50;

void setUp(void) {}
void tearDown(void) {}

// Feed a steady reading for `durationMs`, one sample per simulated millisecond.
// Advances `now` and returns how many press-edges fired during the span.
static int feed(Debouncer &d, bool pressed, unsigned long &now,
                unsigned long durationMs) {
  int edges = 0;
  for (unsigned long i = 0; i < durationMs; ++i) {
    if (d.update(pressed, now)) {
      edges++;
    }
    now++;
  }
  return edges;
}

// Simulate contact bounce: the reading flips every millisecond for `durationMs`.
static int feedBouncing(Debouncer &d, unsigned long &now,
                        unsigned long durationMs) {
  int edges = 0;
  for (unsigned long i = 0; i < durationMs; ++i) {
    if (d.update((i % 2) == 0, now)) {
      edges++;
    }
    now++;
  }
  return edges;
}

// Sitting idle must never fire anything.
void test_idle_never_fires(void) {
  Debouncer d(DEBOUNCE_MS);
  unsigned long now = 0;
  TEST_ASSERT_EQUAL_INT(0, feed(d, false, now, 1000));
  TEST_ASSERT_FALSE(d.isPressed());
}

// The happy path: one clean push produces exactly one press.
void test_clean_press_fires_once(void) {
  Debouncer d(DEBOUNCE_MS);
  unsigned long now = 0;
  feed(d, false, now, 100);
  TEST_ASSERT_EQUAL_INT(1, feed(d, true, now, 200));
  TEST_ASSERT_TRUE(d.isPressed());
}

// The whole reason this class exists: a bouncing contact is still ONE press.
void test_bouncy_press_fires_once(void) {
  Debouncer d(DEBOUNCE_MS);
  unsigned long now = 0;
  feed(d, false, now, 100);

  int edges = feedBouncing(d, now, 10);   // contacts chattering
  edges += feed(d, true, now, 200);       // then settled, held down

  TEST_ASSERT_EQUAL_INT(1, edges);
}

// Holding the button must not auto-repeat — one push, one press.
void test_held_button_does_not_repeat(void) {
  Debouncer d(DEBOUNCE_MS);
  unsigned long now = 0;
  feed(d, false, now, 100);
  TEST_ASSERT_EQUAL_INT(1, feed(d, true, now, 5000));   // held 5 seconds
}

// Releasing is not a press. Only the edge INTO pressed counts.
void test_release_does_not_fire(void) {
  Debouncer d(DEBOUNCE_MS);
  unsigned long now = 0;
  feed(d, false, now, 100);
  feed(d, true, now, 200);
  TEST_ASSERT_EQUAL_INT(0, feed(d, false, now, 200));
  TEST_ASSERT_FALSE(d.isPressed());
}

// Two deliberate pushes are two presses.
void test_press_release_press_fires_twice(void) {
  Debouncer d(DEBOUNCE_MS);
  unsigned long now = 0;
  int edges = feed(d, false, now, 100);
  edges += feed(d, true,  now, 200);
  edges += feed(d, false, now, 200);
  edges += feed(d, true,  now, 200);
  TEST_ASSERT_EQUAL_INT(2, edges);
}

// Electrical noise shorter than the window must be swallowed entirely.
void test_glitch_shorter_than_window_ignored(void) {
  Debouncer d(DEBOUNCE_MS);
  unsigned long now = 0;
  feed(d, false, now, 100);
  int edges = feed(d, true, now, 5);       // 5ms blip, well under 50ms
  edges += feed(d, false, now, 200);
  TEST_ASSERT_EQUAL_INT(0, edges);
  TEST_ASSERT_FALSE(d.isPressed());
}

// millis() wraps to zero roughly every 49 days. The unsigned subtraction in
// update() is what keeps that from freezing the button; prove it.
void test_survives_millis_rollover(void) {
  Debouncer d(DEBOUNCE_MS);
  unsigned long now = ULONG_MAX - 25;      // 25ms before the wrap
  feed(d, false, now, 10);
  TEST_ASSERT_EQUAL_INT(1, feed(d, true, now, 200));   // spans the rollover
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_idle_never_fires);
  RUN_TEST(test_clean_press_fires_once);
  RUN_TEST(test_bouncy_press_fires_once);
  RUN_TEST(test_held_button_does_not_repeat);
  RUN_TEST(test_release_does_not_fire);
  RUN_TEST(test_press_release_press_fires_twice);
  RUN_TEST(test_glitch_shorter_than_window_ignored);
  RUN_TEST(test_survives_millis_rollover);
  return UNITY_END();
}
