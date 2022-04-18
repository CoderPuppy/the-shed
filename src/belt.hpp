// belt.hpp
// Author: Max Kipust (mak4819@rit.edu)
// Author: Drew Young (ajy2158@rit.edu)
//
// Belt interface

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
  void print(ostream& s);
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
  void print(ostream& s);
  void tick();

 private:
  int offset, pending_offset;
  BeltElement *belt;
};

#endif
