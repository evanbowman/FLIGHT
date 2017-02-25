#include "Bullet.hpp"

void Bullet::SetModel(std::shared_ptr<Model> model) {
    m_model = model;
}

void Bullet::Display(const GLuint shaderProgram) {
    if (auto modelSp = m_model.lock()) {
	const size_t numVertices = modelSp->BindVertices(shaderProgram);
	glm::mat4 model;
	model = glm::translate(model, {0, 30, 0});
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform4f(glGetUniformLocation(shaderProgram, "color"), 0.949f, 0.765f, 0.027f, 1.f);
	glDrawArrays(GL_TRIANGLES, 0, numVertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
    }
}
