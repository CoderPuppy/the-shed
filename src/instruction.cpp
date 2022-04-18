// instruction.cpp
// Author: Max Kipust (mak4819@rit.edu)
// Author: Drew Young (ajy2158@rit.edu)
//
// Implementation of instruction execution and decode

#include "includes.hpp"

class INVALID : public Instruction {
 public:
  void X1T1() { programState = INVALID_OPCODE; };
  void X1T2() { programState = INVALID_OPCODE; };
  void X2T1() { programState = INVALID_OPCODE; };
  void X2T2() { programState = INVALID_OPCODE; };
  void X3T1() { programState = INVALID_OPCODE; };
  void X3T2() { programState = INVALID_OPCODE; };
  void X4T1() { programState = INVALID_OPCODE; };
  void X4T2() { programState = INVALID_OPCODE; };
  string getMnemonic() { return "INVALID"; }
  int getLatency() { return 5; }
};

class HALT : public Instruction {
 public:
  void X1T1() { programState = HALTING; };
  void X1T2() { programState = HALTING; };
  void X2T1() { programState = HALTING; };
  void X2T2() { programState = HALTING; };
  void X3T1() { programState = HALTING; };
  void X3T2() { programState = HALTING; };
  void X4T1() { programState = HALTED; };
  void X4T2() { programState = HALTED; };
  string getMnemonic() { return "HALT"; }
  int getLatency() { return 4; }
};

class NOP : public Instruction {
 public:
  void X1T1(){};
  void X1T2(){};
  void X2T1(){};
  void X2T2(){};
  void X3T1(){};
  void X3T2(){};
  void X4T1(){};
  void X4T2(){};
  string getMnemonic() { return "NOP"; }
  int getLatency() { return 5; }
};

class ALU : public Instruction {
 public:
  void X1T1(){};
  void X1T2() {
    alu1.OP1().pullFrom(belt.get(b1).data);
    alu1.OP2().pullFrom(belt.get(b2).data);
    alu1.perform(op);
    belt.push(alu1.OUT(), alu1.CARRY(), alu1.OFLOW());
  };
  void X2T1(){};
  void X2T2(){};
  void X3T1(){};
  void X3T2(){};
  void X4T1(){};
  void X4T2(){};
  string getMnemonic() {
    return opToString(op) + " B" + std::to_string(b1) + " B" +
           std::to_string(b2);
  }
  ALU(int belt1, int belt2, BusALU::Operation operation) {
    b1 = belt1;
    b2 = belt2;
    op = operation;
  }
  int getLatency() { return 1; }

 private:
  int b1;
  int b2;
  BusALU::Operation op;
};

class NEGATE : public Instruction {
 public:
  void X1T1(){};
  void X1T2() {
    alu1.OP1().pullFrom(belt.get(b1).data);
    alu1.OP2().pullFrom(negate_mask);
    alu1.perform(BusALU::op_xor);
    belt.push(alu1.OUT(), alu1.CARRY(), alu1.OFLOW());
  };
  void X2T1(){};
  void X2T2(){};
  void X3T1(){};
  void X3T2(){};
  void X4T1(){};
  void X4T2(){};
  string getMnemonic() { return "NEGATE B" + std::to_string(b1); }
  NEGATE(int belt1) { b1 = belt1; }
  int getLatency() { return 1; }

 private:
  int b1;
};

class ALUC : public Instruction {
 public:
  void X1T1(){};
  void X1T2() {
    alu1.OP1().pullFrom(belt.get(b1).data);
    alu1.OP2().pullFrom(belt.get(b2).carry);
    alu1.perform(op);
    belt.push(alu1.OUT(), alu1.CARRY(), alu1.OFLOW());
  };
  void X2T1(){};
  void X2T2(){};
  void X3T1(){};
  void X3T2(){};
  void X4T1(){};
  void X4T2(){};
  string getMnemonic() {
    return opToString(op) + "C B" + std::to_string(b1) + " B" +
           std::to_string(b2);
  }
  ALUC(int belt1, int belt2, BusALU::Operation operation) {
    b1 = belt1;
    b2 = belt2;
    op = operation;
  }
  int getLatency() { return 1; }

