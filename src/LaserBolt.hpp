#pragma once

#include "Entity.hpp"

class LaserBolt : public Entity {
    long long timer;
public:
    void Display(const GLuint shaderProgram) override;
    void Update(long long dt);
};
