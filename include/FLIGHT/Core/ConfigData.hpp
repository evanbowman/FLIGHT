#pragma once

#include <SFML/Window.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <yaml-cpp/yaml.h>

#include "ResourcePath.hpp"

namespace FLIGHT {
struct ConfigData {
    bool empty = true;
    struct GraphicsConf {
        int antialiasing = 4;
        bool vsyncEnabled = true;
        bool hideCursor = true;
    } graphics;
    struct ControlsConf {
        struct KeyboardMapping {
            sf::Keyboard::Key pause = sf::Keyboard::Escape;
            sf::Keyboard::Key weapon = sf::Keyboard::Z;
            sf::Keyboard::Key aim = sf::Keyboard::X;
        } keyboardMapping;
        struct GamepadMapping {
            unsigned vendorId, productId;
            unsigned pause, weapon, aim;
        };
        std::vector<GamepadMapping> gamepadMappings;
    } controls;
    struct LocalizationConf {
        std::string font;
        struct Strings {
            std::string appName = "FLIGHT";
            std::string credits = "A Game By Evan Bowman";
            std::string startText = "press start";
        } strings;
    } localization;
};

ConfigData LoadConfig();
}
