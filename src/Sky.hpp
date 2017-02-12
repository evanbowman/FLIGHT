#pragma once

#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Entity.hpp"
#include "FrustumCheck.hpp"

class SkyManager : public Entity {
    glm::vec3 m_sunPos;
    bool m_sunVisible;
    GLuint m_quadVbo;
    GLuint m_quadEbo;
public:
    SkyManager();
    void Update(const glm::vec3 & cameraPos, const glm::vec3 & viewDir);
    void Display(const GLuint) override;
};
