#ifndef belt_hpp
#define belt_hpp

#include "includes.hpp"

class BeltElement {
 public:
  StorageObject data;
  Clearable carry;
  Clearable oflow;
  BeltElement(int index);
  void connectsTo(Flow& f);
  string toString();
};

class Belt {
 public:
  int length;
  Belt(int length);
  void shift(int amount);
  void shift();
  BeltElement& get(int index);
  void connectsTo(Flow& f);
  void push(OutFlow& data, OutFlow& carry, OutFlow& oflow);
  void push(OutFlow& data);
  string toString();

 private:
  int offset;
  BeltElement *belt;
};

#endif
