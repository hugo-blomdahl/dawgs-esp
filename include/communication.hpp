#pragma once
#include <stdint.h>
#include <string>
#include <iostream>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_mac.h"
#include "nvs_flash.h"
#include <esp_random.h>
#include <list>
#include <mutex>
#include <algorithm>
#define BUFFER_SIZE 45
struct Message {
        uint8_t macAddress[6];
        std::string message;
    };
class PacketHistory {
        private:
            uint16_t buffer[BUFFER_SIZE] = {0};
            size_t head = 0;
            bool full = false;
            
        public:
            bool exists(uint16_t msgID) const {
                size_t count = full ? BUFFER_SIZE : head;
                for(size_t i=0; i<count; i++){
                    if(buffer[i] == msgID) return true;
                }
                return false;
            }
            
            void add(uint16_t msgID){
                buffer[head] = msgID;
                head = (head + 1) % BUFFER_SIZE;
                if(head == 0) full = true;
            }
    };

class Comms {
    private:
        static Comms* instance;
        static uint8_t selfAddress[6];
        static const char* TAG_COMMS;
        static void onDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);
        std::list<Message>* messages = nullptr;
        static std::mutex listMutex;
        
        PacketHistory packetHistory;
        
// Total: 15 bytes header + up to 235 bytes payload

    public:
        Comms();
        struct MultiHopPacket {
            uint8_t srcMAC[6];      // Original sender (6 bytes)
            uint8_t dstMAC[6];      // Final destination (6 bytes)
            uint8_t ttl;            // Hops remaining (1 byte)
            uint16_t msgID;          // Unique message ID (2 byte)
            uint8_t payloadLen;     // How long is the message? (1 byte)
            char payload[234];      // Actual message (rest of 250 bytes)
        };
        int sendMsg(uint8_t* address, std::string message);
        void broadcastMsg(std::string message);
        void setMessageList(std::list<Message>* messageList);
        static std::mutex& getMutex();
};
