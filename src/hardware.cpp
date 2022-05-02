// hardware.cpp
// Author: Drew Young (ajy2158@rit.edu)
// Author: Max Kipust (mak4819@rit.edu)
//
// Hardware definition

#include "includes.hpp"

static const long nop_instr(0b0100111111111111);

// Belt
Belt belt(BELT_SIZE);

// Misc registers
Counter prog_cnt("PC", BITS);
Bus prog_cnt_bus("PC Bus", BITS);
StorageObject ret_addr("RA", BITS);
Bus ret_addr_bus("RA Bus", BITS);
Counter frame_ptr("FP", BITS);
StorageObject ret_frame_ptr("RFP", BITS);
Bus ret_frame_ptr_bus("RFP Bus", BITS);
Counter stack_ptr("SP", BITS);
StorageObject cmp("Cmp", BITS);

// Memories
Memory instr_mem("IMem", BITS, BITS);
Memory data_mem("DMem", BITS, BITS);
Memory stack_mem("SMem", BITS, BITS);

// Pipeline registers
StorageObject instr_reg_X1("IR X1", BITS, nop_instr);
Bus instr_reg_X1_bus("IR X1 Bus", BITS);
StorageObject instr_reg_X2("IR X2", BITS, nop_instr);
Bus instr_reg_X2_bus("IR X2 Bus", BITS);
StorageObject instr_reg_X3("IR X3", BITS, nop_instr);
Bus instr_reg_X3_bus("IR X3 Bus", BITS);
StorageObject instr_reg_X4("IR X4", BITS, nop_instr);
StorageObject imm_X1("Imm X1", BITS);
Bus imm_X1_bus("Imm X1 Bus", BITS);
StorageObject imm_X2("Imm X2", BITS);
StorageObject prog_cnt_X1("PC X1", BITS);
StorageObject data_reg("Data", BITS);
Bus data_reg_bus("Data Bus", BITS);
StorageObject addr_reg("Addr", BITS);
Bus addr_reg_bus("Addr Bus", BITS);

// ALUs
BusALU alu1("ALU1", BITS);
Clearable alu1_flag("ALU1 carry", 1);
BusALU alu2("ALU2", BITS);
Clearable alu2_flag("ALU2 carry", 1);
BusALU sign_ext("sign_ext", BITS);
Bus zero_ext("zero_ext", IMM_BITS);

// consts
Bus const_bus("Const Bus", BITS);
StorageObject const_nop("nop_instruction", BITS, nop_instr);
StorageObject const_sign_ext_mask("sign_ext_mask", BITS,
                                  1u << (IMM_BITS - 1));
StorageObject const_0("0", BITS, 0);
StorageObject const_1("1", BITS, 1);
StorageObject const_2("2", BITS, 2);
StorageObject const_8("8", BITS, 8);

void connect(void) {
  mult_connect();

  belt.connectsTo(data_reg_bus.IN());
  belt.connectsTo(alu1.OP1());
  belt.connectsTo(alu1.OP2());
  belt.connectsTo(alu1.OUT());
  belt.connectsTo(alu1.CARRY());
  belt.connectsTo(alu1.OFLOW());
  belt.connectsTo(data_mem.READ());
  belt.connectsTo(stack_mem.READ());

  prog_cnt.connectsTo(prog_cnt_bus.IN());
  prog_cnt.connectsTo(ret_addr_bus.OUT());
  prog_cnt.connectsTo(alu1.OUT());
  prog_cnt.connectsTo(instr_mem.READ());

  ret_addr.connectsTo(ret_addr_bus.IN());
  ret_addr.connectsTo(alu1.OUT());
  ret_addr.connectsTo(stack_mem.READ());
  ret_addr.connectsTo(stack_mem.WRITE());

  frame_ptr.connectsTo(alu1.OP1());
  frame_ptr.connectsTo(alu2.OUT());
  frame_ptr.connectsTo(ret_frame_ptr_bus.OUT());

  ret_frame_ptr.connectsTo(ret_frame_ptr_bus.IN());
  ret_frame_ptr.connectsTo(alu2.OUT());
  ret_frame_ptr.connectsTo(stack_mem.READ());
  ret_frame_ptr.connectsTo(stack_mem.WRITE());

  stack_ptr.connectsTo(alu2.OP1());
  stack_ptr.connectsTo(alu2.OUT());

  cmp.connectsTo(alu2.OUT());

  instr_mem.MAR().connectsTo(prog_cnt_bus.OUT());

  data_mem.MAR().connectsTo(addr_reg_bus.OUT());

  stack_mem.MAR().connectsTo(addr_reg_bus.OUT());
  stack_mem.MAR().connectsTo(alu2.OUT());

  instr_reg_X1.connectsTo(instr_mem.READ());
  instr_reg_X1.connectsTo(instr_reg_X1_bus.IN());
  instr_reg_X1.connectsTo(zero_ext.IN());
  instr_reg_X1.connectsTo(sign_ext.OP1());
  instr_reg_X1.connectsTo(sign_ext.OP2());
  instr_reg_X1.connectsTo(const_bus.OUT());

  instr_reg_X2.connectsTo(instr_reg_X1_bus.OUT());
  instr_reg_X2.connectsTo(instr_reg_X2_bus.IN());

  instr_reg_X3.connectsTo(instr_reg_X2_bus.OUT());
  instr_reg_X3.connectsTo(instr_reg_X3_bus.IN());

  instr_reg_X4.connectsTo(instr_reg_X3_bus.OUT());

  imm_X1.connectsTo(zero_ext.OUT());
  imm_X1.connectsTo(sign_ext.OUT());
  imm_X1.connectsTo(imm_X1_bus.IN());
  imm_X1.connectsTo(alu1.OP1());
  imm_X1.connectsTo(alu1.OP2());

  imm_X2.connectsTo(imm_X1_bus.OUT());
  imm_X2.connectsTo(alu2.OP2());

  prog_cnt_X1.connectsTo(prog_cnt_bus.OUT());
  prog_cnt_X1.connectsTo(alu1.OP1());

  data_reg.connectsTo(data_reg_bus.OUT());
  data_reg.connectsTo(data_mem.WRITE());
  data_reg.connectsTo(stack_mem.WRITE());

  addr_reg.connectsTo(alu1.OUT());
  addr_reg.connectsTo(addr_reg_bus.IN());
  addr_reg.connectsTo(alu2.OP2());

  alu1_flag.connectsTo(alu1.CARRY());
  alu1_flag.connectsTo(alu1.OFLOW());
  alu2_flag.connectsTo(alu2.CARRY());
  alu2_flag.connectsTo(alu2.OFLOW());

  const_nop.connectsTo(const_bus.IN());
  const_sign_ext_mask.connectsTo(sign_ext.OP2());
  const_0.connectsTo(alu1.OP1());
  const_1.connectsTo(alu1.OP2());
  const_1.connectsTo(alu2.OP2());
  const_2.connectsTo(alu1.OP2());
  const_8.connectsTo(alu1.OP2());
}
