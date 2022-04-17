#include "includes.hpp"

BeltElement::BeltElement(int number)
  : data(("BELT_" + std::to_string(number)).c_str(), BITS)
  , carry(("BELT_CARRY_" + std::to_string(number)).c_str(), BITS)
  , oflow(("BELT_OFLOW_" + std::to_string(number)).c_str(), BITS)
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
  : length(length), offset(0)
  , belt((BeltElement*) calloc(length, sizeof(BeltElement))) {
  for(int i = 0; i < length; i++) {
    new (&belt[i]) BeltElement(i);
  }
}

void Belt::shift(int amount) {
  offset = (offset - amount) % length;
}

void Belt::shift() { shift(1); }

BeltElement& Belt::get(int index) {
  int i = (index + offset) % length;
  if(i < 0) {
    i = length + i;
  }
  return belt[i];
}

void Belt::connectsTo(Flow& flow) {
  for(int i = 0; i < length; i++) {
    belt[i].connectsTo(flow);
  }
}

void Belt::addToBelt(OutFlow& dataFlow, OutFlow& carryFlow,
                     OutFlow& overflowFlow) {
  shift();
  get(0).data.latchFrom(dataFlow);
  get(0).carry.latchFrom(carryFlow);
  get(0).oflow.latchFrom(overflowFlow);
}

void Belt::addToBelt(OutFlow& dataFlow) {
  shift();
  get(0).data.latchFrom(dataFlow);
  get(0).carry.clear();
  get(0).oflow.clear();
}

string Belt::toString() {
  std::ostringstream ss;
  for(int i = 0; i < length; i++) {
    ss << std::left << std::setfill(' ') << std::setw(9) << get(i).toString()
       << " | ";
  }
  return ss.str();
}
