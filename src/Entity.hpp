/// @file
/// @brief A game entity interface.

#pragma once

#include <glm/glm.hpp>
#ifdef FLIGHT_MAC
#include <OpenGL/gl3.h>
#elif FLIGHT_WINDOWS
#include <GL/glew.h>
#endif
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "BB.hpp"
#include "Message.hpp"
#include "Shader.hpp"
#include "Time.hpp"

namespace FLIGHT {
/// @class Entity
/// @brief  A game entity class.
///
/// With a few exceptions, all of the 3D objects appearing in the game world
/// derive from this class. The game maintains a list of entities, and
/// deallocates them when an instance of the class calls SetDeallocFlag().
/// Entities can send and recieve messages, as well as leave messages in their
/// outboxes for observers to pick up.
class Entity {
    bool m_deallocFlag;

protected:
    MessageBuffer m_outbox;
    MessageBuffer m_inbox;
    glm::vec3 m_position;
    glm::vec3 m_rotation;

public:
    /// @brief Poll the Messages in an Entity's outbox.
    /// @return A managed pointer to a Message.
    std::unique_ptr<Message> PollMessages();

    /// @brief Send a Message to the Entity's inbox.
    /// @param msg A managed pointer to a Message.
    void SendMessage(std::unique_ptr<Message> msg);

    /// @brief Set the object's position in world coorditates.
    /// @param position A 3D vector representing position.
    void SetPosition(const glm::vec3 & position);

    /// @brief Set the object's rotation.
    /// @param rotation A vector where each component represents the rotation
    /// around the corresponding axis.
    void SetRotation(const glm::vec3 & vec);

    /// @brief Get a vector pointing in the direction that the object is facing.
    /// @return A unit vector representing the forward direction.
    glm::vec3 GetForwardVec() const;

    /// @brief Check whether an entity has requested to be freed.
    /// @return The deallocFlag.
    bool GetDeallocFlag() const;

    /// @brief Set the Entity to be deallocated.
    void SetDeallocFlag(const bool deallocFlag = true);

    /// @brief Get the Entity's position.
    /// @return A position vector.
    const glm::vec3 & GetPosition() const;

    /// @brief Get the Entity's rotation.
    /// @return The rotation vector, where each component represents the
    /// rotation around the corresponding axis.
    const glm::vec3 & GetRotation() const;

    Entity();

    /// @brief Display the Entity.
    /// @param shader The shader program to use.
    virtual void Display(ShaderProgram & shader) = 0;

    /// @brief Update the Entity's state.
    /// @param dt Delta time.
    virtual void Update(const Time dt) = 0;

    virtual ~Entity() {}
};

/// @class Solid
/// @brief A collidable Entity
class Solid : public Entity {
public:
    /// @brief Get an AABB that encompasses the Solid.
    /// @return Bounding volume.
    virtual AABB GetAABB() = 0;

    /// @brief Get an MBS that encompasses the Solid.
    /// @return Bounding volume.
    virtual MBS GetMBS() = 0;

    /// @brief Get an OBB that encompasses the Solid.
    /// @return Bounding volume.
    virtual OBB GetOBB() = 0;

    /// @brief Query whether a Solid is a StaticSolid or a DynamicSolid
    ///
    /// This is empirically faster than doing a dynamic_cast, based on profiling
    /// data.
    /// @return true if static, otherwise false.
    virtual bool IsStatic() = 0;
};

/// @class StaticSolid
/// @brief An immotile Solid.
///
/// A static solid is a Solid that does not introduce collisions
/// with other solids through motion of its own. The CollisionManager
/// does not test collisions between static solids, and moving a
/// static solid has undefined behavior.
class StaticSolid : public Solid {
public:
    bool IsStatic() final { return true; }
};

/// @class DynamicSolid
/// @brief A movable Solid
///
/// Like a StaticSolid, but is treated by the CollisionManager in a special way
/// so that it can be repositioned.
class DynamicSolid : public Solid {
public:
    bool IsStatic() final { return false; }
};

/// @class StaticSolidPrealocMBS
/// @brief A static solid that stores it's MBS radius
///
/// Some entities are represented by many Models, and calculating a MBS from
/// merged AABBs repeatedly would be wasteful.
class StaticSolidPreallocMBS : public StaticSolid {
protected:
    float m_mbsRadius;

public:
    StaticSolidPreallocMBS() : m_mbsRadius(0.f) {}

    /// @brief Construct with a default radius.
    /// @param mbsRadius The default radius.
    StaticSolidPreallocMBS(const float mbsRadius);

    MBS GetMBS() override;
};

/// @class DynamicSolidPrealocMBS
/// @brief A dynamic solid that stores it's MBS radius
///
/// Some entities are represented by many Models, and calculating a MBS from
/// merged AABBs repeatedly would be wasteful.
class DynamicSolidPreallocMBS : public DynamicSolid {
protected:
    float m_mbsRadius;

public:
    DynamicSolidPreallocMBS() : m_mbsRadius(0.f) {}

    /// @brief Construct with a default radius.
    /// @param mbsRadius default radius.
    DynamicSolidPreallocMBS(const float mbsRadius);

    MBS GetMBS() override;
};
}
