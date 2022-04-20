// hardware.hpp
// Author: Drew Young (ajy2158@rit.edu)
// Author: Max Kipust (mak4819@rit.edu)
//
// Hardware definition

#ifndef hardware_hpp
#define hardware_hpp

#include "includes.hpp"

static const unsigned int IMM_BITS(8);
static const unsigned int BITS(16);
typedef uint16_t data_t;
typedef uint8_t ze_imm_t;
typedef int8_t se_imm_t;

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
extern BusALU alu1;
extern StorageObject alu1_flag;
extern BusALU alu2;
extern StorageObject alu2_flag;
extern BusALU sign_ext;
extern Bus zero_ext;

// consts
extern Bus const_bus;
extern StorageObject const_nop;
extern StorageObject const_sign_ext_mask;
extern StorageObject const_0;
extern StorageObject const_1;
extern StorageObject const_2;

void connect(void);

#endif
