#pragma once

#include "Message.hpp"

namespace FLIGHT {
class Manager : public MessageReceiver {
public:
    virtual ~Manager() {}
};
}
