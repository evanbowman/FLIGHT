#include "Entity.hpp"
#include "Game.hpp"

namespace FLIGHT {
Entity::Entity()
    : m_deallocFlag(false), m_position{}, m_rotation{0, 0.0000000001f, 0} {}

bool Entity::GetDeallocFlag() const { return m_deallocFlag; }

void Entity::SetDeallocFlag(const bool deallocFlag) {
    m_deallocFlag = deallocFlag;
}

void Entity::SetPosition(const glm::vec3 & position) { m_position = position; }

void Entity::SetRotation(const glm::vec3 & vec) { m_rotation = vec; }

const glm::vec3 & Entity::GetPosition() const { return m_position; }

const glm::vec3 & Entity::GetRotation() const { return m_rotation; }

glm::vec3 Entity::GetForwardVec() const {
    glm::mat4 yawPitchRotMat;
    yawPitchRotMat =
        glm::rotate(yawPitchRotMat, m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    yawPitchRotMat =
        glm::rotate(yawPitchRotMat, m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    return glm::normalize(-1.0f * glm::vec3(yawPitchRotMat[2].x,
                                            yawPitchRotMat[2].y,
                                            yawPitchRotMat[2].z));
}

std::unique_ptr<Message> Entity::PollMessages() { return m_outbox.Poll(); }

void Entity::SendMessage(std::unique_ptr<Message> msg) {
    m_inbox.Push(std::move(msg));
}

    SolidPreallocMBS::SolidPreallocMBS(const float mbsRadius)
	: m_mbsRadius(mbsRadius) {}
    
    MBS SolidPreallocMBS::GetMBS() {
	return {m_mbsRadius, m_position};
    }
}
