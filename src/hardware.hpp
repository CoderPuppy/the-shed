// hardware.h
// Author: Drew Young (ajy2158@rit.edu)
//
// Hardware definition

#ifndef HARDWARE_HPP
#define HARDWARE_HPP

#include "includes.hpp"

static const unsigned int IMM_BITS(8);
static const unsigned int BITS(16);

// Belt
static const unsigned int BELT_SIZE(8);
extern Belt belt;

// Misc registers
extern Counter prog_cnt;
extern Bus prog_cnt_bus;
extern StorageObject ret_addr;
extern Bus ret_addr_bus;
extern Counter frame_ptr;
extern StorageObject ret_frame_ptr;
extern Bus ret_frame_ptr_bus;
extern Counter stack_ptr;
extern StorageObject cmp;

// Memories
extern Memory instr_mem;
extern Memory data_mem;
extern Memory stack_mem;

// Pipeline registers
extern StorageObject instr_reg_X1;
extern Bus instr_reg_X1_bus;
extern StorageObject instr_reg_X2;
extern Bus instr_reg_X2_bus;
extern StorageObject instr_reg_X3;
extern Bus instr_reg_X3_bus;
extern StorageObject instr_reg_X4;
extern StorageObject imm_X1;
extern Bus imm_X1_bus;
extern StorageObject imm_X2;
extern StorageObject prog_cnt_X1;
extern StorageObject data_reg;
extern Bus data_reg_bus;
extern StorageObject addr_reg;
extern Bus addr_reg_bus;

// ALUs
extern BusALU sign_ext;
extern BusALU alu1;
extern BusALU alu2;
extern Bus zero_ext;

// consts
extern StorageObject nop_instr;
// extern StorageObject max_value;
extern Bus const_bus;
extern StorageObject sign_ext_mask;
extern StorageObject negate_mask;

enum STATE_ENUM { RUNNING, HALTING, HALTED, INVALID_OPCODE };
extern STATE_ENUM programState;

void connect(void);

#endif