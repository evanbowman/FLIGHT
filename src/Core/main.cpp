#include <FLIGHT/Core/ConfigData.hpp>
#include <FLIGHT/Core/Dialog.hpp>
#include <FLIGHT/Core/Game.hpp>
#include <iostream>

static void VerifyEnvCompat() {
    auto desktop = sf::VideoMode::getDesktopMode();
    if (desktop.width < desktop.height) {
        throw std::runtime_error("Game does not support vertical displays");
    }
    if (sf::Texture::getMaximumSize() <
        std::max(desktop.width, desktop.height)) {
        throw std::runtime_error(
            "Graphics driver\'s max texture size is insufficient");
    }
}

int main() {
    try {
        VerifyEnvCompat();
	Singleton<FLIGHT::Game>::Instance().Configure(FLIGHT::LoadConfig());
        Singleton<FLIGHT::Game>::Instance().Run();
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
