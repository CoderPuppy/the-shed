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
      return "LSHIFT";
    case BusALU::op_rshift:
      return "RSHIFT";
    case BusALU::op_rashift:
      return "RASHIFT";
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

string toHexString(int fill, long value) {
  std::ostringstream ss;
  ss << "0x" << std::right << std::hex << std::uppercase << std::setfill('0')
     << std::setw(fill) << value;
  return ss.str();
}
