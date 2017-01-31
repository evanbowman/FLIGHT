#pragma once

#include "Entity.hpp"
#include "Sprite.hpp"
#include "AssetManager.hpp"

class Plane : public Entity {
protected:
    Sprite m_leftWing, m_rightWing, m_engine;
public:
    virtual void Display(const GLuint shaderProgram) override;
};
