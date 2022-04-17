#include "includes.hpp"

StorageObject mult_reg("Mult", BITS);
Bus mult_reg_bus("Mult bus", BITS);

BusALU mult_alu_01("MultALU01", BITS * 2);
BusALU mult_alu_02("MultALU02", BITS * 2);
BusALU mult_alu_03("MultALU03", BITS * 2);
BusALU mult_alu_04("MultALU04", BITS * 2);
BusALU mult_alu_05("MultALU05", BITS * 2);
BusALU mult_alu_06("MultALU06", BITS * 2);
BusALU mult_alu_07("MultALU07", BITS * 2);
BusALU mult_alu_08("MultALU08", BITS * 2);
BusALU mult_alu_09("MultALU09", BITS * 2);
BusALU mult_alu_0A("MultALU0A", BITS * 2);
BusALU mult_alu_0B("MultALU0B", BITS * 2);
BusALU mult_alu_0C("MultALU0C", BITS * 2);
BusALU mult_alu_0D("MultALU0D", BITS * 2);
BusALU mult_alu_0E("MultALU0E", BITS * 2);
BusALU mult_alu_0F("MultALU0F", BITS * 2);
BusALU *mult_alu_0[BITS - 1] = {
    &mult_alu_01, &mult_alu_02, &mult_alu_03, &mult_alu_04, &mult_alu_05,
    &mult_alu_06, &mult_alu_07, &mult_alu_08, &mult_alu_09, &mult_alu_0A,
    &mult_alu_0B, &mult_alu_0C, &mult_alu_0D, &mult_alu_0E, &mult_alu_0F,
};

StorageObject mult_reg_00("MultReg00", BITS * 2);
Bus mult_reg_00_bus("MultReg00 bus", BITS);
StorageObject mult_reg_01("MultReg01", BITS * 2);
StorageObject mult_reg_02("MultReg02", BITS * 2);
StorageObject mult_reg_03("MultReg03", BITS * 2);
StorageObject mult_reg_04("MultReg04", BITS * 2);
StorageObject mult_reg_05("MultReg05", BITS * 2);
StorageObject mult_reg_06("MultReg06", BITS * 2);
StorageObject mult_reg_07("MultReg07", BITS * 2);
StorageObject mult_reg_08("MultReg08", BITS * 2);
StorageObject mult_reg_09("MultReg09", BITS * 2);
StorageObject mult_reg_0A("MultReg0A", BITS * 2);
StorageObject mult_reg_0B("MultReg0B", BITS * 2);
StorageObject mult_reg_0C("MultReg0C", BITS * 2);
StorageObject mult_reg_0D("MultReg0D", BITS * 2);
StorageObject mult_reg_0E("MultReg0E", BITS * 2);
StorageObject mult_reg_0F("MultReg0F", BITS * 2);
StorageObject *mult_reg_0[BITS] = {
    &mult_reg_00, &mult_reg_01, &mult_reg_02, &mult_reg_03,
    &mult_reg_04, &mult_reg_05, &mult_reg_06, &mult_reg_07,
    &mult_reg_08, &mult_reg_09, &mult_reg_0A, &mult_reg_0B,
    &mult_reg_0C, &mult_reg_0D, &mult_reg_0E, &mult_reg_0F,
};

BusALU mult_alu_10("MultALU10", BITS * 2);
BusALU mult_alu_11("MultALU11", BITS * 2);
BusALU mult_alu_12("MultALU12", BITS * 2);
BusALU mult_alu_13("MultALU13", BITS * 2);
BusALU mult_alu_14("MultALU14", BITS * 2);
BusALU mult_alu_15("MultALU15", BITS * 2);
BusALU mult_alu_16("MultALU16", BITS * 2);
BusALU mult_alu_17("MultALU17", BITS * 2);
BusALU *mult_alu_1[BITS / 2] = {
    &mult_alu_10, &mult_alu_11, &mult_alu_12, &mult_alu_13,
    &mult_alu_14, &mult_alu_15, &mult_alu_16, &mult_alu_17,
};

