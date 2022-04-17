#ifndef instruction_hpp
#define instruction_hpp

#include "includes.hpp"

class Instruction {
 public:
  virtual void X1T1() {}
  virtual void X1T2() {}
  virtual void X2T1() {}
  virtual void X2T2() {}
  virtual void X3T1() {}
  virtual void X3T2() {}
  virtual void X4T1() {}
  virtual void X4T2() {}
  virtual std::string getMnemonic(StorageObject& IR) = 0;
};

Instruction& decode(StorageObject& IR);

#endif
