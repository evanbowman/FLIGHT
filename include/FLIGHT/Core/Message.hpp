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
    std::vector<Message> m_messages;

public:
    template <typename F>
    void Poll(F && cb) {
        for (auto & msg : m_messages) {
            cb(msg);
        }
        m_messages.clear();
    }
    void Push(const Message & msg);
    void Clear();
};

class MessageTarget {
protected:
    MessageBuffer m_outbox;
    MessageBuffer m_inbox;

public:
    template <typename F>
    void PollMessages(F && cb) {
        m_outbox.Poll(cb);
    }
    void SendMessage(const Message & msg);
};

class MessageError : public std::runtime_error {
public:
    MessageError() : std::runtime_error("Invalid message") {}
};
}
