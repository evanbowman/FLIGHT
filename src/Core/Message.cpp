#include <FLIGHT/Core/Message.hpp>

namespace FLIGHT {
void MessageReceiver::AcceptMessage(MessageRef msg) {
    m_messages.push_back(std::move(msg));
}
}
