#pragma once
#include <stdint.h>
#include <string>
class Comms {
    public:
        int sendMsg(uint8_t* address, std::string message);
        void broadcastMsg();
        void initCommunication();
};
