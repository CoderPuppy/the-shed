ARCHLIB = ../archlib
CXX = g++
SYS_TYPE=M1
CXXFLAGS = -g -I$(ARCHLIB)
LDFLAGS = $(ARCHLIB)/*.o

OBJFILES = build/shed.o build/belt.o build/execute.o build/hardware.o build/helpers.o build/instruction.o build/multiplier.o 

all: shed

progs: progs/test.obj

.PHONY: all progs clean

shed: $(OBJFILES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ 

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

progs/%.obj: progs/%.S
	python3 ../assembler.py $<

clean:
	rm shed build/*

