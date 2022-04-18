// shed.cpp
// Author: Max Kipust (mak4819@rit.edu)
// Author: Drew Young (ajy2158@rit.edu)
//
// Simulator entry point

#include "includes.hpp"

void setup(char *file) {
  instr_mem.load(file);

  prog_cnt.latchFrom(instr_mem.READ());
  Clock::tick();
  trace_header();
}

int main(int argc, char *argv[]) {
  // turn on data flow tracing and memory dump after load

  // CPUObject::debug |= CPUObject::trace | CPUObject::memload;

  // make sure we've been given an object file name

  if (argc != 2) {
    cerr << "Usage:  " << argv[0] << " object-file-name" << endl << endl;
    exit(1);
  }

  cout << hex;  // change base for future printing

  try {
    connect();
    setup(argv[1]);
    executeLoop();
    switch (programState) {
      case HALTED:
        cout << endl << "SHED halted due to HALT instruction" << endl;
        break;
      case INVALID_OPCODE:
        cout << endl << "SHED halted due to an invalid opcode" << endl;
        break;
      default:
        cout << endl << "SHED halted due to unknown reason" << endl;
        break;
    }
    data_mem.dump(0, 20);
  }

  catch (ArchLibError &err) {
    cout << endl
         << "Simulation aborted - ArchLib runtime error" << endl
         << "Cause:  " << err.what() << endl;
    return (1);
  }

  return (0);
}
