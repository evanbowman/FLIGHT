#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <mapbox/variant.hpp>
#include <functional>
#include <FLIGHT/Util/MemPool.hpp>
#include <FLIGHT/Util/Singleton.hpp>

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

using MessageRef = std::unique_ptr<Message, std::function<void(Message *)>>;

template <typename T, typename ...Args>
MessageRef NewMessage(Args && ...args) {
    return {
        Singleton<MemPool<Message>>::Instance().New(T(std::forward<Args>(args)...)),
        [](Message * msg) {
            Singleton<MemPool<Message>>::Instance().Delete(msg);
        }
    };
}

class MessageReceiver {
    std::vector<MessageRef> m_messages;
public:
    void AcceptMessage(MessageRef msg);
    template <typename F>
    void PollMessages(F && callback) {
        for (auto & msg : m_messages) {
            callback(std::move(msg));
        }
        m_messages.clear();
    }
};

class MessageError : public std::runtime_error {
public:
    MessageError() : std::runtime_error("Invalid message") {}
};
}
