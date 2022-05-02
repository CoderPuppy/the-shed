// instruction.cpp
// Author: Max Kipust (mak4819@rit.edu)
// Author: Drew Young (ajy2158@rit.edu)
//
// Implementation of instruction execution and decode

#include "includes.hpp"

void signExtImm() {
  sign_ext.OP1().pullFrom(instr_reg_X1);
  sign_ext.OP2().pullFrom(const_sign_ext_mask);
  sign_ext.perform(BusALU::op_extendSign);
  imm_X1.latchFrom(sign_ext.OUT());
}
void zeroExtImm() {
  zero_ext.IN().pullFrom(instr_reg_X1);
  imm_X1.latchFrom(zero_ext.OUT());
}

class INVALID : public Instruction {
 public:
  void X1T1() { programState = State::invalid_opcode; }
  void X1T2() { programState = State::invalid_opcode; }
  void X2T1() { programState = State::invalid_opcode; }
  void X2T2() { programState = State::invalid_opcode; }
  void X3T1() { programState = State::invalid_opcode; }
  void X3T2() { programState = State::invalid_opcode; }
  void X4T1() { programState = State::invalid_opcode; }
  void X4T2() { programState = State::invalid_opcode; }
  void print(ostream& s) { s << "INVALID"; }
  int getLatency() { return 5; }
};

class HALT : public Instruction {
 public:
  void X1T1() { programState = State::halting; }
  void X1T2() { programState = State::halting; }
  void X2T1() { programState = State::halting; }
  void X2T2() { programState = State::halting; }
  void X3T1() { programState = State::halting; }
  void X3T2() { programState = State::halting; }
  void X4T1() { programState = State::halted; }
  void X4T2() { programState = State::halted; }
  void print(ostream& s) { s << "HALT"; }
  int getLatency() { return 4; }
};

class NOP : public Instruction {
 public:
  void print(ostream& s) { s << "NOP"; }
  int getLatency() { return 5; }
};

class ALU : public Instruction {
 public:
  void X1T2() {
    alu1.OP1().pullFrom(belt.get(b1).data);
    alu1.OP2().pullFrom(belt.get(b2).data);
    alu1.perform(op);
    belt.push(alu1.OUT(), alu1.CARRY(), alu1.OFLOW());
  }
  void print(ostream& s) {
    s << opToString(op) << " B" << b1 << " B" << b2;
  }
  ALU(int b1, int b2, BusALU::Operation op) : b1(b1), b2(b2), op(op) {}
  int getLatency() { return 1; }

 private:
  int b1;
  int b2;
  BusALU::Operation op;
};

class NEGATE : public Instruction {
 public:
  void X1T2() {
    alu1.OP1().pullFrom(const_0);
    alu1.OP2().pullFrom(belt.get(b1).data);
    alu1.perform(BusALU::op_sub);
    belt.push(alu1.OUT(), alu1.CARRY(), alu1.OFLOW());
  }
  void print(ostream& s) { s << "NEG B" << b1; }
  NEGATE(int b1) : b1(b1) {}
  int getLatency() { return 1; }

 private:
  int b1;
};

class ALUC : public Instruction {
 public:
  void X1T2() {
    alu1.OP1().pullFrom(belt.get(b1).data);
    alu1.OP2().pullFrom(belt.get(b2).carry);
    alu1.perform(op);
    belt.push(alu1.OUT(), alu1.CARRY(), alu1.OFLOW());
  }
  void print(ostream& s) {
    s << opToString(op) << "C B" << b1 << " B" << b2;
  }
  ALUC(int b1, int b2, BusALU::Operation op) : b1(b1), b2(b2), op(op) {}
  int getLatency() { return 1; }

 private:
  int b1;
  int b2;
  BusALU::Operation op;
};

