// execute.hpp
// Author: Max Kipust (mak4819@rit.edu)
//
// Execution interface

#ifndef execute_hpp
#define execute_hpp

#include "includes.hpp"

extern bool branched;

enum STATE_ENUM { RUNNING, HALTING, HALTED, INVALID_OPCODE, ERROR };
extern STATE_ENUM programState;

void executeLoop();

#endif
