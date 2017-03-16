/// @file
/// @brief In game coin collectible.

#pragma once

#include <memory>

#include "Entity.hpp"
#include "Model.hpp"

namespace FLIGHT {
/// @class Coin
/// @brief A coin that game entities can pick up.
class Coin : public StaticSolidPreallocMBS {
    std::weak_ptr<Model> m_model;
    Time m_timer;
    void MessageLoop();

public:
    /// @brief Construct a coin with position info.
    /// @param position The coin's initial position.
    Coin(const glm::vec3 & position);

    /// @brief Display the coin.
    /// @param shader The shader program to use.
    ///
    /// The shader program should define the uniforms mat4 model, mat4
    /// cameraSpace, and vec4 color, along with input vec3 position.
    void Display(ShaderProgram & shader) override;

    /// @brief Update the coin.
    ///
    /// The coin basically just floats up and down by interpolating a harmonic
    /// function.
    /// @param dt Delta time.
    void Update(const Time dt) override;

    AABB GetAABB() override;
    OBB GetOBB() override;
};
}
