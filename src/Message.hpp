#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace FLIGHT {
class Message {
public:
    enum class Id { PickedUpCoin, Collision, TerrainCollision };
    virtual Id GetId() = 0;
    virtual ~Message() {}
};

class MessageBuffer {
    std::vector<std::unique_ptr<Message>> m_messages;

public:
    std::unique_ptr<Message> Poll();
    void Push(std::unique_ptr<Message> msg);
    void Clear();
};

class MessageError : public std::runtime_error {
public:
    explicit MessageError(Message::Id id);
};

struct PickedUpCoin : public Message {
    Id GetId() override { return Id::PickedUpCoin; }
};

class Solid;

struct Collision : public Message {
    Id GetId() override { return Id::Collision; }
    std::shared_ptr<Solid> with;
    Collision(std::shared_ptr<Solid> with) { this->with = with; }
};

struct TerrainCollision : public Message {
    Id GetId() override { return Id::TerrainCollision; }
};
}
