#include "Plane.hpp"
#include "Game.hpp"

namespace FLIGHT {
Plane::Plane() : m_pitch{}, m_roll{}, m_thrust{}, m_yVelocity{} {}

Plane::Plane(const Blueprint & blueprint)
    : m_pitch(0.f), m_roll(0.f), m_thrust(1.f), m_yVelocity(0.f) {
    for (auto & part : blueprint.GetParts()) {
        Sprite sprite;
        auto model = GetGame().GetAssetMgr().GetModel(part.model);
        auto material = GetGame().GetAssetMgr().GetMaterial(part.material);
        auto texture = GetGame().GetAssetMgr().GetTexture(part.texture);
        if (!model) {
            throw std::runtime_error("model \'" + part.model +
                                     "\' isn\'t loaded");
        }
        if (!texture) {
            throw std::runtime_error("texture \'" + part.texture +
                                     "\' isn\'t loaded");
        }
        if (!material) {
            throw std::runtime_error("material \'" + part.material +
                                     "\' isn\'t loaded");
        }
        sprite.SetModel(model);
        sprite.SetTexture(texture);
        sprite.SetMaterial(material);
        sprite.SetPosition(part.position);
        sprite.SetScale(part.scale);
        sprite.SetRotation(part.rotation);
        m_parts.push_back(sprite);
    }
    m_mbsRadius = MBS(GetAABB()).GetRadius();
}

void Plane::Display(ShaderProgram & shader) {
    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(modelMatrix, m_position);
    modelMatrix = glm::rotate(modelMatrix, m_rotation.y, {0, 1, 0});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.z, {0, 0, 1});
    modelMatrix = glm::rotate(modelMatrix, m_rotation.x, {1, 0, 0});
    for (auto & part : m_parts) {
        part.Display(modelMatrix, shader);
    }
}

AABB Plane::GetAABB() {
    AABB ret = m_parts.front().GetAABB();
    auto it = m_parts.begin();
    ++it;
    for (; it != m_parts.end(); ++it) {
        ret.Merge(it->GetAABB());
    }
    ret.Rotate(m_rotation.y, {0, 1, 0});
    ret.Rotate(m_rotation.z, {0, 0, 1});
    ret.Rotate(m_rotation.x, {1, 0, 0});
    ret.Translate(m_position);
    return ret;
}

OBB Plane::GetOBB() {
    AABB aabb = m_parts.front().GetAABB();
    auto it = m_parts.begin();
    ++it;
    for (; it != m_parts.end(); ++it) {
        aabb.Merge(it->GetAABB());
    }
    OBB obb(aabb);
    obb.Rotate(m_rotation.y, {0, 1, 0});
    obb.Rotate(m_rotation.z, {0, 0, 1});
    obb.Rotate(m_rotation.x, {1, 0, 0});
    obb.Translate(m_position);
    return obb;
}

void Plane::SetThrust(const float thrust) { m_thrust = thrust; }

float Plane::GetThrust() const { return m_thrust; }

void Plane::MessageLoop() {
    while (auto msg = m_inbox.Poll()) {
        switch (msg->GetId()) {
        case Message::Id::Collision: {
            Collision * collision = static_cast<Collision *>(msg.get());
            if (dynamic_cast<Plane *>(collision->with.get())) {
                m_outbox.Push(std::make_unique<Death>());
            } else if (dynamic_cast<Coin *>(collision->with.get())) {
                SetColor({0.949f, 0.765f, 0.027f, 1.f});
                BeginDecay();
                m_outbox.Push(std::make_unique<PickedUpCoin>());
            }
        } break;

        case Message::Id::TerrainCollision:
            m_outbox.Push(std::make_unique<Death>());
            break;

        default:
            throw MessageError(msg->GetId());
        }
    }
}

void Plane::Update(const Time dt) {
    MessageLoop();
    ColorMixDecay::Update(dt);
    const float rateFactor = 0.000035f * dt;
    static const float yCeil = GetElevationLimit();
    static const float yFloor = -3.f;
    const float yDisp = std::sin(m_rotation.x) * rateFactor;
    m_yVelocity = MATH::lerp(yDisp, m_yVelocity, 0.05 * dt * 0.0001f);
    if (m_position.y + m_yVelocity < yCeil &&
        m_position.y + m_yVelocity > yFloor) {
        m_position.y += m_thrust * m_yVelocity;
    }
    m_position.z -=
        m_thrust * std::cos(m_rotation.y) * rateFactor * std::cos(m_rotation.x);
    m_position.x -=
        m_thrust * std::sin(m_rotation.y) * rateFactor * std::cos(m_rotation.x);
    static const float turningRate = 0.000000025f;
    m_rotation.y += m_thrust * turningRate * dt * m_roll;
}

const glm::vec3 & Plane::GetDirection() const { return m_direction; }

void Plane::SetDirection(const glm::vec3 & direction) {
    m_direction = direction;
}

void Plane::SetPitch(const float pitch) {
    m_rotation.x = glm::radians(pitch);
    m_pitch = pitch;
}

float Plane::GetPitch() const { return m_pitch; }

void Plane::SetRoll(const float roll) {
    m_rotation.z = glm::radians(roll);
    m_roll = roll;
}

float Plane::GetRoll() const { return m_roll; }
}
