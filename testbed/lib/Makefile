CXX = g++
CFLAGS = -std=c++11 -Wno-psabi -c -I . -I */.

SRC=$(wildcard *.cpp Navio/*/*.cpp)
OBJECTS = $(SRC:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CFLAGS) -o $@ $<

all: $(OBJECTS)
	ar rcs libmynavio.a $(OBJECTS)

clean:
	rm -f *.o Navio/*/*.o *.a Navio/*.a
