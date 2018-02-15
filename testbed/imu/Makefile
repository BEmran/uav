CXX ?= g++
LIB = ../lib
NAVIO = $(LIB)/Navio
INCLUDES = -I $(LIB)

all:
	$(MAKE) -C $(LIB) all
	$(CXX) -std=c++11 $(INCLUDES) main.cpp -L$(LIB) -lmine -o mian

clean:
	rm -f mian

