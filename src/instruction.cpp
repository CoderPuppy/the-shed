#include "includes.hpp"

static long field1(StorageObject& IR) { return IR(15, 14); }
static long field2(StorageObject& IR) { return IR(13, 11); }
static long field3(StorageObject& IR) { return IR(10,  8); }
static long field4(StorageObject& IR) { return IR( 7,  0); }

class INVALID : public Instruction {
 public:
  void X1T1() { state = State::invalid_opcode; };
  void X1T2() { state = State::invalid_opcode; };
  void X2T1() { state = State::invalid_opcode; };
  void X2T2() { state = State::invalid_opcode; };
  void X3T1() { state = State::invalid_opcode; };
  void X3T2() { state = State::invalid_opcode; };
  void X4T1() { state = State::invalid_opcode; };
  void X4T2() { state = State::invalid_opcode; };
  string getMnemonic() { return "INVALID"; }
};
static INVALID instr_invalid();

class HALT : public Instruction {
 public:
  void X1T1() { state = State::halting; };
  void X1T2() { state = State::halting; };
  void X2T1() { state = State::halting; };
  void X2T2() { state = State::halting; };
  void X3T1() { state = State::halting; };
  void X3T2() { state = State::halting; };
  void X4T1() { state = State::halted; };
  void X4T2() { state = State::halted; };
  string getMnemonic() { return "HALT"; }
};
static HALT instr_halt();

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
static NOP instr_nop();

class ALU : public Instruction {
 public:
  const BusALU::Operation op;
  ALU(BusALU::Operation op) : op(op) {}

  void X1T1(){};
  void X1T2() {
    alu1.OP1().pullFrom(belt.get(field1(instr_reg_X1)).data);
    alu1.OP2().pullFrom(belt.get(field2(instr_reg_X1)).data);
    alu1.perform(op);
    belt.addToBelt(alu1.OUT(), alu1.CARRY(), alu1.OFLOW());
  };
  void X2T1(){};
  void X2T2(){};
  void X3T1(){};
  void X3T2(){};
  void X4T1(){};
  void X4T2(){};
  string getMnemonic(StorageObject& IR) {
    return opToString(op) + " B" + std::to_string(field2(IR)) + " B" +
           std::to_string(field3(IR));
  }
};
static ALU instr_add(BusALU::op_add);
static ALU instr_and_(BusALU::op_and);
static ALU instr_or_(BusALU::op_or);
static ALU instr_xor_(BusALU::op_xor);
static ALU instr_lshift(BusALU::op_lshift);
static ALU instr_rshift(BusALU::op_rshift);
static ALU instr_arshift(BusALU::op_rashift);
static ALU instr_sub(BusALU::op_sub);

class ALUC : public Instruction {
 public:
  BusALU::Operation op;
  ALUC(BusALU::Operation op) : op(op) {}

  void X1T1(){};
  void X1T2() {
    alu1.OP1().pullFrom(belt.get(field1(instr_reg_X1)).data);
    alu1.OP2().pullFrom(belt.get(field2(instr_reg_X1)).carry);
    alu1.perform(op);
    belt.addToBelt(alu1.OUT(), alu1.CARRY(), alu1.OFLOW());
  };
  void X2T1(){};
  void X2T2(){};
  void X3T1(){};
  void X3T2(){};
  void X4T1(){};
  void X4T2(){};
  string getMnemonic(StorageObject& IR) {
    return opToString(op) + "C B" + std::to_string(field1(IR)) +
           " B" + std::to_string(field2(IR));
  }
};
static ALUC instr_addc(BusALU::op_add);

class MULT : public Instruction {
 public:
  // TODO
  void X1T1(){};
  void X1T2(){};
  void X2T1(){};
  void X2T2(){};
  void X3T1(){};
  void X3T2(){};
  void X4T1(){};
  void X4T2(){};
  string getMnemonic(StorageObject& IR) {
    return "MULT B" + std::to_string(field1(IR)) + " B" +
           std::to_string(field2(IR));
  }
};
static MULT

class STORE : public Instruction {
 public:
	// TODO
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

class ALUI : public SE_IMM {
 public:
  void X1T2() {
    alu1.OP1().pullFrom(belt.get(b1).data);
    alu1.OP2().pullFrom(imm_X1);
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

 private:
  int b1;
  long immediate;
  BusALU::Operation op;
};

class MULTI : public SE_IMM {
 public:
	// TODO
  void X1T2(){};
  void X2T1(){};
  void X2T2(){};
  void X3T1(){};
  void X3T2(){};
  void X4T1(){};
  void X4T2(){};
  string getMnemonic() {
    std::ostringstream ss;
    ss << "MULT"
       << " (NOT IMPLEMNTED) I ";
    ss << "B" << std::to_string(b1) << " ";
    ss << "I" << toHexString(4, immediate);
    return ss.str();
  }
  MULTI(int belt1, long imm) {
    b1 = belt1;
    immediate = imm;
  }

