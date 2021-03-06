// instruction.hpp
// Author: Max Kipust (mak4819@rit.edu)
// Author: Drew Young (ajy2158@rit.edu)
//
// Interface for instruction decode and execution

#ifndef instruction_hpp
#define instruction_hpp

#include "includes.hpp"

class Instruction {
 public:
  // please don't do anything bad in the C methods
  // they are only for checking control conditions

  virtual void X1T1() {}
  virtual void X1T2() {}
  virtual void X1C() {}

  virtual void X2T1() {}
  virtual void X2T2() {}
  virtual void X2C() {}

  virtual void X3T1() {}
  virtual void X3T2() {}
  virtual void X3C() {}

  virtual void X4T1() {}
  virtual void X4T2() {}
  virtual void X4C() {}

  virtual void print(ostream& s) = 0;
  virtual int getLatency() = 0;
};

unique_ptr<Instruction> decode(StorageObject& IR);

#endif
