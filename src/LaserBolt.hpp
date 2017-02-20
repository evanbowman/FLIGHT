#pragma once

#include "Entity.hpp"
#include "Time.hpp"

class LaserBolt : public Entity {
    Time timer;
public:
    void Display(const GLuint shaderProgram) override;
    void Update(Time dt);
};
