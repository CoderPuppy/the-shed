ARCHLIB ?= ../archlib
CXXOPTS += -g -I$(ARCHLIB) -Wall
# -L../archlib/ -larch2-5a
CXX ?= clang++

all: shed

clean:
	rm shed *.o

.PHONY: all clean

shed: shed.o hardware.o
	$(CXX) $(CXXOPTS) ../archlib/*.o $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXOPTS) -c $< -o $@
