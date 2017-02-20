#include "Game.hpp"
#include <iostream>

int main() {
    try {
	Game game("FLIGHT");
	game.Run();
    } catch (const std::exception & ex) {
	std::cerr << "Error: " << ex.what() << std::endl;
    }
}
