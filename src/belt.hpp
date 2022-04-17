#ifndef belt_hpp
#define belt_hpp

#include "includes.hpp"

class BeltElement {
 public:
  StorageObject data;
  Clearable carry;
  Clearable oflow;
  BeltElement(int number);
  void connectsTo(Flow& f);
  string toString();
};

class Belt {
 public:
  const int length;
  Belt(int length);
  void shift(int amount);
  void shift();
  BeltElement& get(int index);
  void connectsTo(Flow& f);
  void addToBelt(OutFlow& dataFlow, OutFlow& carryFlow, OutFlow& overflowFlow);
  void addToBelt(OutFlow& dataFlow);
  string toString();

 private:
  int offset;
  BeltElement *belt;
};

#endif
