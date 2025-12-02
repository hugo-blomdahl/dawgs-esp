#pragma once
#include <stdint.h>
#include <string>
class Comms {
    private:
        static const uint8_t selfAddress[];
    public:
        Comms();
        int sendMsg(uint8_t* address, std::string message);
        void broadcastMsg();
        void initCommunication();
};
