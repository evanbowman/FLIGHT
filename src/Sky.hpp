#pragma once

#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Entity.hpp"

class SkyManager : public Entity {
    GLuint m_vbo;
public:
    SkyManager();
    void Display(const GLuint shaderProgram) override;
};
