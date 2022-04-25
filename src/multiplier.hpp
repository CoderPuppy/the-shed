// multiplier.hpp
// Author: Drew Young (ajy2158@rit.edu)
//
// Implementation of a multiplier. 16-bit times 16-bit to 32-bit output on
// two 16-bit registers.

#ifndef multiplier_hpp
#define multiplier_hpp

#include "includes.hpp"

void mult_connect();
void mult_setup(StorageObject& op);
void mult_tick0(StorageObject& op);
void mult_tick1();
void mult_tick2();
void mult_tick3();
void mult_tick4();
void mult_tick5();

#endif
