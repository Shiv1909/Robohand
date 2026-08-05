/*
  Native unit tests for the calibration console's command parser.

      pio test -e native

  The important cases are the rejections. This parser stands between hand-typed
  input and a servo: "go 700" must come back as BadArg, not get forwarded to an
  arm that will drive into its end stop and stall.
*/

#include <unity.h>
#include "command_parser.h"

void setUp(void) {}
void tearDown(void) {}

// ---- nothing / nonsense ----

void test_empty_line_is_none(void) {
  TEST_ASSERT_TRUE(parseCommand("").cmd == Cmd::None);
}

void test_whitespace_only_is_none(void) {
  TEST_ASSERT_TRUE(parseCommand("   \t  ").cmd == Cmd::None);
}

void test_null_is_none(void) {
  TEST_ASSERT_TRUE(parseCommand(nullptr).cmd == Cmd::None);
}

void test_gibberish_is_unknown(void) {
  TEST_ASSERT_TRUE(parseCommand("wobble").cmd == Cmd::Unknown);
}

// A bare number is not a command — easy to type by accident.
void test_bare_number_is_unknown(void) {
  TEST_ASSERT_TRUE(parseCommand("90").cmd == Cmd::Unknown);
}

// ---- no-argument commands ----

void test_no_arg_commands(void) {
  TEST_ASSERT_TRUE(parseCommand("help").cmd == Cmd::Help);
  TEST_ASSERT_TRUE(parseCommand("show").cmd == Cmd::Show);
  TEST_ASSERT_TRUE(parseCommand("save").cmd == Cmd::Save);
  TEST_ASSERT_TRUE(parseCommand("test").cmd == Cmd::Test);
}

// ---- commands with an angle ----

void test_go_parses_value(void) {
  Command c = parseCommand("go 90");
  TEST_ASSERT_TRUE(c.cmd == Cmd::Go);
  TEST_ASSERT_EQUAL_INT(90, c.value);
}

void test_set_commands_parse_value(void) {
  TEST_ASSERT_EQUAL_INT(62,  parseCommand("on 62").value);
  TEST_ASSERT_TRUE(parseCommand("on 62").cmd == Cmd::SetOn);
  TEST_ASSERT_TRUE(parseCommand("off 118").cmd == Cmd::SetOff);
  TEST_ASSERT_TRUE(parseCommand("rest 90").cmd == Cmd::SetRest);
}

void test_boundary_angles_accepted(void) {
  TEST_ASSERT_TRUE(parseCommand("go 0").cmd == Cmd::Go);
  TEST_ASSERT_TRUE(parseCommand("go 180").cmd == Cmd::Go);
}

// ---- forgiving about formatting ----

void test_extra_whitespace_ok(void) {
  Command c = parseCommand("   go    75   ");
  TEST_ASSERT_TRUE(c.cmd == Cmd::Go);
  TEST_ASSERT_EQUAL_INT(75, c.value);
}

void test_trailing_newline_ok(void) {
  Command c = parseCommand("go 75\r\n");
  TEST_ASSERT_TRUE(c.cmd == Cmd::Go);
  TEST_ASSERT_EQUAL_INT(75, c.value);
}

void test_case_insensitive(void) {
  TEST_ASSERT_TRUE(parseCommand("GO 90").cmd == Cmd::Go);
  TEST_ASSERT_TRUE(parseCommand("Help").cmd == Cmd::Help);
}

// ---- the rejections that protect the servo ----

void test_angle_above_max_rejected(void) {
  TEST_ASSERT_TRUE(parseCommand("go 181").cmd == Cmd::BadArg);
  TEST_ASSERT_TRUE(parseCommand("go 700").cmd == Cmd::BadArg);
}

void test_negative_angle_rejected(void) {
  TEST_ASSERT_TRUE(parseCommand("go -5").cmd == Cmd::BadArg);
}

void test_missing_argument_rejected(void) {
  TEST_ASSERT_TRUE(parseCommand("go").cmd == Cmd::BadArg);
  TEST_ASSERT_TRUE(parseCommand("on   ").cmd == Cmd::BadArg);
}

void test_non_numeric_argument_rejected(void) {
  TEST_ASSERT_TRUE(parseCommand("go abc").cmd == Cmd::BadArg);
}

// "go 62x" must not silently become 62.
void test_trailing_junk_rejected(void) {
  TEST_ASSERT_TRUE(parseCommand("go 62x").cmd == Cmd::BadArg);
  TEST_ASSERT_TRUE(parseCommand("go 62 45").cmd == Cmd::BadArg);
}

// ---- hold has a different range from angles ----

void test_hold_accepts_wider_range(void) {
  Command c = parseCommand("hold 2000");
  TEST_ASSERT_TRUE(c.cmd == Cmd::SetHold);
  TEST_ASSERT_EQUAL_INT(2000, c.value);
}

void test_hold_above_max_rejected(void) {
  TEST_ASSERT_TRUE(parseCommand("hold 5001").cmd == Cmd::BadArg);
}

// 200 is a legal hold but an illegal angle — proves the ranges aren't shared.
void test_angle_and_hold_ranges_are_separate(void) {
  TEST_ASSERT_TRUE(parseCommand("hold 200").cmd == Cmd::SetHold);
  TEST_ASSERT_TRUE(parseCommand("go 200").cmd == Cmd::BadArg);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_empty_line_is_none);
  RUN_TEST(test_whitespace_only_is_none);
  RUN_TEST(test_null_is_none);
  RUN_TEST(test_gibberish_is_unknown);
  RUN_TEST(test_bare_number_is_unknown);
  RUN_TEST(test_no_arg_commands);
  RUN_TEST(test_go_parses_value);
  RUN_TEST(test_set_commands_parse_value);
  RUN_TEST(test_boundary_angles_accepted);
  RUN_TEST(test_extra_whitespace_ok);
  RUN_TEST(test_trailing_newline_ok);
  RUN_TEST(test_case_insensitive);
  RUN_TEST(test_angle_above_max_rejected);
  RUN_TEST(test_negative_angle_rejected);
  RUN_TEST(test_missing_argument_rejected);
  RUN_TEST(test_non_numeric_argument_rejected);
  RUN_TEST(test_trailing_junk_rejected);
  RUN_TEST(test_hold_accepts_wider_range);
  RUN_TEST(test_hold_above_max_rejected);
  RUN_TEST(test_angle_and_hold_ranges_are_separate);
  return UNITY_END();
}
