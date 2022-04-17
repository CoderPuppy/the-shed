#include "includes.hpp"

BeltElement::BeltElement(int number) {
  std::string index = std::to_string(number);
  std::string beltname = "BELT_" + index;
  std::string carryname = "BELTCARRY_" + index;
  std::string overflowname = "BELT_OFLOW" + index;

  // StorageObject dataTmp(beltname.c_str(), BITS);
  // Clearable carryTmp(carryname.c_str(), 1);
  // Clearable oflowTmp(overflowname.c_str(), 1);

  data = new StorageObject(beltname.c_str(), BITS);
  carry = new Clearable(carryname.c_str(), 1);
  oflow = new Clearable(overflowname.c_str(), 1);
}

void BeltElement::connectsTo(Flow& f) {
  data->connectsTo(f);
  carry->connectsTo(f);
  oflow->connectsTo(f);
}

string BeltElement::toString() {
  std::ostringstream ss;
  ss << "0x" << std::right << std::hex << std::uppercase << std::setfill('0')
     << std::setw(4) << data->value() << " " << (carry->value() ? "C" : "")
     << (oflow->value() ? "O" : "");
  return ss.str();
}

Belt::Belt(int elements) {
  numberOfElements = elements;
  offset = 0;
  for (int i = 0; i < elements; i++) {
    belt.push_back(new BeltElement(i));
  }
}

void Belt::shift(int amount) { offset = (offset - amount) % numberOfElements; }

void Belt::shift() { shift(1); }

BeltElement* Belt::get(int index) {
  int i = (index + offset) % numberOfElements;
  if (i < 0) {
    i = numberOfElements + i;
  }
  return belt[i];
}

void Belt::connectsTo(Flow& flow) {
  for (int i = 0; i < numberOfElements; i++) {
    belt[i]->connectsTo(flow);
  }
}

void Belt::addToBelt(OutFlow& dataFlow, OutFlow& carryFlow,
                     OutFlow& overflowFlow) {
  shift();
  get(0)->data->latchFrom(dataFlow);
  get(0)->carry->latchFrom(carryFlow);
  get(0)->oflow->latchFrom(overflowFlow);
}

void Belt::addToBelt(OutFlow& dataFlow) {
  shift();
  get(0)->data->latchFrom(dataFlow);
  get(0)->carry->clear();
  get(0)->oflow->clear();
}

string Belt::toString() {
  std::ostringstream ss;
  for (int i = 0; i < numberOfElements; i++) {
    ss << std::left << std::setfill(' ') << std::setw(9) << get(i)->toString()
       << " | ";
  }
  return ss.str();
}