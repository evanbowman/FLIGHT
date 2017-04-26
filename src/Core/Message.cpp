#include <FLIGHT/Core/Message.hpp>

namespace FLIGHT {
std::unique_ptr<Message> MessageTarget::PollMessages() {
    return m_outbox.Poll();
}

void MessageTarget::SendMessage(std::unique_ptr<Message> msg) {
    m_inbox.Push(std::move(msg));
}

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
