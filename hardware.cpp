#include <StorageObject.h>
#include <Counter.h>
#include <Clearable.h>
#include <Memory.h>
#include <BusALU.h>
#include <Bus.h>

#include "hardware.h"

// Belt
StorageObject belt_0("Belt0", BITS);
StorageObject belt_1("Belt1", BITS);
StorageObject belt_2("Belt2", BITS);
StorageObject belt_3("Belt3", BITS);
StorageObject belt_4("Belt4", BITS);
StorageObject belt_5("Belt5", BITS);
StorageObject belt_6("Belt6", BITS);
StorageObject belt_7("Belt7", BITS);
StorageObject *belt[BELT_SIZE] = {
	&belt_0,
	&belt_1,
	&belt_2,
	&belt_3,
	&belt_4,
	&belt_5,
	&belt_6,
	&belt_7,
};
Clearable belt_carry_0("BeltCarry0", BITS);
Clearable belt_carry_1("BeltCarry1", BITS);
Clearable belt_carry_2("BeltCarry2", BITS);
Clearable belt_carry_3("BeltCarry3", BITS);
Clearable belt_carry_4("BeltCarry4", BITS);
Clearable belt_carry_5("BeltCarry5", BITS);
Clearable belt_carry_6("BeltCarry6", BITS);
Clearable belt_carry_7("BeltCarry7", BITS);
Clearable *belt_carry[BELT_SIZE] = {
	&belt_carry_0,
	&belt_carry_1,
	&belt_carry_2,
	&belt_carry_3,
	&belt_carry_4,
	&belt_carry_5,
	&belt_carry_6,
	&belt_carry_7,
};
Clearable belt_oflow_0("BeltOflow0", BITS);
Clearable belt_oflow_1("BeltOflow1", BITS);
Clearable belt_oflow_2("BeltOflow2", BITS);
Clearable belt_oflow_3("BeltOflow3", BITS);
Clearable belt_oflow_4("BeltOflow4", BITS);
Clearable belt_oflow_5("BeltOflow5", BITS);
Clearable belt_oflow_6("BeltOflow6", BITS);
Clearable belt_oflow_7("BeltOflow7", BITS);
Clearable *belt_oflow[BELT_SIZE] = {
	&belt_oflow_0,
	&belt_oflow_1,
	&belt_oflow_2,
	&belt_oflow_3,
	&belt_oflow_4,
	&belt_oflow_5,
	&belt_oflow_6,
	&belt_oflow_7,
};

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
StorageObject instr_reg_X1("IR X1", BITS);
Bus instr_reg_X1_bus("IR X1 Bus", BITS);
StorageObject instr_reg_X2("IR X2", BITS);
Bus instr_reg_X2_bus("IR X2 Bus", BITS);
StorageObject instr_reg_X3("IR X3", BITS);
Bus instr_reg_X3_bus("IR X3 Bus", BITS);
StorageObject instr_reg_X4("IR X4", BITS);
StorageObject imm_X1("Imm X1", BITS);
Bus imm_X1_bus("Imm X1 Bus", BITS);
StorageObject imm_X2("Imm X2", BITS);
StorageObject prog_cnt_X1("PC X1", BITS);
StorageObject data_reg("Data", BITS);
Bus data_reg_bus("Data Bus", BITS);
StorageObject addr_reg("Addr", BITS);
Bus addr_reg_bus("Addr Bus", BITS);

// ALUs
BusALU sign_ext("sign_ext", BITS);
BusALU alu1("ALU1", BITS);
BusALU alu2("ALU2", BITS);
Bus zero_ext("zero_ext", IMM_BITS);

void connect(void) {
	for(int i = 0; i < BELT_SIZE; i++) {
		belt[i]->connectsTo(data_reg_bus.IN());
		belt[i]->connectsTo(alu1.OP1());
		belt[i]->connectsTo(alu1.OP2());
		belt[i]->connectsTo(alu1.OUT());

		belt_carry[i]->connectsTo(alu1.OP2());
		belt_carry[i]->connectsTo(alu1.CARRY());

		belt_oflow[i]->connectsTo(alu1.OP2());
		belt_oflow[i]->connectsTo(alu1.OFLOW());
	}

	prog_cnt.connectsTo(prog_cnt_bus.IN());
	prog_cnt.connectsTo(ret_addr_bus.OUT());
	prog_cnt.connectsTo(alu1.OUT());

	ret_addr.connectsTo(ret_addr_bus.IN());
	ret_addr.connectsTo(alu1.OUT());
	ret_addr.connectsTo(stack_mem.READ());
	ret_addr.connectsTo(stack_mem.WRITE());

	frame_ptr.connectsTo(alu1.OP1());
	frame_ptr.connectsTo(alu1.OUT());
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
}