StorageObject mult_reg_10("MultReg10", BITS * 2);
StorageObject mult_reg_11("MultReg11", BITS * 2);
StorageObject mult_reg_12("MultReg12", BITS * 2);
StorageObject mult_reg_13("MultReg13", BITS * 2);
StorageObject mult_reg_14("MultReg14", BITS * 2);
StorageObject mult_reg_15("MultReg15", BITS * 2);
StorageObject mult_reg_16("MultReg16", BITS * 2);
StorageObject mult_reg_17("MultReg17", BITS * 2);
StorageObject *mult_reg_1[BITS / 2] = {
    &mult_reg_10, &mult_reg_11, &mult_reg_12, &mult_reg_13,
    &mult_reg_14, &mult_reg_15, &mult_reg_16, &mult_reg_17,
};

BusALU mult_alu_20("MultALU20", BITS * 2);
BusALU mult_alu_21("MultALU21", BITS * 2);
BusALU mult_alu_22("MultALU22", BITS * 2);
BusALU mult_alu_23("MultALU23", BITS * 2);
BusALU *mult_alu_2[BITS / 4] = {
    &mult_alu_20,
    &mult_alu_21,
    &mult_alu_22,
    &mult_alu_23,
};

StorageObject mult_reg_20("MultReg20", BITS * 2);
StorageObject mult_reg_21("MultReg21", BITS * 2);
StorageObject mult_reg_22("MultReg22", BITS * 2);
StorageObject mult_reg_23("MultReg23", BITS * 2);
StorageObject *mult_reg_2[BITS / 4] = {
    &mult_reg_20,
    &mult_reg_21,
    &mult_reg_22,
    &mult_reg_23,
};

BusALU mult_alu_30("MultALU30", BITS * 2);
BusALU mult_alu_31("MultALU31", BITS * 2);
BusALU *mult_alu_3[BITS / 8] = {
    &mult_alu_30,
    &mult_alu_31,
};

StorageObject mult_reg_30("MultReg30", BITS * 2);
StorageObject mult_reg_31("MultReg31", BITS * 2);
StorageObject *mult_reg_3[BITS / 8] = {
    &mult_reg_30,
    &mult_reg_31,
};

BusALU mult_alu_4("MultALU4", BITS * 2);
StorageObject mult_reg_4("MultReg4", BITS * 2);
Bus mult_reg_4_bus("MultReg4 bus", BITS);

BusALU mult_alu_5("MultALU5", BITS * 2);

void mult_connect(void) {
  belt.connectsTo(mult_reg_bus.IN());
  belt.connectsTo(mult_reg_00_bus.IN());
  for(int j = 0; j < BITS - 1; j++) belt.connectsTo(mult_alu_0[j]->OP1());
  belt.connectsTo(mult_reg_4_bus.OUT());
  belt.connectsTo(mult_alu_5.OUT());

  imm_X1.connectsTo(mult_reg_00_bus.IN());
  for(int i = 0; i < BITS - 1; i++) {
    imm_X1.connectsTo(mult_alu_0[i]->OP1());
    mult_reg_0[i + 1]->connectsTo(mult_alu_0[i]->OUT());
  }

  for(int i = 0; i < BITS; i++) {
    BusALU *alu = mult_alu_1[i / 2];
    mult_reg_0[i]->connectsTo(i % 2 == 0 ? alu->OP1() : alu->OP2());
  }
  for(int i = 0; i < BITS / 2; i++) {
    mult_reg_1[i]->connectsTo(mult_alu_1[i]->OUT());

    BusALU *alu = mult_alu_2[i / 2];
    mult_reg_1[i]->connectsTo(i % 2 == 0 ? alu->OP1() : alu->OP2());
  }
  for(int i = 0; i < BITS / 4; i++) {
    mult_reg_2[i]->connectsTo(mult_alu_2[i]->OUT());

    BusALU *alu = mult_alu_3[i / 2];
    mult_reg_2[i]->connectsTo(i % 2 == 0 ? alu->OP1() : alu->OP2());
  }
  for(int i = 0; i < BITS / 8; i++) {
    mult_reg_3[i]->connectsTo(mult_alu_3[i]->OUT());
    mult_reg_3[i]->connectsTo(i % 2 == 0 ? mult_alu_4.OP1() : mult_alu_4.OP2());
  }
  mult_reg_4.connectsTo(mult_alu_4.OUT());
  mult_reg_4.connectsTo(mult_alu_5.OP1());
}
