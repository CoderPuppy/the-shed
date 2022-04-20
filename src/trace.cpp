// trace.cpp
// Author: Max Kipust (mak4819@rit.edu)
// Author: Drew Young (ajy2158@rit.edu)
//
// Functions to output a trace

#include "includes.hpp"

void trace_header() {
  cout << hex << uppercase << setfill('0');
  cout << "  PC  | ";
  cout << "X1                | ";
  cout << "X2                | ";
  cout << "X3                | ";
  cout << "X4                | ";
  cout << "BELT" << string(73, ' ') << " | ";
  cout << " RA  | ";
  cout << " FP  | ";
  cout << "RFP  | ";
  cout << " SP  | ";
  cout << endl;
  cout << string(195, '-') << endl;
}

void trace_cycle(Instruction& x1, Instruction& x2, Instruction& x3,
                 Instruction& x4) {
  ostringstream ss;
  ss << hex << uppercase << setfill('0');

  cout << " ";

  cout << setw(4) << prog_cnt_X1.value();

  cout << " | ";

  cout << (x1.getLatency() == 1 ? "*" : " ");
  ss.str(string());
  x1.print(ss);
  cout << ss.str() << string(16 - ss.str().size(), ' ');

  cout << " | ";

  cout << (x2.getLatency() == 2 ? "*" : " ");
  ss.str(string());
  x2.print(ss);
  cout << ss.str() << string(16 - ss.str().size(), ' ');

  cout << " | ";

  cout << (x3.getLatency() == 3 ? "*" : " ");
  ss.str(string());
  x3.print(ss);
  cout << ss.str() << string(16 - ss.str().size(), ' ');

  cout << " | ";

  cout << (x4.getLatency() == 4 ? "*" : " ");
  ss.str(string());
  x4.print(ss);
  cout << ss.str() << string(16 - ss.str().size(), ' ');

  cout << " | ";

  belt.print(cout);

  cout << " | ";

  cout << setw(4) << ret_addr.value();

  cout << " | ";

  cout << setw(4) << frame_ptr.value();

  cout << " | ";

  cout << setw(4) << ret_frame_ptr.value();

  cout << " | ";

  cout << setw(4) << stack_ptr.value();

  cout << " |" << endl;
}
