#pragma once

#include <memory>

#include "Entity.hpp"
#include "Model.hpp"
#include "Time.hpp"
#include <glm/gtc/type_ptr.hpp>

class Bullet : public Entity {
    std::weak_ptr<Model> m_model;
public:
    void SetModel(std::shared_ptr<Model> model);
    void Display(const GLuint shaderProgram) override;
    void Update(const Time dt) override;
};
