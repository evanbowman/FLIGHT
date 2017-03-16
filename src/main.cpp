#include <iostream>

#include "ConfigData.hpp"
#include "Dialog.hpp"
#include "Game.hpp"

static void VerifyEnvCompat() {
    auto desktop = sf::VideoMode::getDesktopMode();
    if (desktop.width < desktop.height) {
	throw std::runtime_error("Game does not support vertical displays");
    }
    if (sf::Texture::getMaximumSize() < std::max(desktop.width, desktop.height)) {
	throw std::runtime_error("Graphics driver\'s max texture size is insufficient");
    }
}

int main() {
    try {
	VerifyEnvCompat();
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
