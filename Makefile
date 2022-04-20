ARCHLIB = ../archlib
CXX = g++
CXXFLAGS = -g -I$(ARCHLIB)/include/arch2-5a
LDFLAGS = $(ARCHLIB)/src/*.o

SRCFILES = shed belt execute hardware helpers instruction multiplier trace
PROGFILES = test test_mul test_store test_callret1

all: shed

progs: $(addprefix progs/, $(addsuffix .obj, $(PROGFILES)))

.PHONY: all progs clean

shed: $(addprefix build/, $(addsuffix .o, $(SRCFILES)))
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ 

build: ; mkdir -p $@

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -MT $@ -MMD -MP -MF build/$*.d -c $< -o $@

progs/%.obj: progs/%.S
	python3 assembler.py $<

clean:
	rm shed build/*

include $(wildcard $(addprefix build/, $(addsuffix .d, $(SRCFILES))))