 private:
  int b1;
  int b2;
  BusALU::Operation op;
};

class MULT : public Instruction {
 public:
  void X1T1() { mult_setup(belt.get(b1).data); }
  void X1T2() { mult_tick0(belt.get(b2).data); }
  void X2T1() { mult_tick1(); }
  void X2T2() { mult_tick2(); }
  void X3T1() { mult_tick3(); }
  void X3T2() { mult_tick4(); }
  void X4T1() { mult_tick5(); }
  void X4T2() { }
  string getMnemonic() {
    return "MUL B" + std::to_string(b1) + " B" +
           std::to_string(b2);
  }
  MULT(int belt1, int belt2) {
    b1 = belt1;
    b2 = belt2;
  }
  int getLatency() { return 4; }

 private:
  int b1;
  int b2;
};

class SE_IMM : public Instruction {
 public:
  void X1T1() {
    sign_ext.OP1().pullFrom(instr_reg_X1);
    sign_ext.OP2().pullFrom(sign_ext_mask);
    imm_X1.latchFrom(sign_ext.OUT());
    sign_ext.perform(BusALU::op_extendSign);
  }
};

class ZE_IMM : public Instruction {
 public:
  void X1T1() {
    zero_ext.IN().pullFrom(instr_reg_X1);
    imm_X1.latchFrom(zero_ext.OUT());
  }
};

class STORE : public SE_IMM {
 public:
  void X1T2() {
    alu1.IN1().pullFrom(belt.get(b1).data);
    alu1.IN2().pullFrom(imm_X1);
    alu1.perform(BusALU::op_add);
    addr_reg.latchFrom(alu1.OUT());

    data_reg_bus.IN().pullFrom(belt.get(b2).data);
    data_reg.latchFrom(data_reg_bus.OUT());
  };
  void X2T1() {
    addr_reg_bus.IN().pullFrom(addr_reg);
    data_mem.MAR().latchFrom(addr_reg_bus.OUT());
  };
  void X2T2() {
    data_mem.WRITE().pullFrom(data_reg);
    data_mem.write();
  };
  void X3T1(){};
  void X3T2(){};
  void X4T1(){};
  void X4T2(){};
  STORE(int belt1, int belt2, long immedate) {
    b1 = belt1;
    b2 = belt2;
    imm = immedate;
  }
  string getMnemonic() {
    std::ostringstream ss;
    ss << "STORE ";
    ss << "B" << std::to_string(b1) << " "
       << "B" << std::to_string(b2) << " ";
    ss << toHexString(4, imm);
    return ss.str();
  }
  int getLatency() { return 2; }

 private:
  int b1;
  int b2;
  long imm;
};

class ALUI : public SE_IMM {
 public:
  void X1T2() {
    alu1.OP1().pullFrom(belt.get(b1).data);
    alu1.OP2().pullFrom(imm_X1);
    alu1.perform(op);
    belt.push(alu1.OUT(), alu1.CARRY(), alu1.OFLOW());
  };
  void X2T1(){};
  void X2T2(){};
  void X3T1(){};
  void X3T2(){};
  void X4T1(){};
  void X4T2(){};
  string getMnemonic() {
    std::ostringstream ss;
    ss << opToString(op) << "I ";
    ss << "B" << std::to_string(b1) << " ";
    ss << toHexString(4, immediate);
    return ss.str();
  }
  ALUI(int belt1, long imm, BusALU::Operation operation) {
    b1 = belt1;
    op = operation;
    immediate = imm;
  }
  int getLatency() { return 1; }

 private:
  int b1;
  long immediate;
  BusALU::Operation op;
};

class MULTI : public SE_IMM {
 public:
  void X1T1() { SE_IMM::X1T1(); mult_setup(belt.get(b1).data); }
  void X1T2() { mult_tick0(imm_X1); }
  void X2T1() { mult_tick1(); }
  void X2T2() { mult_tick2(); }
  void X3T1() { mult_tick3(); }
  void X3T2() { mult_tick4(); }
  void X4T1() { mult_tick5(); }
  void X4T2(){};
  string getMnemonic() {
    std::ostringstream ss;
    ss << "MULI ";
    ss << "B" << std::to_string(b1) << " ";
    ss << toHexString(4, immediate);
    return ss.str();
  }
  MULTI(int belt1, long imm) {
    b1 = belt1;
    immediate = imm;
  }
  int getLatency() { return 4; }

