#include "Sprite.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <OpenGL/gl3.h>
#include <glm/gtc/matrix_transform.hpp>

Sprite::Sprite() : m_position{}, m_scale{1, 1, 1} {}

void Sprite::SetTexture(std::shared_ptr<Texture> texture) {
    m_texture = texture;
}

void Sprite::SetModel(std::shared_ptr<Model> model) {
    m_model = model;
}

void Sprite::Display(const GLuint shaderProgram) {
    auto texSp = m_texture.lock();
    if (!texSp) {
	throw std::runtime_error("Sprite missing texture data");
    }
    glBindTexture(GL_TEXTURE_2D, texSp->GetId());
    auto modSp = m_model.lock();
    if (!modSp) {
	throw std::runtime_error("Sprite missing model data");
    }
    size_t numVertices = modSp->Bind(shaderProgram);
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(0.1f, 5.f, 0.5f));
    GLint modelAttrib = glGetAttribLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelAttrib, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Sprite::SetPosition(const glm::vec3 & position){
    m_position = position;
}

void Sprite::SetScale(const glm::vec3 & scale) {
    m_scale = scale;
}
