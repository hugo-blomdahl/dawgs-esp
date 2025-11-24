#include "communication.hpp"
#include <stdint.h>

Comms::Comms(){
    uint8_t selfAddress[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
}

int Comms::sendMsg(uint8_t* address, std::string message){
    return 0;
}

void Comms::broadcastMsg(){
    return;
}

void Comms::initCommunication(){
    //wait for random amount of time? so not everyone starts at the same time?
    return;
}
