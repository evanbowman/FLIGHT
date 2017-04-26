#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <mapbox/variant.hpp>

namespace FLIGHT {

struct PickedUpCoin {};

class Solid;

struct Collision {
    std::shared_ptr<Solid> with;
    Collision(std::shared_ptr<Solid> with) { this->with = with; }
};

class Death {};

struct TerrainCollision {};
    
using Message = mapbox::util::variant<Collision,
                                      Death,
                                      TerrainCollision,
                                      PickedUpCoin>;

class MessageBuffer {
    std::vector<std::unique_ptr<Message>> m_messages;

public:
    std::unique_ptr<Message> Poll();
    void Push(std::unique_ptr<Message> msg);
    void Clear();
};

class MessageTarget {
protected:
    MessageBuffer m_outbox;
    MessageBuffer m_inbox;

public:
    std::unique_ptr<Message> PollMessages();
    void SendMessage(std::unique_ptr<Message> msg);
};
}
