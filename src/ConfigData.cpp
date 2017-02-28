#include "ConfigData.hpp"

static void LoadGfxConfig(YAML::Node gfx, ConfigData & conf) {
    if (auto antialiasing = gfx["antialiasing-level"]) {
	conf.graphics.antialiasing = antialiasing.as<int>();
    }
    if (auto vsync = gfx["vsync"]) {
	conf.graphics.vsyncEnabled = vsync.as<bool>();
    }
}

static void LoadLocalizationConfig(YAML::Node local, ConfigData & conf) {
    if (auto strings = local["strings"]) {
	if (auto appName = strings["app-name"]) {
	    conf.localization.appName = appName.as<std::string>();
	}
    }
}

ConfigData LoadConfig() {
    std::ifstream file(ResourcePath() + "config.yml");
    YAML::Node node = YAML::Load((std::stringstream() << file.rdbuf()).str());
    ConfigData conf;
    if (auto gfx = node["graphics"]) {
	LoadGfxConfig(gfx, conf);
    }
    if (auto local = node["localization"]) {
	LoadLocalizationConfig(local, conf);
    }
    return conf;
}
