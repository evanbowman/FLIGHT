CC = clang++
CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -framework sfml-window -framework OpenGL -framework sfml-system -framework sfml-graphics

build:
	$(CC) $(CXXFLAGS) main.cpp $(LDFLAGS) -o test

run:
	./test
