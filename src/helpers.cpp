// helpers.cpp
// Author: Max Kipust (mak4819@rit.edu)
//
// Various helpers

#include "includes.hpp"

string opToString(BusALU::Operation op) {
  switch (op) {
    case BusALU::op_none:
      return "NONE";
    case BusALU::op_add:
      return "ADD";
    case BusALU::op_sub:
      return "SUB";
    case BusALU::op_and:
      return "AND";
    case BusALU::op_or:
      return "OR";
    case BusALU::op_xor:
      return "XOR";
    case BusALU::op_not:
      return "NOT";
    case BusALU::op_extendSign:
      return "ESIGN";
    case BusALU::op_lshift:
      return "SLL";
    case BusALU::op_rshift:
      return "SRL";
    case BusALU::op_rashift:
      return "SRA";
    case BusALU::op_rop1:
      return "OP1";
    case BusALU::op_rop2:
      return "OP2";
    case BusALU::op_zero:
      return "ZERO";
    case BusALU::op_one:
      return "ONE";
    default:
      return "UNKNOWN";
  }
}
