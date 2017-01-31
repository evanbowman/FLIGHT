#pragma once

#include <memory>
#include "Texture.hpp"
#include "Model.hpp"

class Sprite {
    std::weak_ptr<Texture> m_texture;
    std::weak_ptr<Model> m_model;
    glm::vec3 m_position;
    glm::vec3 m_scale;
public:
    Sprite();
    
    void SetTexture(std::shared_ptr<Texture> texture);
    void SetModel(std::shared_ptr<Model> model);
    void Display(const GLuint shaderProgram);
    void SetPosition(const glm::vec3 & position);
    void SetScale(const glm::vec3 & scale);
};
