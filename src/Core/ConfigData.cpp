#include <FLIGHT/Core/ConfigData.hpp>
#include <FLIGHT/Core/Game.hpp>

namespace FLIGHT {
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
        {"z", sf::Keyboard::Z},         {"num1", sf::Keyboard::Num1},
        {"num2", sf::Keyboard::Num2},   {"num3", sf::Keyboard::Num3},
        {"num4", sf::Keyboard::Num4},   {"num5", sf::Keyboard::Num5},
        {"num6", sf::Keyboard::Num6},   {"num7", sf::Keyboard::Num7},
        {"num8", sf::Keyboard::Num8},   {"num9", sf::Keyboard::Num9},
        {"num0", sf::Keyboard::Num0}};
    auto match = dictionary.find(keyName);
    if (match not_eq dictionary.end()) {
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

static void LoadGraphicsConfig(ConfigData & conf, pugi::xml_node & root) {
    auto graphics = root.child("Graphics");
    if (graphics) {
        static const std::unordered_map<std::string,
                                        std::function<void(pugi::xml_node &)>>
            actions{{"aa-level",
                     [&conf](auto & node) {
                         conf.graphics.antialiasing =
                             std::stoi(node.child_value());
                     }},
                    {"vsync-enabled",
                     [&conf](auto & node) {
                         if (node.child_value() == std::string("no")) {
                             conf.graphics.vsyncEnabled = false;
                         }
                     }},
                    {"hide-cursor", [&conf](auto & node) {
                         if (node.child_value() == std::string("no")) {
                             conf.graphics.hideCursor = false;
                         }
                     }}};
        for (auto & setting : graphics) {
            auto action = actions.find(setting.attribute("id").value());
            if (action != actions.end()) {
                action->second(setting);
            }
        }
    }
}

static void LoadControlsConfig(ConfigData & conf, pugi::xml_node & root) {
    if (auto controls = root.child("Controls")) {
        if (auto keyboard = controls.child("Keyboard")) {
            static const std::unordered_map<
                std::string, std::function<void(pugi::xml_node &)>>
                actions{{"reg1",
                         [&conf](auto & node) {
                             SetKeyboardMapping(
                                 conf.controls.keyboardMapping.regularKeys[0],
                                 node.child_value());
                         }},
                        {"reg2",
                         [&conf](auto & node) {
                             SetKeyboardMapping(
                                 conf.controls.keyboardMapping.regularKeys[1],
                                 node.child_value());
                         }},
                        {"reg3",
                         [&conf](auto & node) {
                             SetKeyboardMapping(
                                 conf.controls.keyboardMapping.regularKeys[2],
                                 node.child_value());
                         }},
                        {"spec1",
                         [&conf](auto & node) {
                             SetKeyboardMapping(
                                 conf.controls.keyboardMapping.specialKeys[0],
                                 node.child_value());
                         }},
                        {"spec2",
                         [&conf](auto & node) {
                             SetKeyboardMapping(
                                 conf.controls.keyboardMapping.specialKeys[1],
                                 node.child_value());
                         }},
                        {"spec3", [&conf](auto & node) {
                             SetKeyboardMapping(
                                 conf.controls.keyboardMapping.specialKeys[2],
                                 node.child_value());
                         }}};
            for (auto & mapping : keyboard) {
                auto action = actions.find(mapping.attribute("id").value());
                if (action != actions.end()) {
                    action->second(mapping);
                }
            }
        }
        if (auto gamepads = controls.child("Gamepads")) {
            for (auto & controller : gamepads) {
                ConfigData::ControlsConf::GamepadMapping info;
                if (auto vendorid = controller.attribute("vendor-id")) {
                    info.vendorId = vendorid.as_uint();
                } else {
                    continue;
                }
                if (auto productid = controller.attribute("product-id")) {
                    info.productId = productid.as_uint();
                } else {
                    continue;
                }
                static const std::unordered_map<
                    std::string, std::function<void(pugi::xml_node &)>>
                    actions{{"reg0",
                             [&info](auto & node) {
                                 info.regularKeys[0] =
                                     (unsigned)std::stoi(node.child_value());
                             }},
                            {"reg1",
                             [&info](auto & node) {
                                 info.regularKeys[1] =
                                     (unsigned)std::stoi(node.child_value());
                             }},
                            {"reg2",
                             [&info](auto & node) {
                                 info.regularKeys[2] =
                                     (unsigned)std::stoi(node.child_value());
                             }},
                            {"spec0",
                             [&info](auto & node) {
                                 info.specialKeys[0] =
                                     (unsigned)std::stoi(node.child_value());
                             }},
                            {"spec1",
                             [&info](auto & node) {
                                 info.specialKeys[1] =
                                     (unsigned)std::stoi(node.child_value());
                             }},
                            {"spec2", [&info](auto & node) {
                                 info.specialKeys[2] =
                                     (unsigned)std::stoi(node.child_value());
                             }}};
                for (auto & mapping : controller) {
                    auto action = actions.find(mapping.attribute("id").value());
                    if (action != actions.end()) {
                        action->second(mapping);
                    }
                }
                conf.controls.gamepadMappings.push_back(info);
            }
        }
    }
}

static void LoadLocaleConfig(ConfigData & conf, pugi::xml_node & root) {
    if (auto localization = root.child("Localization")) {
        if (auto font = localization.child("Font")) {
            conf.localization.font = font.attribute("file").value();
        }
        if (auto strings = localization.child("Strings")) {
            static const std::unordered_map<
                std::string, std::function<void(pugi::xml_node &)>>
                actions{{"app-name",
                         [&conf](auto & node) {
                             conf.localization.strings.appName =
                                 node.child_value();
                         }},
                        {"credits",
                         [&conf](auto & node) {
                             conf.localization.strings.credits =
                                 node.child_value();
                         }},
                        {"start-text", [&conf](auto & node) {
                             conf.localization.strings.startText =
                                 node.child_value();
                         }}};
            for (auto & string : strings) {
                auto action = actions.find(string.attribute("id").value());
                if (action != actions.end()) {
                    action->second(string);
                }
            }
        }
    }
}

ConfigData LoadConfig() {
    ConfigData conf;
    conf.empty = false;
    pugi::xml_document doc;
    auto res = doc.load_file((ResourcePath() + "Config.xml").c_str());
    if (not res) {
        throw std::runtime_error("Error loading config file: " +
                                 std::string(res.description()));
    }
    if (auto root = doc.child("Config")) {
        LoadGraphicsConfig(conf, root);
        LoadControlsConfig(conf, root);
        LoadLocaleConfig(conf, root);
    }
    return conf;
}
}
