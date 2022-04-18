ARCHLIB = ../archlib
CXX = g++
CXXFLAGS = -g -I$(ARCHLIB) -std=c++17
LDFLAGS = $(ARCHLIB)/*.o

SRCFILES = shed belt execute hardware helpers instruction multiplier
PROGFILES = test test_mul test_store

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
