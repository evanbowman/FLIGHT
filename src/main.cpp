#include "App.hpp"

#include <iostream>

int main() {
    try {
	App app("FLIGHT");
	app.Run();
    } catch (const std::exception & ex) {
	std::cerr << ex.what() << std::endl;
    }
}
