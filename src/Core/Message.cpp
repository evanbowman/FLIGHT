#include <FLIGHT/Core/Message.hpp>

namespace FLIGHT {
void MessageTarget::SendMessage(const Message & msg) {
    m_inbox.Push(msg);
}

void MessageBuffer::Push(const Message & msg) {
    m_messages.push_back(msg);
}

void MessageBuffer::Clear() { m_messages.clear(); }
}