class CARRY : public Instruction {
 public:
  void X1T2() {
    alu1.OP2().pullFrom(belt.get(b1).carry);
    alu1.perform(BusALU::op_rop2);
    belt.push(alu1.OUT());
  }
  void print(ostream& s) { s << "CARRY B" << b1; }
  CARRY(int b1) : b1(b1) {}
  int getLatency() { return 1; }

 private:
  int b1;
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
  void print(ostream& s) { s << "MUL B" << b1 << " B" << b2; }
  MULT(int b1, int b2) : b1(b1), b2(b2) {}
  int getLatency() { return 4; }

 private:
  int b1;
  int b2;
};

class STORE : public Instruction {
 public:
  void X1T1() { signExtImm(); }
  void X1T2() {
    alu1.IN1().pullFrom(belt.get(b1).data);
    alu1.IN2().pullFrom(imm_X1);
    alu1.perform(BusALU::op_add);
    addr_reg.latchFrom(alu1.OUT());
    alu1_flag.latchFrom(alu1.CARRY());

    data_reg_bus.IN().pullFrom(belt.get(b2).data);
    data_reg.latchFrom(data_reg_bus.OUT());
  }
  void X2T1() {
    if (alu1_flag() != imm_X2()) {
      programState = State::heap_overflow;
    }
    addr_reg_bus.IN().pullFrom(addr_reg);
    data_mem.MAR().latchFrom(addr_reg_bus.OUT());
  }
  void X2T2() {
    data_mem.WRITE().pullFrom(data_reg);
    data_mem.write();
  }
  void print(ostream& s) {
    s << "ST ";
    s << "B" << b1 << " ";
    s << "B" << b2 << " ";
    s << setw(4) << (data_t)(se_imm_t)imm;
  }
  STORE(int b1, int b2, long imm) : b1(b1), b2(b2), imm(imm) {}
  int getLatency() { return 2; }

 private:
  int b1;
  int b2;
  long imm;
};

class LOAD : public Instruction {
 public:
  void X1T1() { signExtImm(); }
  void X1T2() {
    alu1.IN1().pullFrom(belt.get(b1).data);
    alu1.IN2().pullFrom(imm_X1);
    alu1.perform(BusALU::op_add);
    addr_reg.latchFrom(alu1.OUT());
    alu1_flag.latchFrom(alu1.CARRY());
  }
  void X2T1() {
    if (alu1_flag() != imm_X2()) {
      programState = State::heap_overflow;
    }
    addr_reg_bus.IN().pullFrom(addr_reg);
    data_mem.MAR().latchFrom(addr_reg_bus.OUT());
  }
  void X2T2() {
    belt.push(data_mem.READ());
    data_mem.read();
  }
  void print(ostream& s) {
    s << "LD ";
    s << "B" << b1 << " ";
    s << setw(4) << (data_t)(se_imm_t)imm;
  }
  LOAD(int b1, long imm) : b1(b1), imm(imm) {}
  int getLatency() { return 2; }

 private:
  int b1;
  long imm;
};

class STORE_STACK : public Instruction {
 public:
  void X1T1() { zeroExtImm(); }
  void X1T2() {
    alu1.IN1().pullFrom(frame_ptr);
    alu1.IN2().pullFrom(imm_X1);
    alu1.perform(BusALU::op_add);
    addr_reg.latchFrom(alu1.OUT());
    alu1_flag.latchFrom(alu1.CARRY());

    data_reg_bus.IN().pullFrom(belt.get(b1).data);
    data_reg.latchFrom(data_reg_bus.OUT());
  }
  void X2T1() {
    if (alu1_flag()) {
      programState = State::invalid_stack_access;
    }
    addr_reg_bus.IN().pullFrom(addr_reg);
    stack_mem.MAR().latchFrom(addr_reg_bus.OUT());

    alu2.OP1().pullFrom(stack_ptr);
    alu2.OP2().pullFrom(addr_reg);
    alu2.perform(BusALU::op_sub);
    cmp.latchFrom(alu2.OUT());
  }
  void X2T2() {
    stack_mem.WRITE().pullFrom(data_reg);
    stack_mem.write();

    // Addr >= SP
    if (cmp() || !cmp.value()) {
      programState = State::invalid_stack_access;
    }
  }
  void print(ostream& s) {
    s << "STS ";
    s << "B" << b1 << " ";
    s << setw(4) << (data_t)(ze_imm_t)imm;
  }
  STORE_STACK(int b1, long imm) : b1(b1), imm(imm) {}
  int getLatency() { return 2; }

