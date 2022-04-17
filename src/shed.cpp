#include "includes.hpp"

void setup(char *file) {
  instr_mem.load(file);

  prog_cnt.latchFrom(instr_mem.READ());
  Clock::tick();
  cout << " ADDR  | ";
  cout << "X1                 | ";
  cout << "X2                 | ";
  cout << "X3                 | ";
  cout << "X4                 | ";
  cout << "BELT       ";
  cout << "\n";
  cout << std::string(188, '-');
  cout << "\n";
}

int main(int argc, char *argv[]) {
  // turn on data flow tracing and memory dump after load

  // CPUObject::debug |= CPUObject::trace | CPUObject::memload;

  // make sure we've been given an object file name

  if (argc != 2) {
    cerr << "Usage:  " << argv[0] << " object-file-name\n\n";
    exit(1);
  }

  cout << hex;  // change base for future printing

  try {
    connect();
    setup(argv[1]);
    executeLoop();
    switch (programState) {
      case HALTED:
        cout << "\nSHED halted due to HALT instruction\n";
        break;
      default:
        cout << "\nSHED halted due to unknown reason\n";
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
