/*
  command_parser — parses one typed line of the calibration console.

  Free of Arduino calls so it runs under `pio test -e native`. That matters more
  than it sounds: this thing takes hand-typed input and turns it into servo
  angles, so a slip like "go 700" must be rejected here rather than sent to the
  servo, where it would drive the arm into its end stop and stall.

  Grammar is deliberately tiny:

      help | show | save | test        no argument
      go <0-180>                       move without committing
      on|off|rest <0-180>              set an angle and move there
      hold <0-5000>                    set the hold duration in ms

  Commands are case-insensitive; a bare number is NOT a command.
*/

#pragma once

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

enum class Cmd {
  None,     // blank line — do nothing, not an error
  Unknown,  // no such command
  BadArg,   // known command, but the argument is missing, junk, or out of range
  Help,
  Show,
  Save,
  Test,
  Go,       // move to value, don't store it
  SetOn,
  SetOff,
  SetRest,
  SetHold,
};

struct Command {
  Cmd cmd   = Cmd::None;
  int value = 0;
};

// A servo commanded outside this range grinds against its end stop.
static const int ANGLE_MIN = 0;
static const int ANGLE_MAX = 180;

// Long holds cook the servo; there's no reason to exceed a few seconds.
static const int HOLD_MIN = 0;
static const int HOLD_MAX = 5000;

inline const char *cp_skipSpace(const char *p) {
  while (*p == ' ' || *p == '\t') {
    p++;
  }
  return p;
}

inline bool cp_isEnd(const char *p) {
  return *p == '\0' || *p == '\r' || *p == '\n';
}

// `word` must be lowercase; the input is folded to match.
inline bool cp_wordIs(const char *start, size_t len, const char *word) {
  if (strlen(word) != len) {
    return false;
  }
  for (size_t i = 0; i < len; i++) {
    if (tolower((unsigned char)start[i]) != word[i]) {
      return false;
    }
  }
  return true;
}

inline Command parseCommand(const char *line) {
  Command c;
  if (line == nullptr) {
    return c;
  }

  const char *p = cp_skipSpace(line);
  if (cp_isEnd(p)) {
    return c;  // None
  }

  const char *word = p;
  while (*p != '\0' && !isspace((unsigned char)*p)) {
    p++;
  }
  size_t wordLen = (size_t)(p - word);

  // ---- no-argument commands ----
  if (cp_wordIs(word, wordLen, "help")) { c.cmd = Cmd::Help; return c; }
  if (cp_wordIs(word, wordLen, "show")) { c.cmd = Cmd::Show; return c; }
  if (cp_wordIs(word, wordLen, "save")) { c.cmd = Cmd::Save; return c; }
  if (cp_wordIs(word, wordLen, "test")) { c.cmd = Cmd::Test; return c; }

  // ---- commands taking a number ----
  Cmd want = Cmd::Unknown;
  if      (cp_wordIs(word, wordLen, "go"))   { want = Cmd::Go; }
  else if (cp_wordIs(word, wordLen, "on"))   { want = Cmd::SetOn; }
  else if (cp_wordIs(word, wordLen, "off"))  { want = Cmd::SetOff; }
  else if (cp_wordIs(word, wordLen, "rest")) { want = Cmd::SetRest; }
  else if (cp_wordIs(word, wordLen, "hold")) { want = Cmd::SetHold; }

  if (want == Cmd::Unknown) {
    c.cmd = Cmd::Unknown;
    return c;
  }

  p = cp_skipSpace(p);
  if (cp_isEnd(p)) {
    c.cmd = Cmd::BadArg;  // command given with no number
    return c;
  }

  char *end = nullptr;
  long v = strtol(p, &end, 10);
  if (end == p) {
    c.cmd = Cmd::BadArg;  // not a number at all
    return c;
  }

  // Reject trailing junk, so "go 62x" is an error rather than silently 62.
  if (!cp_isEnd(cp_skipSpace(end))) {
    c.cmd = Cmd::BadArg;
    return c;
  }

  const int lo = (want == Cmd::SetHold) ? HOLD_MIN : ANGLE_MIN;
  const int hi = (want == Cmd::SetHold) ? HOLD_MAX : ANGLE_MAX;
  if (v < lo || v > hi) {
    c.cmd = Cmd::BadArg;
    return c;
  }

  c.cmd   = want;
  c.value = (int)v;
  return c;
}
