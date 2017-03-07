#pragma once

#include <glm/glm.hpp>
#ifdef FLIGHT_MAC
#include <OpenGL/gl3.h>
#elif FLIGHT_WINDOWS
#include <GL/glew.h>
#endif
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <memory>

#include "Time.hpp"
#include "BB.hpp"
#include "Shader.hpp"
#include "Message.hpp"

namespace FLIGHT {
    class Entity {
	bool m_deallocFlag;
    protected:
	MessageBuffer m_outbox;
	MessageBuffer m_inbox;
	glm::vec3 m_position;
	glm::vec3 m_rotation;
    public:
	std::unique_ptr<Message> PollMessages();
	void SendMessage(std::unique_ptr<Message> msg);
	void SetPosition(const glm::vec3 & position);
	void SetRotation(const glm::vec3 & vec);
	glm::vec3 GetForwardVec() const;
	bool GetDeallocFlag() const;
	void SetDeallocFlag(const bool deallocFlag = true);
	const glm::vec3 & GetPosition() const;
	const glm::vec3 & GetRotation() const;
	Entity();
	virtual void Display(ShaderProgram & shader) = 0;
	virtual void Update(const Time dt) = 0;
	virtual ~Entity() {}
    };

    class Solid : public Entity {
    public:
	virtual AABB GetAABB() = 0;
    };
}
