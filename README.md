# The Shed
## Max Kipust, Drew Young
### When you wanted a Mill but you build a S.H.E.D in your backyard instead
 - #### S: Student
 - #### H: Hell Of
 - #### E: Eternal
 - #### D: Development

---

The Shed is a pipelined belt based machine written using the Arch Package.
Each instruction has a static latency, which is the number of cycles
between when the instruction is issued and when the result becomes visible
to the program. This is easy for a compiler to schedule around, but for a
human programmer it can introduce bugs.


### Usage

If you obtained the Shed through try, you should generate a `Makefile` using [gmakemake](https://www.cs.rit.edu/~mjh/tools/gmakemake):

		gmakemake > Makefile

If you obtained the Shed from [the GitHub repository](https://github.com/CoderPuppy/the-shed), skip here. Build the project using make:

		make

To run:

		./CPU program-obj-file-name [data-obj-file-name]


---
### Files
- `belt.cpp` : Logic for managing the belt
- `belt.hpp` : Header for belt.cpp
- `execute.cpp` : Main execute loop and fetch
- `execute.hpp` : Header for execute.cpp
- `hardware.cpp` : Initializes hardware and creates connections
- `hardware.hpp` : Header for hardware.cpp
- `helpers.cpp` : Small misc helpers
- `helpers.hpp` : Header for helpers.cpp
- `includes.hpp` : Header that includes all other headers
- `instruction.cpp` : Implementation for decoding and each instruction
- `instruction.hpp` : Header for instruction.cpp
- `multiplier.cpp` : Implements logic for multiplication
- `multiplier.hpp` : Header for multiplier.cpp
- `CPU.cpp` : Main entry point of the CPU
- `trace.cpp` : implementation to print out tracing
- `trace.hpp` : header for trace.cpp
- `assembler.py` : a very bad assembler

## Test Objects
- `test.{S,obj}` : misc scratch test
- `test_store.{S,obj}` : tests heap store and load
- `test_stack.{S,obj}` : tests the stack and call and return
- `test_mul.{S,obj}` : test multiply
- `test_lcall.{S,obj}` : very basic long call and return test
- `test_callret1.{S,obj}` : very basic call/return test
- `nop_halt.{S,obj}` : a very basic program that doesn't do anything
- `factorial.{S,obj}` : factorial program, the first instruction is what its taking the factorial of

## SHEDHS
- `Instruction.hs` : Instruction definition
- `Interpreter.hs` : Prototype interpreter for the Shed
- `Assembler.hs` : Assembler, from the internal representation to the binary representation
- `DSL.hs` : A library for safely and easily building programs for the Shed
- `TestProgs.hs` : The source for a few more test programs
