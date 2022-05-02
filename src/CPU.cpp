// CPU.cpp
// Author: Max Kipust (mak4819@rit.edu)
// Author: Drew Young (ajy2158@rit.edu)
//
// Simulator entry point

#include "includes.hpp"

void setup(char *file, char *data_file) {
  instr_mem.load(file);
  if (data_file != NULL) {
    data_mem.load(data_file);
  }

  prog_cnt.latchFrom(instr_mem.READ());
  Clock::tick();
  trace_header();
}

int main(int argc, char *argv[]) {
  // turn on data flow tracing and memory dump after load

  // CPUObject::debug |= CPUObject::trace | CPUObject::memload;

  // make sure we've been given an object file name
  if (argc < 2 && argc > 3) {
    cerr << "Usage:  " << argv[0]
         << " program-object-file-name [heap-object-file-name]" << endl
         << endl;
    exit(1);
  }

  cout << hex;  // change base for future printing

  try {
    connect();
    if (argc == 2) {
      setup(argv[1], NULL);
    } else {
      setup(argv[1], argv[2]);
    }
    executeLoop();
    switch (programState) {
      case State::running:
        cout << endl
             << "SHED halted due to unknown error (RUNNING)" << endl;
        break;
      case State::halting:
        cout << endl
             << "SHED halted due to unknown error (HALTING)" << endl;
        break;
      case State::halted:
        cout << endl << "SHED halted due to HALT instruction" << endl;
        break;
      case State::invalid_opcode:
        cout << endl << "SHED halted due to an invalid opcode" << endl;
        break;
      case State::stack_overflow:
        cout << endl << "SHED halted due to a stack overflow" << endl;
        break;
      case State::pc_overflow:
        cout << endl << "SHED halted due to PC overflow" << endl;
        break;
      case State::heap_overflow:
        cout << endl << "SHED halted due to a heap overflow" << endl;
        break;
      case State::invalid_stack_access:
        cout << endl
             << "SHED halted due to an invalid stack access" << endl;
        break;
      case State::stack_underflow:
        cout << endl
             << "SHED halted due to a stack underflow" << endl;
        break;
      default:
        cout << endl << "SHED halted due to unknown reason" << endl;
        break;
    }
  }

  catch (ArchLibError &err) {
    cout << endl
         << "Simulation aborted - ArchLib runtime error" << endl
         << "Cause:  " << err.what() << endl;
    return (1);
  }

  return (0);
}
