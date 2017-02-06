#pragma once

#include "Plane.hpp"

class RedTail : public Plane {
    Sprite m_fuselage;
public:
    RedTail();
    void Display(const GLuint shaderProgram) override;
};
