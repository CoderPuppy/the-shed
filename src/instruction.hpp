#ifndef instruction_hpp
#define instruction_hpp

#include "includes.hpp"

class Instruction {
 public:
  virtual void X1T1() = 0;
  virtual void X1T2() = 0;
  virtual void X2T1() = 0;
  virtual void X2T2() = 0;
  virtual void X3T1() = 0;
  virtual void X3T2() = 0;
  virtual void X4T1() = 0;
  virtual void X4T2() = 0;
  virtual std::string getMnemonic() = 0;
};

Instruction* decode(StorageObject& IR);

#endif