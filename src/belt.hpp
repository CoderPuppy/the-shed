#ifndef belt_hpp
#define belt_hpp

#include <vector>

class BeltElement {
 public:
  StorageObject* data;
  Clearable* carry;
  Clearable* oflow;
  BeltElement(int number);
  void connectsTo(Flow& f);
};

class Belt {
 public:
  int numberOfElements;
  Belt(int elements);
  void shift(int amount);
  void shift();
  BeltElement* get(int index);
  void connectsTo(Flow& f);
  void addToBelt(OutFlow& dataFlow, OutFlow& carryFlow, OutFlow& overflowFlow);
  void addToBelt(OutFlow& dataFlow);

 private:
  int offset;
  std::vector<BeltElement*> belt;
};

#endif