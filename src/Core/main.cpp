#include <FLIGHT/Core/ConfigData.hpp>
#include <FLIGHT/Core/Game.hpp>
#include <iostream>

int main() {
    using namespace FLIGHT;
    try {
        Singleton<Game>::Instance().Configure(LoadConfig());
        Singleton<Game>::Instance().Run();
        console.Log(Logger::Priority::Info, "FLIGHT exited normally, bye!");
        return EXIT_SUCCESS;
    } catch (const std::exception & ex) {
        console.Log(Logger::Priority::Critical, ex.what());
    }
    console.Log(Logger::Priority::Critical,
                  "FLIGHT exited abnormally, "
                  "check the previous CRITICAL log entries.");
    return EXIT_FAILURE;
}

#if defined(_WIN32) or defined(_WIN64)
int WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { return main(); }
#endif
