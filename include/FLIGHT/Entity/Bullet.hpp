#pragma once

#include <FLIGHT/Entity/Entity.hpp>
#include <FLIGHT/Graphics/Model.hpp>
#include <FLIGHT/Util/Time.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

namespace FLIGHT {
class Bullet : public Entity {
    std::weak_ptr<Model> m_model;

public:
    void SetModel(std::shared_ptr<Model> model);
    void Display(DisplayImpl & renderer) override;
    void Update(const Time dt) override;
};
}