 private:
  int b1;
  long immediate;
};

class BRANCH : public SE_IMM {
 public:
  void X1T2() {
    BeltElement& b = belt.get(b1);
    if (cond(b)) {
      alu1.OP1().pullFrom(prog_cnt_X1);
      alu1.OP2().pullFrom(imm_X1);
      alu1.perform(BusALU::op_add);
      prog_cnt.latchFrom(alu1.OUT());
    }
  };
  void X2T1(){};
  void X2T2(){};
  void X3T1(){};
  void X3T2(){};
  void X4T1(){};
  void X4T2(){};
  string getMnemonic() {
    std::ostringstream ss;
    ss << mnem;
    ss << " B" << std::to_string(b1);
    ss << " I" << toHexString(4, immediate);
    return ss.str();
  }
  BRANCH(string mnemonic, int belt1, long imm,
         std::function<bool(BeltElement&)> condition) {
    mnem = mnemonic;
    b1 = belt1;
    immediate = imm;
    cond = condition;
  }
  int getLatency() { return 1; }

 private:
  string mnem;
  int b1;
  long immediate;
  std::function<bool(BeltElement&)> cond;
};

unique_ptr<Instruction> field1_01_field4_ff_field3_7(long field1, long field2,
                                                     long field3, long field4) {
  switch (field2) {
    case 0x0:
      // halt
      return unique_ptr<HALT>(new HALT());
    case 0x1:
      // nop
      return unique_ptr<NOP>(new NOP());
    case 0x2:
      // return
      // TODO
    case 0x3:
      // get_pc
      // TODO
    default:
      return unique_ptr<INVALID>(new INVALID());
  }
}

unique_ptr<Instruction> field1_01_field4_ff(long field1, long field2,
                                            long field3, long field4) {
  switch (field3) {
    case 0x0:
      // invert
      return unique_ptr<ALU>(new ALU(field2, field2, BusALU::op_not));
    case 0x1:
      // negate
      return unique_ptr<NEGATE>(new NEGATE(field2));
    case 0x2:
      // longcall
      // TODO
    case 0x3:
      // longjump
      // TODO
    case 0x4:
      // get_carry
      // TODO
    case 0x7:
      return field1_01_field4_ff_field3_7(field1, field2, field3, field4);
    default:
      return unique_ptr<INVALID>(new INVALID());
  }
}

unique_ptr<Instruction> field1_10(long field1, long field2, long field3,
                                  long field4) {
  switch (field3) {
    case 0x0:
      // addi
      return unique_ptr<ALUI>(new ALUI(field2, field4, BusALU::op_add));
    case 0x1:
      // mlui
      return unique_ptr<MULTI>(new MULTI(field2, field4));
    case 0x2:
      // andi
      return unique_ptr<ALUI>(new ALUI(field2, field4, BusALU::op_and));
    case 0x3:
      // ori
      return unique_ptr<ALUI>(new ALUI(field2, field4, BusALU::op_or));
    case 0x4:
      // xori
      return unique_ptr<ALUI>(new ALUI(field2, field4, BusALU::op_xor));
    case 0x5:
      // lshifti
      return unique_ptr<ALUI>(new ALUI(field2, field4, BusALU::op_lshift));
    case 0x6:
      // rshifti
      return unique_ptr<ALUI>(new ALUI(field2, field4, BusALU::op_rshift));
    case 0x7:
      // arshifi
      return unique_ptr<ALUI>(new ALUI(field2, field4, BusALU::op_rashift));
    default:
      return unique_ptr<INVALID>(new INVALID());
  }
}