 private:
  int b1;
  long imm;
};

class LOAD_STACK : public Instruction {
 public:
  void X1T1() { zeroExtImm(); }
  void X1T2() {
    alu1.IN1().pullFrom(frame_ptr);
    alu1.IN2().pullFrom(imm_X1);
    alu1.perform(BusALU::op_add);
    addr_reg.latchFrom(alu1.OUT());
    alu1_flag.latchFrom(alu1.CARRY());
  }
  void X2T1() {
    if (alu1_flag()) {
      programState = State::invalid_stack_access;
    }
    addr_reg_bus.IN().pullFrom(addr_reg);
    stack_mem.MAR().latchFrom(addr_reg_bus.OUT());

    alu2.OP1().pullFrom(stack_ptr);
    alu2.OP2().pullFrom(addr_reg);
    alu2.perform(BusALU::op_sub);
    cmp.latchFrom(alu2.OUT());
  }
  void X2T2() {
    stack_mem.read();
    belt.push(stack_mem.READ());

    // Addr >= SP
    if (cmp() || !cmp.value()) {
      programState = State::invalid_stack_access;
    }
  }
  void print(ostream& s) {
    s << "LDS ";
    s << setw(4) << (data_t)(ze_imm_t)imm;
  }
  LOAD_STACK(long imm) : imm(imm) {}
  int getLatency() { return 2; }

 private:
  long imm;
};

class ALLOC : public Instruction {
 public:
  void X1T1() { zeroExtImm(); }
  void X2T1() {
    alu2.OP1().pullFrom(stack_ptr);
    alu2.OP2().pullFrom(imm_X2);
    alu2.perform(BusALU::op_add);
    stack_ptr.latchFrom(alu2.OUT());
    alu2_flag.latchFrom(alu2.CARRY());
  }
  void X2T2() {
    if (alu2_flag()) {
      programState = State::stack_overflow;
    }
  }
  void print(ostream& s) {
    s << "ALLOC ";
    s << setw(4) << (data_t)(ze_imm_t)imm;
  }
  ALLOC(long imm) : imm(imm) {}
  int getLatency() { return 2; }

 private:
  long imm;
};

class ALUI : public Instruction {
 public:
  void X1T1() {
    if (sign_ext)
      signExtImm();
    else
      zeroExtImm();
  }
  void X1T2() {
    alu1.OP1().pullFrom(belt.get(b1).data);
    alu1.OP2().pullFrom(imm_X1);
    alu1.perform(op);
    belt.push(alu1.OUT(), alu1.CARRY(), alu1.OFLOW());
  }
  void print(ostream& s) {
    s << opToString(op) << "I ";
    s << "B" << b1 << " ";
    s << setw(4) << (data_t)(se_imm_t)imm;
  }
  ALUI(int b1, long imm, BusALU::Operation op, bool sign_ext)
      : b1(b1), imm(imm), op(op), sign_ext(sign_ext) {}
  int getLatency() { return 1; }

 private:
  int b1;
  long imm;
  BusALU::Operation op;
  bool sign_ext;
};

class CONST : public Instruction {
 public:
  void X1T1() { signExtImm(); }
  void X1T2() {
    alu1.OP2().pullFrom(imm_X1);
    alu1.perform(BusALU::op_rop2);
    belt.push(alu1.OUT());
  }
  void print(ostream& s) {
    s << "CONST ";
    s << setw(4) << (data_t)(se_imm_t)imm;
  }
  CONST(long imm) : imm(imm) {}
  int getLatency() { return 1; }

