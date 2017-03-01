#pragma once

#include <yaml-cpp/yaml.h>
#include <SFML/Window.hpp>
#include <fstream>
#include <unordered_map>
#include <iostream>

#include "ResourcePath.hpp"

struct ConfigData {
    struct {
	int antialiasing = 4;
	bool vsyncEnabled = true;
	bool hideCursor = true;
    } graphics;
    struct {
	struct {
	    sf::Keyboard::Key pause = sf::Keyboard::Escape;
	    sf::Keyboard::Key weapon = sf::Keyboard::Z;
	} keyboardMapping;
    } controls;
    struct {
	struct {
	    std::string appName = "FLIGHT";
	} strings;
    } localization;
};

ConfigData LoadConfig();