unique_ptr<Instruction> field1_01(long field1, long field2, long field3,
                                  long field4) {
  switch (field4) {
    case 0x00:
      // add
      return unique_ptr<ALU>(new ALU(field2, field3, BusALU::op_add));
    case 0x01:
      // mul
      return unique_ptr<MULT>(new MULT(field2, field3));
    case 0x02:
      // and
      return unique_ptr<ALU>(new ALU(field2, field3, BusALU::op_and));
    case 0x03:
      // or
      return unique_ptr<ALU>(new ALU(field2, field3, BusALU::op_or));
    case 0x04:
      // xor
      return unique_ptr<ALU>(new ALU(field2, field3, BusALU::op_xor));
    case 0x05:
      // lshift
      return unique_ptr<ALU>(new ALU(field2, field3, BusALU::op_lshift));
    case 0x06:
      // rshift
      return unique_ptr<ALU>(new ALU(field2, field3, BusALU::op_rshift));
    case 0x07:
      // arshift
      return unique_ptr<ALU>(new ALU(field2, field3, BusALU::op_rashift));
    case 0x08:
      // addc
      return unique_ptr<ALUC>(new ALUC(field2, field3, BusALU::op_add));
    case 0x09:
      // sub
      return unique_ptr<ALU>(new ALU(field2, field3, BusALU::op_sub));
    case 0xFF:
      return field1_01_field4_ff(field1, field2, field3, field4);
    default:
      return unique_ptr<INVALID>(new INVALID());
  }
}

unique_ptr<Instruction> field1_11_field3_111(long field1, long field2,
                                             long field3, long field4) {
  switch (field2) {
    case 0x0:
      // TODO: constant
      return unique_ptr<INVALID>(new INVALID());
    case 0x1:
      // TODO: upper
      return unique_ptr<INVALID>(new INVALID());
    case 0x2:
      // TODO: call
      return unique_ptr<INVALID>(new INVALID());
    case 0x3:
      // jump
      return unique_ptr<BRANCH>(new BRANCH("JMP", 0, field4,
            [](BeltElement& be) -> bool { return true; }));
    case 0x4:
      // TODO: read_stack
      return unique_ptr<INVALID>(new INVALID());
    case 0x5:
      // TODO: alloca
      return unique_ptr<INVALID>(new INVALID());
    default:
      return unique_ptr<INVALID>(new INVALID());
  }
}

unique_ptr<Instruction> field1_11(long field1, long field2, long field3,
                                  long field4) {
  switch (field3) {
    case 0x0:
      // branch_zero
      return unique_ptr<BRANCH>(new BRANCH("BZ", field2, field4,
        [](BeltElement& be) -> bool {
          return (be.data.value() == 0);
        }));
    case 0x1:
      // branch_neg
      return unique_ptr<BRANCH>(new BRANCH("BNEG", field2, field4,
        [](BeltElement& be) -> bool {
          return be.data(BITS - 1);
        }));
    case 0x2:
      // branch_oflow
      return unique_ptr<BRANCH>(new BRANCH("BO", field2, field4,
        [](BeltElement& be) -> bool {
          return be.oflow();
        }));
    case 0x3:
      // branch_carry
      return unique_ptr<BRANCH>(new BRANCH("BC", field2, field4,
        [](BeltElement& be) -> bool {
          return be.carry();
        }));
    case 0x4:
      // TODO: write_stack
      return unique_ptr<INVALID>(new INVALID());
    case 0x5:
      // TODO: load
      return unique_ptr<INVALID>(new INVALID());
    case 0x7:
      return field1_11_field3_111(field1, field2, field3, field4);
    default:
      return unique_ptr<INVALID>(new INVALID());
  }
}

unique_ptr<Instruction> decode(StorageObject& IR) {
  long field1 = IR(15, 14);
  long field2 = IR(13, 11);
  long field3 = IR(10, 8);
  long field4 = IR(7, 0);

  switch (field1) {
    case 0b00:
      return unique_ptr<STORE>(new STORE(field2, field3, field4));
    case 0b01:
      return field1_01(field1, field2, field3, field4);
    case 0b10:
      return field1_10(field1, field2, field3, field4);
    case 0b11:
      return field1_11(field1, field2, field3, field4);
    default:
      return unique_ptr<INVALID>(new INVALID());
  }
}
