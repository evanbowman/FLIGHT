#include "Game.hpp"
#include <iostream>
#include "ConfigData.hpp"

int main() {
    try {
	Game game(LoadConfig());
	game.Run();
    } catch (const std::exception & ex) {
	std::cerr << ex.what() << std::endl;
    }
}

#ifdef FLIGHT_WINDOWS
int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { return main(); }
#endif
