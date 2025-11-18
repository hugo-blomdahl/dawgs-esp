#pragma once
#include <stdint.h>
#include <string>
class Movement {
    public:
        int sendMsg(uint8_t* address, std::string message);
        void broadcastMsg();
        void initCommunication();
};
