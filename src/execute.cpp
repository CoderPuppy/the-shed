#include "includes.hpp"

void fetchT1() {
  if (programState != HALTING) {
    prog_cnt_X1.latchFrom(prog_cnt_bus.OUT());
    instr_mem.MAR().latchFrom(prog_cnt_bus.OUT());
    prog_cnt_bus.IN().pullFrom(prog_cnt);
  }
}
void fetchT2() {
  if (programState != HALTING) {
    instr_mem.read();
    instr_reg_X1.latchFrom(instr_mem.READ());
    prog_cnt.incr();
  } else {
    const_bus.IN().pullFrom(nop_instr);
    instr_reg_X1.latchFrom(const_bus.OUT());
  }
  instr_reg_X1_bus.IN().pullFrom(instr_reg_X1);
  instr_reg_X2.latchFrom(instr_reg_X1_bus.OUT());

  instr_reg_X2_bus.IN().pullFrom(instr_reg_X2);
  instr_reg_X3.latchFrom(instr_reg_X2_bus.OUT());

  instr_reg_X3_bus.IN().pullFrom(instr_reg_X3);
  instr_reg_X4.latchFrom(instr_reg_X3_bus.OUT());
}

void execute() {
  Instruction* x1 = decode(instr_reg_X1);
  Instruction* x2 = decode(instr_reg_X2);
  Instruction* x3 = decode(instr_reg_X3);
  Instruction* x4 = decode(instr_reg_X4);
  cout << "0x" << std::right << std::hex << std::uppercase << std::setfill('0')
       << std::setw(4) << prog_cnt.value() << " | ";
  cout << std::left << std::setfill(' ') << std::setw(10) << x1->getMnemonic();
  cout << " | ";
  cout << std::left << std::setfill(' ') << std::setw(10) << x2->getMnemonic();
  cout << " | ";
  cout << std::left << std::setfill(' ') << std::setw(10) << x3->getMnemonic();
  cout << " | ";
  cout << std::left << std::setfill(' ') << std::setw(10) << x4->getMnemonic();
  cout << " | ";
  cout << belt.toString();
  cout << "\n";
  fetchT1();
  x1->X1T1();
  x2->X2T1();
  x3->X3T1();
  x4->X4T1();
  Clock::tick();
  fetchT2();
  x1->X1T2();
  x2->X2T2();
  x3->X3T2();
  x4->X4T2();
  Clock::tick();

  delete x1;
  delete x2;
  delete x3;
  delete x4;
}

void executeLoop() {
  while (programState == RUNNING || programState == HALTING) {
    execute();
  }
}