 private:
  long imm;
};

class UPPER : public Instruction {
 public:
  void X1T1() { zeroExtImm(); }
  void X1T2() {
    alu1.IN1().pullFrom(imm_X1);
    alu1.IN2().pullFrom(const_8);
    alu1.perform(BusALU::op_lshift);
    belt.push(alu1.OUT());
  }
  void print(ostream& s) {
    s << "UPPER ";
    s << setw(4) << (data_t)(se_imm_t)imm;
  }
  UPPER(long imm) : imm(imm) {}
  int getLatency() { return 1; }

 private:
  long imm;
};

class MULTI : public Instruction {
 public:
  void X1T1() {
    signExtImm();
    mult_setup(belt.get(b1).data);
  }
  void X1T2() { mult_tick0(imm_X1); }
  void X2T1() { mult_tick1(); }
  void X2T2() { mult_tick2(); }
  void X3T1() { mult_tick3(); }
  void X3T2() { mult_tick4(); }
  void X4T1() { mult_tick5(); }
  void print(ostream& s) {
    s << "MULI ";
    s << "B" << b1 << " ";
    s << setw(4) << (data_t)(se_imm_t)imm;
  }
  MULTI(int b1, long imm) : b1(b1), imm(imm) {}
  int getLatency() { return 4; }

 private:
  int b1;
  long imm;
};

class PC : public Instruction {
 public:
  void X1T2() {
    alu1.OP1().pullFrom(prog_cnt_X1);
    alu1.perform(BusALU::op_rop1);
    belt.push(alu1.OUT());
  }
  void print(ostream& s) { s << "PC"; }
  int getLatency() { return 1; }
};

class BRANCH : public Instruction {
 public:
  void X1T1() {
    signExtImm();
    if (cond(belt.get(b1))) branched = true;
  }
  void X1T2() {
    if (cond(belt.get(b1))) {
      alu1.OP1().pullFrom(prog_cnt_X1);
      alu1.OP2().pullFrom(imm_X1);
      alu1.perform(BusALU::op_add);
      prog_cnt.latchFrom(alu1.OUT());
      alu1_flag.latchFrom(alu1.CARRY());
    }
  }
  void X1C() {
    if (cond(belt.get(b1)) && alu1_flag() != imm_X1()) {
      programState = State::pc_overflow;
    }
  }
  void print(ostream& s) {
    s << mnemonic;
    s << " B" << b1 << " ";
    s << setw(4) << (data_t)(se_imm_t)imm;
  }
  BRANCH(string mnemonic, int b1, long imm,
         function<bool(BeltElement&)> cond)
      : mnemonic(mnemonic), b1(b1), imm(imm), cond(cond) {}
  int getLatency() { return 2; }

 private:
  string mnemonic;
  int b1;
  long imm;
  function<bool(BeltElement&)> cond;
};

class RET1 : public Instruction {
 public:
  void X1T1() {
    branched = true;

    ret_addr_bus.IN().pullFrom(ret_addr);
    prog_cnt.latchFrom(ret_addr_bus.OUT());
  }
  void X1T2() {
    alu1.OP1().pullFrom(frame_ptr);
    alu1.OP2().pullFrom(const_1);
    alu1.perform(BusALU::op_sub);
    addr_reg.latchFrom(alu1.OUT());
    alu1_flag.latchFrom(alu1.CARRY());
  }
  void X1C() {
    if (!alu1_flag()) {
      programState = State::stack_underflow;
    }
  }
  void X2T1() {
    addr_reg_bus.IN().pullFrom(addr_reg);
    stack_mem.MAR().latchFrom(addr_reg_bus.OUT());
  }
  void X2T2() {
    stack_mem.read();
    ret_addr.latchFrom(stack_mem.READ());
  }
  void print(ostream& s) { s << "RET1"; }
  int getLatency() { return 2; }
};

