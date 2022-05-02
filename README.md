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

where `program-obj-file-name` is the object file to be loaded into instruction memory and `data-obj-file-name` is the object file to be loaded into data memory

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

	This is used as a scratch test file.
	As of right now it multiplies 1 * 1

- `test_store.{S,obj}` : tests heap store and load

	You should see the number 0x42 in belt position 0 and 0x42 in memory
	address 0 in the memory dump.

- `test_stack.{S,obj}` : tests the stack and call and return

	This tests storing a value in the stack, calling a function that clears
	the belt and retrieving the value once the function returns. you should
	see the value 0x10 get added to the belt, stored in the stack at
	position 0, the stack get cleared, and the value fetched from the stack.

- `test_mul.{S,obj}` : test multiply

	this tests the multiply instruction and adding at the same time.
	You should see a belt that looks like this.

	0005    | 0000    | 0080    | 0004    | 0000    | 0006    | 0003    | 0002

- `test_lcall.{S,obj}` : very basic long call and jump test

	this tests long call and long jump. You should see 0x21 in belt position 0
	and 0x20 in belt position 1.

- `test_callret1.{S,obj}` : very basic call/return test
- `nop_halt.{S,obj}` : a very basic program that doesn't do anything

	This program should nop 2 times and then halt. Not that the process of
	halting inserts nops as the halt instruction works its way through the
	pipeline.

- `factorial.{S,obj}` : factorial program

	The program should halt due to halt instruction The result (0x78) should be in belt position 0. The first instruction is const 5, this can be changed to run the program on different values

## SHEDHS
- `Instruction.hs` : Instruction definition
- `Interpreter.hs` : Prototype interpreter for the Shed
- `Assembler.hs` : Assembler, from the internal representation to the binary representation
- `DSL.hs` : A library for safely and easily building programs for the Shed
- `TestProgs.hs` : The source for a few more test programs
