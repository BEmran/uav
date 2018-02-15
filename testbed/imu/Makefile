CXX ?= g++ -std=c++11
LIB = ../lib
NAVIO = $(LIB)/Navio
INCLUDES = -I $(LIB) -I $(NAVIO)

all:
	$(MAKE) -C $(LIB) all
	$(CXX) main.cpp -o main $(INCLUDES) -L $(LIB) -lmynavio -lrt
clean:
	rm -f mian