class RET2 : public Instruction {
 public:
  void X1T1() {
    ret_frame_ptr_bus.IN().pullFrom(ret_frame_ptr);
    frame_ptr.latchFrom(ret_frame_ptr_bus.OUT());

    alu1.OP1().pullFrom(frame_ptr);
    alu1.OP2().pullFrom(const_2);
    alu1.perform(BusALU::op_sub);
    addr_reg.latchFrom(alu1.OUT());
    alu1_flag.latchFrom(alu1.CARRY());
  }
  void X1T2() {
    if (!alu1_flag()) {
      programState = State::stack_underflow;
    }
  }
  void X2T1() {
    alu2.OP2().pullFrom(addr_reg);
    alu2.perform(BusALU::op_rop2);
    stack_mem.MAR().latchFrom(alu2.OUT());
    stack_ptr.latchFrom(alu2.OUT());
  }
  void X2T2() {
    stack_mem.read();
    ret_frame_ptr.latchFrom(stack_mem.READ());
  }
  void print(ostream& s) { s << "RET2"; }
  int getLatency() { return 2; }
};

class CALL1 : public Instruction {
 public:
  void X1T1() {
    branched = true;

    signExtImm();
  }
  void X1T2() {
    alu1.OP1().pullFrom(prog_cnt_X1);
    alu1.OP2().pullFrom(imm_X1);
    alu1.perform(BusALU::op_add);
    prog_cnt.latchFrom(alu1.OUT());
    alu1_flag.latchFrom(alu1.CARRY());
  }
  void X1C() {
    if (alu1_flag() != imm_X1()) {
      programState = State::pc_overflow;
    }
  }
  void X2T1() {
    alu2.OP1().pullFrom(stack_ptr);
    alu2.OP2().pullFrom(const_1);
    alu2.perform(BusALU::op_add);
    stack_mem.MAR().latchFrom(alu2.OUT());
    frame_ptr.latchFrom(alu2.OUT());
    alu2_flag.latchFrom(alu2.CARRY());
  }
  void X2T2() {
    if (alu2_flag()) {
      programState = State::stack_overflow;
    }

    stack_mem.WRITE().pullFrom(ret_addr);
    stack_mem.write();

    frame_ptr.incr();
  }
  void X2C() {
    if (frame_ptr.overflow()) {
      programState = State::stack_overflow;
    }
  }
  void print(ostream& s) {
    s << "CALL1 ";
    s << setw(4) << (data_t)(se_imm_t)imm;
  }
  int getLatency() { return 2; }
  CALL1(int imm) : imm(imm) {}

 private:
  int imm;
};

class LCALL : public Instruction {
 public:
  void X1T1() { branched = true; }
  void X1T2() {
    alu1.OP1().pullFrom(belt.get(b1).data);
    alu1.perform(BusALU::op_rop1);
    prog_cnt.latchFrom(alu1.OUT());
  }
  void X2T1() {
    alu2.OP1().pullFrom(stack_ptr);
    alu2.OP2().pullFrom(const_1);
    alu2.perform(BusALU::op_add);
    stack_mem.MAR().latchFrom(alu2.OUT());
    frame_ptr.latchFrom(alu2.OUT());
    alu2_flag.latchFrom(alu2.CARRY());
  }
  void X2T2() {
    if (alu2_flag()) {
      programState = State::stack_overflow;
    }

    stack_mem.WRITE().pullFrom(ret_addr);
    stack_mem.write();

    frame_ptr.incr();
  }
  void X2C() {
    if (frame_ptr.overflow()) {
      programState = State::stack_overflow;
    }
  }

  LCALL(int b1) : b1(b1) {}
  void print(ostream& s) {
    s << "LCALL";
    s << " B" << b1 << " ";
  }
  int getLatency() { return 2; }

