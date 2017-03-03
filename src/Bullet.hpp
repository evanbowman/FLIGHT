#pragma once

#include <memory>

#include "Entity.hpp"
#include "Model.hpp"
#include "Time.hpp"
#include <glm/gtc/type_ptr.hpp>
#ifdef FLIGHT_WINDOWS
#include <Windows.h>
#include <gl/glew.h>
#endif

namespace FLIGHT {    
    class Bullet : public Entity {
	std::weak_ptr<Model> m_model;
    public:
	void SetModel(std::shared_ptr<Model> model);
	void Display(ShaderProgram & shader) override;
	void Update(const Time dt) override;
    };
}
