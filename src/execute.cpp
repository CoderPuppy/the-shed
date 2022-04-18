// execute.cpp
// Author: Max Kipust (mak4819@rit.edu)
//
// Instruction fetch and main execute loop

#include "includes.hpp"

void fetchT1() {
  if (programState != HALTING) {
    prog_cnt_X1.latchFrom(prog_cnt_bus.OUT());
    instr_mem.MAR().latchFrom(prog_cnt_bus.OUT());
    prog_cnt_bus.IN().pullFrom(prog_cnt);
    prog_cnt.incr();
  }
}
void fetchT2() {
  if (programState != HALTING) {
    instr_mem.read();
    instr_reg_X1.latchFrom(instr_mem.READ());

  } else {
    const_bus.IN().pullFrom(const_nop);
    instr_reg_X1.latchFrom(const_bus.OUT());
  }
  instr_reg_X1_bus.IN().pullFrom(instr_reg_X1);
  instr_reg_X2.latchFrom(instr_reg_X1_bus.OUT());

  instr_reg_X2_bus.IN().pullFrom(instr_reg_X2);
  instr_reg_X3.latchFrom(instr_reg_X2_bus.OUT());

  instr_reg_X3_bus.IN().pullFrom(instr_reg_X3);
  instr_reg_X4.latchFrom(instr_reg_X3_bus.OUT());

  imm_X1_bus.IN().pullFrom(imm_X1);
  imm_X2.latchFrom(imm_X1_bus.OUT());
}

void execute() {
  unique_ptr<Instruction> x1 = decode(instr_reg_X1);
  unique_ptr<Instruction> x2 = decode(instr_reg_X2);
  unique_ptr<Instruction> x3 = decode(instr_reg_X3);
  unique_ptr<Instruction> x4 = decode(instr_reg_X4);

  trace_cycle(*x1, *x2, *x3, *x4);

  fetchT1();
  x4->X4T1();
  x3->X3T1();
  x2->X2T1();
  x1->X1T1();
  Clock::tick();

  fetchT2();
  x4->X4T2();
  x3->X3T2();
  x2->X2T2();
  x1->X1T2();
  Clock::tick();

  belt.tick();
}

void executeLoop() {
  fetchT1();
  Clock::tick();
  fetchT2();
  Clock::tick();
  while (programState == RUNNING || programState == HALTING) {
    execute();
  }
}
