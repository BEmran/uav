CXX = g++
CFLAGS = -std=c++11 -Wno-psabi -c -I . -I */.

SRC=$(wildcard */*.cpp */*/*.cpp)
OBJECTS = $(SRC:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CFLAGS) -o $@ $<

all: $(OBJECTS)
	ar rcs libmine.a $(OBJECTS)

clean:
	rm -f */*.o *.a */*/*.o