 private:
  int b1;
};

class CALL2 : public Instruction {
 public:
  void X1T1() {
    alu1.OP1().pullFrom(frame_ptr);
    alu1.perform(BusALU::op_rop1);
    addr_reg.latchFrom(alu1.OUT());
  }
  void X1T2() {
    alu1.OP1().pullFrom(prog_cnt_X1);
    alu1.OP2().pullFrom(const_1);
    alu1.perform(BusALU::op_add);
    ret_addr.latchFrom(alu1.OUT());
    alu1_flag.latchFrom(alu1.CARRY());
  }
  void X1C() {
    if (alu1_flag()) {
      programState = State::pc_overflow;
    }
  }
  void X2T1() {
    alu2.OP1().pullFrom(stack_ptr);
    alu2.perform(BusALU::op_rop1);
    stack_mem.MAR().latchFrom(alu2.OUT());

    stack_ptr.perform(Counter::incr2);
  }
  void X2T2() {
    if (stack_ptr.overflow()) {
      programState = State::stack_overflow;
    }

    stack_mem.WRITE().pullFrom(ret_frame_ptr);
    stack_mem.write();

    alu2.OP2().pullFrom(addr_reg);
    alu2.perform(BusALU::op_rop2);
    ret_frame_ptr.latchFrom(alu2.OUT());
  }
  void print(ostream& s) { s << "CALL2"; }
  int getLatency() { return 2; }
};

class LJUMP : public Instruction {
 public:
  void X1T1() {}
  void X1T2() {
    alu1.IN1().pullFrom(belt.get(b1).data);
    alu1.perform(BusALU::op_rop1);
    prog_cnt.latchFrom(alu1.OUT());
  }
  void print(ostream& s) {
    s << "LJUMP";
    s << " B" << b1 << " ";
  }
  LJUMP(int b1) : b1(b1) {}
  int getLatency() { return 2; }

 private:
  int b1;
};

class DUMP : public Instruction {
 public:
  void X1T1() {
    cout << mem->name() << endl;
    mem->dump(belt.get(b1).data.value(), belt.get(b2).data.value());
  }
  void X1T2() {}
  void print(ostream& s) {
    s << "DUMP";
    s << " B" << b1 << " ";
    s << " B" << b2 << " ";
  }
  DUMP(int b1, int b2, Memory* mem) : b1(b1), b2(b2), mem(mem) {}
  int getLatency() { return 1; }

 private:
  int b1;
  int b2;
  Memory* mem;
};

unique_ptr<Instruction> field1_01_field4_ff_field3_7(long field1,
                                                     long field2,
                                                     long field3,
                                                     long field4) {
  switch (field2) {
    case 0x0:
      // halt
      return unique_ptr<HALT>(new HALT());
    case 0x1:
      // nop
      return unique_ptr<NOP>(new NOP());
    case 0x2:
      // pc
      return unique_ptr<PC>(new PC());
    case 0x3:
      // ret1
      return unique_ptr<RET1>(new RET1());
    case 0x4:
      // ret2
      return unique_ptr<RET2>(new RET2());
    case 0x5:
      // call2
      return unique_ptr<CALL2>(new CALL2());
    default:
      return unique_ptr<INVALID>(new INVALID());
  }
}

