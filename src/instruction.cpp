#include "includes.hpp"

static long field1(StorageObject& IR) { return IR(15, 14); }
static long field2(StorageObject& IR) { return IR(13, 11); }
static long field3(StorageObject& IR) { return IR(10,  8); }
static long field4(StorageObject& IR) { return IR( 7,  0); }

static void signExtImm() {
  sign_ext.OP1().pullFrom(instr_reg_X1);
  sign_ext.OP2().pullFrom(sign_ext_mask);
  imm_X1.latchFrom(sign_ext.OUT());
  sign_ext.perform(BusALU::op_extendSign);
}
static void zeroExtImm() {
  zero_ext.IN().pullFrom(instr_reg_X1);
  imm_X1.latchFrom(zero_ext.OUT());
}

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
  string getMnemonic(StorageObject& IR) { return "INVALID"; }
};
static INVALID instr_invalid;

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
  string getMnemonic(StorageObject& IR) { return "HALT"; }
};
static HALT instr_halt;

class NOP : public Instruction {
 public:
  string getMnemonic(StorageObject& IR) { return "NOP"; }
};
static NOP instr_nop;

class ALU : public Instruction {
 public:
  enum class OP2 {
    belt, carry, ze_imm, se_imm
  };

  const BusALU::Operation op;
  const OP2 op2;
  ALU(BusALU::Operation op, OP2 op2) : op(op), op2(op2) {}

  void X1T1() {
    switch(op2) {
      case OP2::belt:
      case OP2::carry:
        break;
      case OP2::ze_imm:
        zeroExtImm();
        break;
      case OP2::se_imm:
        signExtImm();
        break;
    }
  }
  void X1T2() {
    alu1.OP1().pullFrom(belt.get(field1(instr_reg_X1)).data);
    switch(op2) {
      case OP2::belt:
        alu1.OP2().pullFrom(belt.get(field2(instr_reg_X1)).data);
        break;
      case OP2::carry:
        alu1.OP2().pullFrom(belt.get(field2(instr_reg_X1)).carry);
        break;
      case OP2::ze_imm:
      case OP2::se_imm:
        alu1.OP2().pullFrom(imm_X1);
        break;
    }
    alu1.perform(op);
    belt.addToBelt(alu1.OUT(), alu1.CARRY(), alu1.OFLOW());
  }
  string getMnemonic(StorageObject& IR) {
    std::ostringstream ss;
    ss << opToString(op) << " B" << field2(IR);
    switch(op2) {
      case OP2::belt:
      case OP2::carry:
        ss << " B" << field3(IR);
        break;
      case OP2::ze_imm:
      case OP2::se_imm:
        ss << " " << toHexString(4, field4(IR));
        break;
    }
    return ss.str();
  }
};
static ALU instr_add(BusALU::op_add, ALU::OP2::belt);
static ALU instr_and_(BusALU::op_and, ALU::OP2::belt);
static ALU instr_or_(BusALU::op_or, ALU::OP2::belt);
static ALU instr_xor_(BusALU::op_xor, ALU::OP2::belt);
static ALU instr_lshift(BusALU::op_lshift, ALU::OP2::belt);
static ALU instr_rshift(BusALU::op_rshift, ALU::OP2::belt);
static ALU instr_arshift(BusALU::op_rashift, ALU::OP2::belt);
static ALU instr_sub(BusALU::op_sub, ALU::OP2::belt);
static ALU instr_addc(BusALU::op_add, ALU::OP2::carry);
static ALU instr_addi(BusALU::op_add, ALU::OP2::se_imm);
static ALU instr_andi(BusALU::op_and, ALU::OP2::se_imm);
static ALU instr_ori(BusALU::op_or, ALU::OP2::ze_imm);
static ALU instr_xori(BusALU::op_xor, ALU::OP2::ze_imm);
static ALU instr_lshifti(BusALU::op_lshift, ALU::OP2::ze_imm);
static ALU instr_rshifti(BusALU::op_rshift, ALU::OP2::ze_imm);
static ALU instr_arshifti(BusALU::op_rashift, ALU::OP2::ze_imm);

class MULT : public Instruction {
 public:
  enum class OP2 {
    belt, se_imm
  };
  OP2 op2;
  MULT(MULT::OP2 op2) : op2(op2) {}

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
    std::ostringstream ss;
    ss << "MULT B" << field1(IR);
    switch(op2) {
      case OP2::belt:
        ss << " B" << field2(IR);
        break;
      case OP2::se_imm:
        ss << " " << toHexString(4, field4(IR));
        break;
    }
    return ss.str();
  }
};
static MULT instr_mult(MULT::OP2::belt);
static MULT instr_multi(MULT::OP2::se_imm);

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
  string getMnemonic(StorageObject& IR) { return "STORE (NOT IMPLEMENTED)"; }
};
static STORE instr_store;

Instruction& field1_01_field4_ff_field3_7(StorageObject& IR) {
  switch(field2(IR)) {
    case 0x0: return instr_halt;
    case 0x1: return instr_nop;
    case 0x2: // TODO: return
    case 0x3: // TODO: get_pc
    default: return instr_invalid;
  }
}

Instruction& field1_01_field4_ff(StorageObject& IR) {
  switch(field3(IR)) {
    case 0x0: // TODO: invert
    case 0x1: // TODO: negate
    case 0x2: // TODO: longcall
    case 0x3: // TODO: longjump
    case 0x4: // TODO: get_carry
    case 0x7: return field1_01_field4_ff_field3_7(IR);
    default: return instr_invalid;
  }
}

Instruction& field1_10(StorageObject& IR) {
  switch(field3(IR)) {
    case 0x0: return instr_addi;
    case 0x1: return instr_multi;
    case 0x2: return instr_andi;
    case 0x3: return instr_ori;
    case 0x4: return instr_xori;
    case 0x5: return instr_lshifti;
    case 0x6: return instr_rshifti;
    case 0x7: return instr_arshifti;
    default: return instr_invalid;
  }
}

Instruction& field1_01(StorageObject& IR) {
  switch(field4(IR)) {
    case 0x00: return instr_add;
    case 0x01: return instr_mult;
    case 0x02: return instr_and_;
    case 0x03: return instr_or_;
    case 0x04: return instr_xor_;
    case 0x05: return instr_lshift;
    case 0x06: return instr_rshift;
    case 0x07: return instr_arshift;
    case 0x08: return instr_addc;
    case 0x09: return instr_sub;
    case 0xFF: return field1_01_field4_ff(IR);
    default: return instr_invalid;
  }
}

Instruction& decode(StorageObject& IR) {
  switch(field1(IR)) {
    case 0b00: return instr_store;
    case 0b01: return field1_01(IR);
    case 0b10: return field1_10(IR);
    case 0b11:
      // look at field 3 + field 2
      // TODO
      return instr_invalid;  // TODO: decode
  }
}
