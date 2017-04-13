#include <FLIGHT/Core/Message.hpp>

namespace FLIGHT {
static std::string MessageIdToString(Message::Id id) {
    switch (id) {
    case Message::Id::Collision:
        return "Collision";

    case Message::Id::TerrainCollision:
        return "TerrainCollision";

    case Message::Id::Death:
        return "Death";

    case Message::Id::PickedUpCoin:
        return "PickedUpCoin";
    }
}

std::unique_ptr<Message> MessageTarget::PollMessages() {
    return m_outbox.Poll();
}

void MessageTarget::SendMessage(std::unique_ptr<Message> msg) {
    m_inbox.Push(std::move(msg));
}

MessageError::MessageError(Message::Id id)
    : std::runtime_error("Receiver failed to accept message of class " +
                         MessageIdToString(id)) {}

std::unique_ptr<Message> MessageBuffer::Poll() {
    if (not m_messages.empty()) {
        auto msg = std::move(m_messages.back());
        m_messages.pop_back();
        return msg;
    }
    return nullptr;
}

void MessageBuffer::Push(std::unique_ptr<Message> msg) {
    m_messages.push_back(std::move(msg));
}

void MessageBuffer::Clear() { m_messages.clear(); }
}
