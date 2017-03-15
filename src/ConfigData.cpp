#include "ConfigData.hpp"

namespace FLIGHT {
static void LoadGfxConfig(YAML::Node gfx, ConfigData & conf) {
    if (auto antialiasing = gfx["antialiasing-level"]) {
        conf.graphics.antialiasing = antialiasing.as<int>();
    }
    if (auto vsync = gfx["vsync-enabled"]) {
        conf.graphics.vsyncEnabled = vsync.as<bool>();
    }
    if (auto hideCursor = gfx["hide-cursor"]) {
        conf.graphics.hideCursor = hideCursor.as<bool>();
    }
}

static void LoadLocalizationConfig(YAML::Node locale, ConfigData & conf) {
    if (auto strings = locale["strings"]) {
        if (auto appName = strings["app-name"]) {
            conf.localization.strings.appName = appName.as<std::string>();
        }
        if (auto creditsPrefix = strings["credits"]) {
            conf.localization.strings.credits = creditsPrefix.as<std::string>();
        }
    }
    if (auto font = locale["font"]) {
        conf.localization.font = font.as<std::string>();
    }
}

static sf::Keyboard::Key MatchConfStrToSFMLKey(const std::string & keyName) {
    static const std::unordered_map<std::string, sf::Keyboard::Key> dictionary{
        {"esc", sf::Keyboard::Escape},  {"up", sf::Keyboard::Up},
        {"down", sf::Keyboard::Down},   {"left", sf::Keyboard::Left},
        {"right", sf::Keyboard::Right}, {"a", sf::Keyboard::A},
        {"b", sf::Keyboard::B},         {"c", sf::Keyboard::C},
        {"d", sf::Keyboard::D},         {"e", sf::Keyboard::E},
        {"f", sf::Keyboard::F},         {"g", sf::Keyboard::G},
        {"h", sf::Keyboard::H},         {"i", sf::Keyboard::I},
        {"j", sf::Keyboard::J},         {"k", sf::Keyboard::K},
        {"l", sf::Keyboard::L},         {"m", sf::Keyboard::M},
        {"n", sf::Keyboard::N},         {"o", sf::Keyboard::O},
        {"p", sf::Keyboard::P},         {"q", sf::Keyboard::Q},
        {"r", sf::Keyboard::R},         {"s", sf::Keyboard::S},
        {"t", sf::Keyboard::T},         {"u", sf::Keyboard::U},
        {"v", sf::Keyboard::V},         {"w", sf::Keyboard::W},
        {"x", sf::Keyboard::X},         {"y", sf::Keyboard::Y},
        {"z", sf::Keyboard::Z}};
    auto match = dictionary.find(keyName);
    if (match != dictionary.end()) {
        return match->second;
    } else {
        throw std::runtime_error("invalid config file value \'" + keyName +
                                 "\'");
    }
}

static void SetKeyboardMapping(sf::Keyboard::Key & binding,
                               const std::string & name) {
    try {
        binding = MatchConfStrToSFMLKey(name);
    } catch (const std::exception & ex) {
        std::cerr << ex.what() << std::endl;
    }
}

static void LoadControlsConfig(YAML::Node controls, ConfigData & conf) {
    if (auto keyboardMapping = controls["keyboard-mapping"]) {
        if (auto pause = keyboardMapping["pause"]) {
            SetKeyboardMapping(conf.controls.keyboardMapping.pause,
                               pause.as<std::string>());
        }
        if (auto weapon = keyboardMapping["weapon"]) {
            SetKeyboardMapping(conf.controls.keyboardMapping.weapon,
                               weapon.as<std::string>());
        }
        if (auto aim = keyboardMapping["aim"]) {
            SetKeyboardMapping(conf.controls.keyboardMapping.aim,
                               aim.as<std::string>());
        }
    }
    if (auto gamepads = controls["gamepads"]) {
        for (auto it = gamepads.begin(); it != gamepads.end(); ++it) {
            ConfigData::ControlsConf::GamepadMapping mapping;
            if (auto vendorId = (*it)["vendor-id"]) {
                mapping.vendorId = vendorId.as<unsigned>();
            } else {
                continue;
            }
            if (auto productId = (*it)["product-id"]) {
                mapping.productId = productId.as<unsigned>();
            } else {
                continue;
            }
            if (auto buttonMapping = (*it)["button-mapping"]) {
                if (auto pause = buttonMapping["pause"]) {
                    mapping.pause = pause.as<unsigned>();
                } else {
                    continue;
                }
                if (auto weapon = buttonMapping["weapon"]) {
                    mapping.weapon = weapon.as<unsigned>();
                } else {
                    continue;
                }
                if (auto aim = buttonMapping["aim"]) {
                    mapping.aim = aim.as<unsigned>();
                } else {
                    continue;
                }
            }
            conf.controls.gamepadMappings.push_back(mapping);
        }
    }
}

ConfigData LoadConfig() {
    std::ifstream file(ResourcePath() + "config.yml");
    std::stringstream ss;
    ss << file.rdbuf();
    YAML::Node node = YAML::Load(ss.str());
    ConfigData conf;
    if (auto gfx = node["graphics"]) {
        LoadGfxConfig(gfx, conf);
    }
    if (auto locale = node["localization"]) {
        LoadLocalizationConfig(locale, conf);
    }
    if (auto controls = node["controls"]) {
        LoadControlsConfig(controls, conf);
    }
    return conf;
}
}