unique_ptr<Instruction> field1_01_field4_ff(long field1, long field2,
                                            long field3, long field4) {
  switch (field3) {
    case 0x0:
      // inv
      return unique_ptr<ALU>(new ALU(field2, field2, BusALU::op_not));
    case 0x1:
      // neg
      return unique_ptr<NEGATE>(new NEGATE(field2));
    case 0x2:
      // lcall
      return unique_ptr<LCALL>(new LCALL(field2));
    case 0x3:
      // ljmp
      return unique_ptr<LJUMP>(new LJUMP(field2));
    case 0x4:
      // carry
      return unique_ptr<CARRY>(new CARRY(field2));
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
      return unique_ptr<ALUI>(
          new ALUI(field2, field4, BusALU::op_add, true));
    case 0x1:
      // mlui
      return unique_ptr<MULTI>(new MULTI(field2, field4));
    case 0x2:
      // andi
      return unique_ptr<ALUI>(
          new ALUI(field2, field4, BusALU::op_and, true));
    case 0x3:
      // ori
      return unique_ptr<ALUI>(
          new ALUI(field2, field4, BusALU::op_or, false));
    case 0x4:
      // xori
      return unique_ptr<ALUI>(
          new ALUI(field2, field4, BusALU::op_xor, false));
    case 0x5:
      // slli
      return unique_ptr<ALUI>(
          new ALUI(field2, field4, BusALU::op_lshift, false));
    case 0x6:
      // srli
      return unique_ptr<ALUI>(
          new ALUI(field2, field4, BusALU::op_rshift, false));
    case 0x7:
      // srai
      return unique_ptr<ALUI>(
          new ALUI(field2, field4, BusALU::op_rashift, false));
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
      // sll
      return unique_ptr<ALU>(new ALU(field2, field3, BusALU::op_lshift));
    case 0x06:
      // srl
      return unique_ptr<ALU>(new ALU(field2, field3, BusALU::op_rshift));
    case 0x07:
      // sra
      return unique_ptr<ALU>(new ALU(field2, field3, BusALU::op_rashift));
    case 0x08:
      // addc
      return unique_ptr<ALUC>(new ALUC(field2, field3, BusALU::op_add));
    case 0x09:
      // sub
      return unique_ptr<ALU>(new ALU(field2, field3, BusALU::op_sub));
    case 0x0A:
      // DDUMP
      return unique_ptr<DUMP>(new DUMP(field2, field3, &data_mem));
    case 0x0B:
      // SDUMP
      return unique_ptr<DUMP>(new DUMP(field2, field3, &stack_mem));
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
      // const
      return unique_ptr<CONST>(new CONST(field4));
    case 0x1:
      // upper
      return unique_ptr<UPPER>(new UPPER(field4));
    case 0x2:
      // call1
      return unique_ptr<CALL1>(new CALL1(field4));
    case 0x3:
      // jmp
      return unique_ptr<BRANCH>(new BRANCH(
          "JMP", 0, field4, [](BeltElement& be) -> bool { return true; }));
    case 0x4:
      // lds
      return unique_ptr<LOAD_STACK>(new LOAD_STACK(field4));
    case 0x5:
      // alloc
      return unique_ptr<ALLOC>(new ALLOC(field4));
    default:
      return unique_ptr<INVALID>(new INVALID());
  }
}

unique_ptr<Instruction> field1_11(long field1, long field2, long field3,
                                  long field4) {
  switch (field3) {
    case 0x0:
      // bzero
      return unique_ptr<BRANCH>(new BRANCH(
          "BZERO", field2, field4,
          [](BeltElement& be) -> bool { return (be.data.value() == 0); }));
    case 0x1:
      // bneg
      return unique_ptr<BRANCH>(new BRANCH(
          "BNEG", field2, field4,
          [](BeltElement& be) -> bool { return be.data(BITS - 1); }));
    case 0x2:
      // boflo
      return unique_ptr<BRANCH>(
          new BRANCH("BOFLO", field2, field4,
                     [](BeltElement& be) -> bool { return be.oflow(); }));
    case 0x3:
      // bcar
      return unique_ptr<BRANCH>(
          new BRANCH("BCAR", field2, field4,
                     [](BeltElement& be) -> bool { return be.carry(); }));
    case 0x4:
      // sts
      return unique_ptr<STORE_STACK>(new STORE_STACK(field2, field4));
    case 0x5:
      // ld
      return unique_ptr<LOAD>(new LOAD(field2, field4));
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
