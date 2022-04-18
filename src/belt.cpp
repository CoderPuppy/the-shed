// belt.cpp
// Author: Max Kipust (mak4819@rit.edu)
// Author: Drew Young (ajy2158@rit.edu)
//
// Belt implementation

#include "includes.hpp"

BeltElement::BeltElement(int index)
  : data(("BELT_" + std::to_string(index)).c_str(), BITS)
  , carry(("BELT_CARRY_" + std::to_string(index)).c_str(), 1)
  , oflow(("BELT_OFLOW_" + std::to_string(index)).c_str(), 1)
{}

void BeltElement::connectsTo(Flow& f) {
  data.connectsTo(f);
  carry.connectsTo(f);
  oflow.connectsTo(f);
}

string BeltElement::toString() {
  std::ostringstream ss;
  ss << "0x" << std::right << std::hex << std::uppercase << std::setfill('0')
     << std::setw(4) << data.value() << " " << (carry.value() ? "C" : "")
     << (oflow.value() ? "O" : "");
  return ss.str();
}

Belt::Belt(int length)
  : length(length), offset(0), pending_offset(0)
  , belt((BeltElement*) ::operator new(length * sizeof(BeltElement)))
{
  for (int i = 0; i < length; i++) {
    new (&belt[i]) BeltElement(i);
  }
}

void Belt::shift(int amount) {
  pending_offset -= amount;
  pending_offset %= length;
  pending_offset += length;
  pending_offset %= length;
}

void Belt::shift() { shift(1); }

BeltElement& Belt::get(int index) {
  index += offset;
  index %= length;
  index += length;
  index %= length;
  return belt[index];
}

void Belt::connectsTo(Flow& flow) {
  for (int i = 0; i < length; i++) {
    belt[i].connectsTo(flow);
  }
}

void Belt::push(OutFlow& data, OutFlow& carry, OutFlow& oflow) {
  shift();
  belt[pending_offset].data.latchFrom(data);
  belt[pending_offset].carry.latchFrom(carry);
  belt[pending_offset].oflow.latchFrom(oflow);
}

void Belt::push(OutFlow& data) {
  shift();
  belt[pending_offset].data.latchFrom(data);
  belt[pending_offset].carry.clear();
  belt[pending_offset].oflow.clear();
}

string Belt::toString() {
  std::ostringstream ss;
  for (int i = 0; i < length; i++) {
    ss << std::left << std::setfill(' ') << std::setw(9) << get(i).toString()
       << " | ";
  }
  return ss.str();
}

void Belt::tick() {
  offset = pending_offset;
}
