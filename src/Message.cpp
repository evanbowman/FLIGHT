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
    
MessageError::MessageError(Message::Id id) :
    std::runtime_error("Receiver failed to accept message of class "
		       + MessageIdToString(id)) {}
}
