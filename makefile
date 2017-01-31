CC = clang++
CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -framework sfml-system -framework sfml-graphics -framework sfml-window -framework OpenGL

build:
	$(CC) $(CXXFLAGS) *.cpp $(LDFLAGS) -o test

run:
	./test
