#include <iostream>

#include "ConfigData.hpp"
#include "Dialog.hpp"
#include "Game.hpp"

int main() {
    try {
        FLIGHT::Game game(FLIGHT::LoadConfig());
        game.Run();
    } catch (const std::exception & ex) {
#ifdef NDEBUG
        PromoteExceptionToOSDialogBox(ex);
#else
        std::cerr << ex.what() << std::endl;
#endif
    }
}

#ifdef FLIGHT_WINDOWS
int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { return main(); }
#endif
