// multiplier.hpp
// Author: Drew Young (ajy2158@rit.edu)
//
// Implementation of a multiplier. 16-bit times 16-bit to 32-bit output on two
// 16-bit registers. Uses 32 ALUs and 32 registers (excluding constants).

#include "includes.hpp"

StorageObject mult_reg("Mult", BITS);
Bus mult_reg_bus("Mult bus", BITS);

class MultStage {
 public:
  int index, size;
  BusALU *alus;
  StorageObject *regs;

  MultStage(int index)
    : index(index), size(BITS/(1 << index))
    , alus(make_array<BusALU>(BITS/(1 << index), [index](auto i, auto alu) {
      std::ostringstream ss;
      ss << "MultALU" << std::hex << index << i;
      new (alu) BusALU(ss.str().c_str(), BITS * 2);
    }))
    , regs(make_array<StorageObject>(BITS/(1 << index),
      [index](auto i, auto reg) {
        std::ostringstream ss;
        ss << "MultReg" << std::hex << index << i;
        new (reg) StorageObject(ss.str().c_str(), BITS * 2, i);
      }
    ))
  {}
};

MultStage mult0(0);
auto mult0_consts = make_array<StorageObject>(BITS, [](auto i, auto const_) {
  std::ostringstream ss;
  ss << "MultConst0" << std::hex << i;
  new (const_) StorageObject(ss.str().c_str(), BITS * 2, i);
});
MultStage mult1(1);
MultStage mult2(2);
MultStage mult3(3);
MultStage mult4(4);
Bus mult4_reg_bus("MultReg4 bus", BITS);
BusALU mult5_alu("MultALU5", BITS * 2);
StorageObject mult5_const("MultConst5", BITS, 16);

void mult_connect() {
  belt.connectsTo(mult_reg_bus.IN());
  belt.connectsTo(mult4_reg_bus.OUT());
  belt.connectsTo(mult5_alu.OUT());

  mult_reg.connectsTo(mult_reg_bus.OUT());

  for (int i = 0; i < mult0.size; i++) {
    imm_X1.connectsTo(mult0.alus[i].OP1());
    belt.connectsTo(mult0.alus[i].OP1());
    mult0_consts[i].connectsTo(mult0.alus[i].OP2());
    mult0.regs[i].connectsTo(mult0.alus[i].OUT());
  }
  for (int i = 0; i < mult1.size; i++) {
    mult0.regs[i * 2    ].connectsTo(mult1.alus[i].OP1());
    mult0.regs[i * 2 + 1].connectsTo(mult1.alus[i].OP2());
    mult1.regs[i].connectsTo(mult1.alus[i].OUT());
  }
  for (int i = 0; i < mult2.size; i++) {
    mult1.regs[i * 2    ].connectsTo(mult2.alus[i].OP1());
    mult1.regs[i * 2 + 1].connectsTo(mult2.alus[i].OP2());
    mult2.regs[i].connectsTo(mult2.alus[i].OUT());
  }
  for (int i = 0; i < mult3.size; i++) {
    mult2.regs[i * 2    ].connectsTo(mult3.alus[i].OP1());
    mult2.regs[i * 2 + 1].connectsTo(mult3.alus[i].OP2());
    mult3.regs[i].connectsTo(mult3.alus[i].OUT());
  }
  for (int i = 0; i < mult4.size; i++) {
    mult3.regs[i * 2    ].connectsTo(mult4.alus[i].OP1());
    mult3.regs[i * 2 + 1].connectsTo(mult4.alus[i].OP2());
    mult4.regs[i].connectsTo(mult4.alus[i].OUT());
  }
  mult4.regs[0].connectsTo(mult4_reg_bus.IN());
  mult4.regs[0].connectsTo(mult5_alu.OP1());
  mult5_const.connectsTo(mult5_alu.OP2());
}

void mult_setup(StorageObject& op) {
  mult_reg_bus.IN().pullFrom(op);
  mult_reg.latchFrom(mult_reg_bus.OUT());
}
void mult_tick0(StorageObject& op) {
  for (int i = 0; i < mult0.size; i++) {
    if (mult_reg(i)) {
      mult0.alus[i].OP1().pullFrom(op);
      mult0.alus[i].OP2().pullFrom(mult0_consts[i]);
      mult0.alus[i].perform(BusALU::op_lshift);
    } else {
      mult0.alus[i].perform(BusALU::op_zero);
    }
    mult0.regs[i].latchFrom(mult0.alus[i].OUT());
  }
}
void mult_tick1() {
  for (int i = 0; i < mult1.size; i++) {
    mult1.alus[i].OP1().pullFrom(mult0.regs[i * 2]);
    mult1.alus[i].OP2().pullFrom(mult0.regs[i * 2 + 1]);
    mult1.alus[i].perform(BusALU::op_add);
    mult1.regs[i].latchFrom(mult1.alus[i].OUT());
  }
}
void mult_tick2() {
  for (int i = 0; i < mult2.size; i++) {
    mult2.alus[i].OP1().pullFrom(mult1.regs[i * 2]);
    mult2.alus[i].OP2().pullFrom(mult1.regs[i * 2 + 1]);
    mult2.alus[i].perform(BusALU::op_add);
    mult2.regs[i].latchFrom(mult2.alus[i].OUT());
  }
}
void mult_tick3() {
  for (int i = 0; i < mult3.size; i++) {
    mult3.alus[i].OP1().pullFrom(mult2.regs[i * 2]);
    mult3.alus[i].OP2().pullFrom(mult2.regs[i * 2 + 1]);
    mult3.alus[i].perform(BusALU::op_add);
    mult3.regs[i].latchFrom(mult3.alus[i].OUT());
  }
}
void mult_tick4() {
  for (int i = 0; i < mult4.size; i++) {
    mult4.alus[i].OP1().pullFrom(mult3.regs[i * 3]);
    mult4.alus[i].OP2().pullFrom(mult3.regs[i * 3 + 1]);
    mult4.alus[i].perform(BusALU::op_add);
    mult4.regs[i].latchFrom(mult4.alus[i].OUT());
  }
}
void mult_tick5() {
  mult4_reg_bus.IN().pullFrom(mult4.regs[0]);
  // TODO: carry & overflow
  belt.push(mult4_reg_bus.OUT());

  mult5_alu.OP1().pullFrom(mult4.regs[0]);
  mult5_alu.OP2().pullFrom(mult5_const);
  mult5_alu.perform(BusALU::op_rshift);
  belt.push(mult5_alu.OUT());
}
