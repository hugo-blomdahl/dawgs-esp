#pragma once
#include <stdint.h>
#include <string>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_mac.h"

class Comms {
    private:
        static uint8_t selfAddress[6];
        static const char* TAG_COMMS;
    public:
        Comms();
        int sendMsg(uint8_t* address, std::string message);
        void broadcastMsg(std::string message);
        void initCommunication();
};
