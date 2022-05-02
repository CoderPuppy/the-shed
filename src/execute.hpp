// execute.hpp
// Author: Max Kipust (mak4819@rit.edu)
// Author: Drew Young (ajy2158@rit.edu)
//
// Execution interface

#ifndef execute_hpp
#define execute_hpp

#include "includes.hpp"

extern bool branched;

enum class State {
  running,
  halting,
  halted,
  invalid_opcode,
  stack_overflow,
  pc_overflow,
  heap_overflow,
  invalid_stack_access,
  stack_underflow
};
extern State programState;

void executeLoop();

#endif
