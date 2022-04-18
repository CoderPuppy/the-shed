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
    const_bus.IN().pullFrom(nop_instr);
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
  cout << "0x" << std::right << std::hex << std::uppercase << std::setfill('0')
       << std::setw(4) << prog_cnt_X1.value() << " | ";
  cout << (x1->getLatency() == 1 ? "*" : " ") << std::left << std::setfill(' ')
       << std::setw(18) << x1->getMnemonic();
  cout << " | ";
  cout << (x2->getLatency() == 2 ? "*" : " ") << std::left << std::setfill(' ')
       << std::setw(18) << x2->getMnemonic();
  cout << " | ";
  cout << (x3->getLatency() == 3 ? "*" : " ") << std::left << std::setfill(' ')
       << std::setw(18) << x3->getMnemonic();
  cout << " | ";
  cout << (x4->getLatency() == 4 ? "*" : " ") << std::left << std::setfill(' ')
       << std::setw(18) << x4->getMnemonic();
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
