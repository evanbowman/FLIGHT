#include <FLIGHT/Core/ConfigData.hpp>
#include <FLIGHT/Core/Dialog.hpp>
#include <FLIGHT/Core/Game.hpp>
#include <iostream>

int main() {
    try {
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
