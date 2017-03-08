#include "Message.hpp"

namespace FLIGHT {
static std::string MessageIdToString(Message::Id id) {
    switch (id) {
    case Message::Id::Collision:
        return "Collision";

    case Message::Id::PickedUpCoin:
        return "PickedUpCoin";
    }
}

MessageError::MessageError(Message::Id id)
    : std::runtime_error("Receiver failed to accept message of class " +
                         MessageIdToString(id)) {}

std::unique_ptr<Message> MessageBuffer::Poll() {
    if (!m_messages.empty()) {
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
