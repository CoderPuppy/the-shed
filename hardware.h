// hardware.h
// Author: Drew Young (ajy2158@rit.edu)
//
// Hardware definition

#ifndef _HARDWARE_H
#define _HARDWARE_H

#include <StorageObject.h>
#include <Counter.h>
#include <Clearable.h>
#include <Memory.h>
#include <BusALU.h>
#include <Bus.h>

static const unsigned int IMM_BITS(8);
static const unsigned int BITS(16);

// Belt
static const unsigned int BELT_SIZE(8);
extern StorageObject belt_0;
extern StorageObject belt_1;
extern StorageObject belt_2;
extern StorageObject belt_3;
extern StorageObject belt_4;
extern StorageObject belt_5;
extern StorageObject belt_6;
extern StorageObject belt_7;
extern StorageObject *belt[BELT_SIZE];
extern Clearable belt_carry_0;
extern Clearable belt_carry_1;
extern Clearable belt_carry_2;
extern Clearable belt_carry_3;
extern Clearable belt_carry_4;
extern Clearable belt_carry_5;
extern Clearable belt_carry_6;
extern Clearable belt_carry_7;
extern Clearable *belt_carry[BELT_SIZE];
extern Clearable belt_oflow_0;
extern Clearable belt_oflow_1;
extern Clearable belt_oflow_2;
extern Clearable belt_oflow_3;
extern Clearable belt_oflow_4;
extern Clearable belt_oflow_5;
extern Clearable belt_oflow_6;
extern Clearable belt_oflow_7;
extern Clearable *belt_oflow[BELT_SIZE];

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

void connect(void);

#endif
