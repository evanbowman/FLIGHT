#pragma once

#include <memory>
#include "Texture.hpp"
#include "Model.hpp"
#include "Material.hpp"
#include "Shader.hpp"

namespace FLIGHT {
    class Sprite {
	std::weak_ptr<Texture> m_texture;
	std::weak_ptr<Model> m_model;
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;
	std::weak_ptr<Material> m_material;
    public:
	Sprite();
	void SetTexture(std::shared_ptr<Texture> texture);
	void SetModel(std::shared_ptr<Model> model);
	std::shared_ptr<Model> GetModel() const;
	void Display(const glm::mat4 & parentContext, ShaderProgram & shader);
	void SetPosition(const glm::vec3 & position);
	void SetRotation(const glm::vec3 & vec);
	const glm::vec3 & GetRotation() const;
	void SetScale(const glm::vec3 & scale);
	void SetMaterial(std::shared_ptr<Material> material);
	AABB GetAABB();
    };
}
