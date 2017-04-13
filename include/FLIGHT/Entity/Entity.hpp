#pragma once

#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <FLIGHT/Core/BB.hpp>
#include <FLIGHT/Core/Message.hpp>
#include <FLIGHT/Graphics/Shader.hpp>
#include <FLIGHT/Graphics/DisplayImpl.hpp>
#include <FLIGHT/Core/Time.hpp>
#include <FLIGHT/Core/Serialize.hpp>

namespace FLIGHT {    
    class Entity : public MessageTarget, public Serializable {
    bool m_deallocFlag;

protected:
    glm::vec3 m_position;
    glm::vec3 m_rotation;

public:
    void SetPosition(const glm::vec3 & position);
    void SetRotation(const glm::vec3 & vec);
    glm::vec3 GetForwardVec() const;
    bool GetDeallocFlag() const;
    void SetDeallocFlag(const bool deallocFlag = true);
    const glm::vec3 & GetPosition() const;
    const glm::vec3 & GetRotation() const;
    Entity();
    virtual void Display(DisplayImpl & renderer) = 0;
    virtual void Update(const Time dt) = 0;
    virtual ~Entity() {}
};

class Solid : public Entity {
public:
    virtual AABB GetAABB() = 0;
    virtual MBS GetMBS() = 0;
    virtual OBB GetOBB() = 0;
    virtual bool IsStatic() = 0;
};

// A static solid is a solid that does not introduce collisions
// with other solids through motion of its own. The CollisionManager
// does not test collisions between static solids, and moving a
// StaticSolid does not have defined behavior in the collision testing.
class StaticSolid : public Solid {
public:
    bool IsStatic() final { return true; }
};

class DynamicSolid : public Solid {
public:
    bool IsStatic() final { return false; }
};

class StaticSolidPreallocMBS : public StaticSolid {
protected:
    float m_mbsRadius;

public:
    StaticSolidPreallocMBS() : m_mbsRadius(0.f) {}
    StaticSolidPreallocMBS(const float mbsRadius);
    MBS GetMBS() override;
};

class DynamicSolidPreallocMBS : public DynamicSolid {
protected:
    float m_mbsRadius;

public:
    DynamicSolidPreallocMBS() : m_mbsRadius(0.f) {}
    DynamicSolidPreallocMBS(const float mbsRadius);
    MBS GetMBS() override;
};
}
