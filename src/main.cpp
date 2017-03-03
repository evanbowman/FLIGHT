#include <iostream>

#include "ConfigData.hpp"
#include "Game.hpp"
#include "Dialog.hpp"

int main() {
    try {
	Game game(LoadConfig());
	game.Run();
    } catch (const std::exception & ex) {
#ifdef NDEBUG
        std::cerr << ex.what() << std::endl;
	//PromoteExceptionToOSDialogBox(ex);
#else
	std::cerr << ex.what() << std::endl;
#endif
    }
}

#ifdef FLIGHT_WINDOWS
int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { return main(); }
#endif
