#pragma once

#include <SFML/Window.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <map>
#include <pugixml.hpp>
#include <array>

#include <FLIGHT/Util/ResourcePath.hpp>

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
            std::array<sf::Keyboard::Key, 3> regularKeys {
                {sf::Keyboard::Escape,
                 sf::Keyboard::Z,
                 sf::Keyboard::X}
            };
            std::array<sf::Keyboard::Key, 3> specialKeys {
                {sf::Keyboard::Num1, sf::Keyboard::Num2, sf::Keyboard::Num3}
            };
        } keyboardMapping;
        struct GamepadMapping {
            unsigned vendorId, productId;
            std::array<unsigned, 3> regularKeys;
            std::array<unsigned, 3> specialKeys;
        };
        std::vector<GamepadMapping> gamepadMappings;
    } controls;
    struct LocalizationConf {
        std::string font = "RobotoSlab-Regular.ttf";
        struct Strings {
            std::string appName = "FLIGHT";
            std::string credits = "A Game By Evan Bowman";
            std::string startText = "press start";
        } strings;
    } localization;
};

ConfigData LoadConfig();
}
