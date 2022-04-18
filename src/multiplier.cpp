#include "includes.hpp"

StorageObject mult_reg("Mult", BITS);
Bus mult_reg_bus("Mult bus", BITS);

class MultStage {
 public:
  BusALU *alus;
  StorageObject *regs;

  MultStage(int number)
    : alus(make_array<BusALU>(BITS/(1 << number), [number](auto i, auto alu) {
      std::ostringstream ss;
      ss << "MultALU" << std::hex << number << i;
      new (alu) BusALU(ss.str().c_str(), BITS * 2);
    }))
    , regs(make_array<StorageObject>(BITS/(1 << number),
      [number](auto i, auto reg) {
        std::ostringstream ss;
        ss << "MultReg" << std::hex << number << i;
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

void mult_connect(void) {
  belt.connectsTo(mult_reg_bus.IN());
  for (int i = 0; i < BITS; i++) belt.connectsTo(mult0.alus[i].OP1());
  belt.connectsTo(mult4_reg_bus.OUT());
  belt.connectsTo(mult5_alu.OUT());

  for (int i = 0; i < BITS; i++) {
    mult0_consts[i].connectsTo(mult0.alus[i].OP1());

    imm_X1.connectsTo(mult0.alus[i].OP1());
    mult0.regs[i].connectsTo(mult0.alus[i].OUT());
  }

  for (int i = 0; i < BITS; i++) {
    BusALU& alu = mult1.alus[i / 2];
    mult0.regs[i].connectsTo(i % 2 == 0 ? alu.OP1() : alu.OP2());
  }
  for (int i = 0; i < BITS / 2; i++) {
    mult1.regs[i].connectsTo(mult1.alus[i].OUT());

    BusALU& alu = mult2.alus[i / 2];
    mult1.regs[i].connectsTo(i % 2 == 0 ? alu.OP1() : alu.OP2());
  }
  for (int i = 0; i < BITS / 4; i++) {
    mult2.regs[i].connectsTo(mult2.alus[i].OUT());

    BusALU& alu = mult3.alus[i / 2];
    mult2.regs[i].connectsTo(i % 2 == 0 ? alu.OP1() : alu.OP2());
  }
  for (int i = 0; i < BITS / 8; i++) {
    mult3.regs[i].connectsTo(mult3.alus[i].OUT());
    BusALU& alu = mult3.alus[i / 2];
    mult3.regs[i].connectsTo(i % 2 == 0 ? alu.OP1() : alu.OP2());
  }
  mult4.regs[0].connectsTo(mult4.alus[0].OUT());
  mult4.regs[0].connectsTo(mult5_alu.OP1());
}
