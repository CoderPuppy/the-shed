BASE = ../../archlib
ARCHVER = arch2-5a
CXX = g++
SYS_TYPE=M1
CCFLAGS = -g -I$(BASE)/include/$(ARCHVER)
CXXFLAGS = $(CCFLAGS)
# LIBFLAGS = -g -L$(BASE)/lib/$(SYS_TYPE) -l$(ARCHVER)
LIBFLAGS = ../../archlib/src/*.o
CCLIBFLAGS = $(LIBFLAGS)

SRCFILES = bin/shed.o bin/belt.o bin/execute.o bin/hardware.o bin/helpers.o bin/instruction.o bin/multiplier.o 

all: shed

progs: ../progs/test.obj

.PHONY: all clean

shed: $(SRCFILES)
	$(CXX) $(CXXFLAGS) $(CCLIBFLAGS) $^ -o bin/$@ 

bin/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

../progs/%.obj: ../progs/%.S
	python3 ../assembler.py $<

clean:
	rm bin/*