 private:
  int b1;
  long immediate;
};

// Instruction* field1_01_field4_ff_field3_7(long field1, long field2, long field3,
//                                           long field4) {
//   switch (field2) {
//     case 0x0:
//       // halt
//       return new HALT();
//     case 0x1:
//       // nop
//       return new NOP();
//     case 0x2:
//       // return
// 			// TODO
//     case 0x3:
//       // get_pc
// 			// TODO
//     default:
//       return new INVALID();
//   }
// }
//
// Instruction* field1_01_field4_ff(long field1, long field2, long field3,
//                                  long field4) {
//   if (field4 != ~(~0u << 8)) {
//     // invalid instruction
//     return new INVALID();
//   }
//
//   switch (field3) {
//     case 0x0:
// 			// invert
// 			// TODO
//     case 0x1:
// 			// negate
// 			// TODO
//     case 0x2:
// 			// longcall
// 			// TODO
//     case 0x3:
// 			// longjump
// 			// TODO
//     case 0x4:
// 			// get_carry
// 			// TODO
//     case 0x7:
//       return field1_01_field4_ff_field3_7(field1, field2, field3, field4);
//     default:
//       return new INVALID();
//   }
// }
//
// Instruction* field1_10(long field1, long field2, long field3, long field4) {
//   switch (field3) {
//     case 0x0:
//       // addi
//       return new ALUI(field2, field4, BusALU::op_add);
//     case 0x1:
//       // mlui
//       return new MULTI(field2, field4);
//     case 0x2:
//       // andi
//       return new ALUI(field2, field4, BusALU::op_and);
//     case 0x3:
//       // ori
//       return new ALUI(field2, field4, BusALU::op_or);
//     case 0x4:
//       // xori
//       return new ALUI(field2, field4, BusALU::op_xor);
//     case 0x5:
//       // lshifti
//       return new ALUI(field2, field4, BusALU::op_lshift);
//     case 0x6:
//       // rshifti
//       return new ALUI(field2, field4, BusALU::op_rshift);
//     case 0x7:
//       // arshifi
//       return new ALUI(field2, field4, BusALU::op_rashift);
//     default:
//       return new INVALID();
//   }
// }
//
// Instruction* field1_01(long field1, long field2, long field3, long field4) {
//   switch (field4) {
//     case 0x00:
//       // add
//       return new ALU(field2, field3, BusALU::op_add);
//     case 0x01:
//       // mul
//       return new MULT(field2, field3);
//     case 0x02:
//       // and
//       return new ALU(field2, field3, BusALU::op_and);
//     case 0x03:
//       // or
//       return new ALU(field2, field3, BusALU::op_or);
//     case 0x04:
//       // xor
//       return new ALU(field2, field3, BusALU::op_xor);
//     case 0x05:
//       // lshift
//       return new ALU(field2, field3, BusALU::op_lshift);
//     case 0x06:
//       // rshift
//       return new ALU(field2, field3, BusALU::op_rshift);
//     case 0x07:
//       // arshift
//       return new ALU(field2, field3, BusALU::op_rashift);
//     case 0x08:
//       // addc
//       return new ALUC(field2, field3, BusALU::op_add);
//     case 0x09:
//       // sub
//       return new ALU(field2, field3, BusALU::op_sub);
//     case 0x0a:
//       // subc
//       return new ALUC(field2, field3, BusALU::op_sub);
//     case 0xFF:
//       return field1_01_field4_ff(field1, field2, field3, field4);
//     default:
//       return new INVALID();
//   }
// }
//
// Instruction& decode(StorageObject& IR) {
//   long field1 = IR(15, 14);
//   long field2 = IR(13, 11);
//   long field3 = IR(10, 8);
//   long field4 = IR(7, 0);
//
//   switch (field1) {
//     case 0b00:
//       return new STORE();
//     case 0b01:
//       return field1_01(field1, field2, field3, field4);
//     case 0b10:
//       return field1_10(field1, field2, field3, field4);
//     case 0b11:
//       // look at field 3 + field 2
// 			// TODO
//       return INVALID::inst;  // TODO: decode
//   }
// }
