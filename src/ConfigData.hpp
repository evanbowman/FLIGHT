#pragma once

#include <yaml-cpp/yaml.h>
#include <SFML/Window.hpp>
#include <fstream>

#include "ResourcePath.hpp"

struct ConfigData {
    struct {
	int antialiasing = 4;
	bool vsyncEnabled = true;
    } graphics;
    struct {
	std::string appName = "FLIGHT";
    } localization;
};

ConfigData LoadConfig();
