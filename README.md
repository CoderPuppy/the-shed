# The Shed
## Max Kipust, Drew Young
### When you wanted a Mill but you build a S.H.E.D in your backyard instead
 - #### S: Student
 - #### H: Hell Of
 - #### E: Eternal
 - #### D: Development

-----

The Shed is a pipelined belt based machine written using the Arch Package.
Each instruction has a static latency, which is the number of cycles between when the instruction is issued and when the result becomes visible to the program. This is easy for a compiler to schedule around, but for a human it is easy to introduce bugs.

### Files
- belt.cpp : Logic for managing the belt
- belt.hpp : Header for belt.cpp
- execute.cpp : Main execute loop and fetch
- execute.hpp : Header for execute.cpp
- hardware.cpp : Initializes hardware and creates connections
- hardware.hpp : Header for hardware.cpp
- helpers.cpp : Small misc helpers
- helpers.hpp : Header for helpers.cpp
- includes.hpp : Header that includes all other headers
- instruction.cpp : Implementation for decoding and each instruction
- instruction.hpp : Header for instruction.cpp
- multiplier.cpp : Implements logic for multiplication
- multiplier.hpp : Header for multiplier.cpp
- shed.cpp : Main entry point of the CPU
- trace.cpp : implementation to print out tracing
- trace.hpp : header for trace.cpp
- assembler.py : a very bad assembler