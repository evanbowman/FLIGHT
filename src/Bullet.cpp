#include "Bullet.hpp"

namespace FLIGHT {    
    void Bullet::SetModel(std::shared_ptr<Model> model) { m_model = model; }

    void Bullet::Update(const Time dt) {}

    void Bullet::Display(ShaderProgram & shader) {
	if (auto modelSp = m_model.lock()) {
	    const size_t numVertices = modelSp->Bind(shader);
	    glm::mat4 model;
	    model = glm::translate(model, {0, 30, 0});
	    shader.SetUniformMat4("model", model);
	    shader.SetUniformVec4("color", {0.949f, 0.765f, 0.027f, 1.f});
	    glDrawArrays(GL_TRIANGLES, 0, numVertices);
	    glBindBuffer(GL_ARRAY_BUFFER, 0);
	    glBindTexture(GL_TEXTURE_2D, 0);
	}
    }
}
