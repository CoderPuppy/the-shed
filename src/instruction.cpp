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
};

class ALU : public Instruction {
 public:
  void X1T1(){};
  void X1T2() {
    alu1.OP1().pullFrom(*(belt.get(b1)->data));
    alu1.OP2().pullFrom(*(belt.get(b2)->data));
    alu1.perform(op);
    belt.addToBelt(alu1.OUT(), alu1.CARRY(), alu1.OFLOW());
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

 private:
  int b1;
  int b2;
  BusALU::Operation op;
};

class ALUC : public Instruction {
 public:
  void X1T1(){};
  void X1T2() {
    alu1.OP1().pullFrom(*(belt.get(b1)->data));
    alu1.OP2().pullFrom(*(belt.get(b2)->carry));
    alu1.perform(op);
    belt.addToBelt(alu1.OUT(), alu1.CARRY(), alu1.OFLOW());
  };
  void X2T1(){};
  void X2T2(){};
  void X3T1(){};
  void X3T2(){};
  void X4T1(){};
  void X4T2(){};
  string getMnemonic() {
    return "ALUC (" + opToString(op) + ") B" + std::to_string(b1) + " BC" +
           std::to_string(b2);
  }
  ALUC(int belt1, int belt2, BusALU::Operation operation) {
    b1 = belt1;
    b2 = belt2;
    op = operation;
  }

 private:
  int b1;
  int b2;
  BusALU::Operation op;
};

class MULT : public Instruction {
 public:
  void X1T1(){};
  void X1T2(){};
  void X2T1(){};
  void X2T2(){};
  void X3T1(){};
  void X3T2(){};
  void X4T1(){};
  void X4T2(){};
  string getMnemonic() {
    return "MULT (NOT IMPLEMENTED) B" + std::to_string(b1) + " B" +
           std::to_string(b2);
  }
  MULT(int belt1, int belt2) {
    b1 = belt1;
    b2 = belt2;
  }

 private:
  int b1;
  int b2;
};
class STORE : public Instruction {
 public:
  void X1T1(){};
  void X1T2(){};
  void X2T1(){};
  void X2T2(){};
  void X3T1(){};
  void X3T2(){};
  void X4T1(){};
  void X4T2(){};
  string getMnemonic() { return "STORE (NOT IMPLEMENTED)"; }
};

Instruction* field1_01_field4_ff_field3_7(long field1, long field2, long field3,
                                          long field4) {
  switch (field2) {
    case 0x0:
      // halt
      return new HALT();
    case 0x1:
      // nop
      return new NOP();
    case 0x2:
      // return
    case 0x3:
      // get_pc
    default:
      return new INVALID();
  }
}

Instruction* field1_01_field4_ff(long field1, long field2, long field3,
                                 long field4) {
  if (field4 != ~(~0u << 8)) {
    // invalid instruction
    return new INVALID();
  }

  switch (field3) {
    case 0x0:
    // invert
    case 0x1:
    // negate
    case 0x2:
    // longcall
    case 0x3:
    // longjump
    case 0x4:
    // get_carry
    case 0x7:
      return field1_01_field4_ff_field3_7(field1, field2, field3, field4);
    default:
      return new INVALID();
  }
}

Instruction* field1_01(long field1, long field2, long field3, long field4) {
  switch (field4) {
    case 0x00:
      // add
      return new ALU(field2, field3, BusALU::op_add);
    case 0x01:
      // mul
      return new MULT(field2, field3);
    case 0x02:
      // and
      return new ALU(field2, field3, BusALU::op_and);
    case 0x03:
      // or
      return new ALU(field2, field3, BusALU::op_or);
    case 0x04:
      // xor
      return new ALU(field2, field3, BusALU::op_xor);
    case 0x05:
      // lshift
      return new ALU(field2, field3, BusALU::op_lshift);
    case 0x06:
      // rshift
      return new ALU(field2, field3, BusALU::op_rshift);
    case 0x07:
      // arshift
      return new ALU(field2, field3, BusALU::op_rashift);
    case 0x08:
      // addc
      return new ALUC(field2, field3, BusALU::op_add);
    case 0x09:
      // sub
      return new ALU(field2, field3, BusALU::op_sub);
    case 0x0a:
      // subc
      return new ALUC(field2, field3, BusALU::op_sub);
    case 0xFF:
      return field1_01_field4_ff(field1, field2, field3, field4);
    default:
      return new INVALID();
  }
}

Instruction* decode(StorageObject& IR) {
  long field1 = IR(15, 14);
  long field2 = IR(13, 11);
  long field3 = IR(10, 8);
  long field4 = IR(7, 0);

  switch (field1) {
    case 0b00:
      return new STORE();
    case 0b01:
      return field1_01(field1, field2, field3, field4);
    case 0b10:
      return new INVALID();  // TODO: decode
    case 0b11:
      // look at field 3 + field 2
      return new INVALID();  // TODO: decode
  }
}