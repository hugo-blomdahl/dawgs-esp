#include "communication.hpp"
#include <cstring> 
#include <string>

Comms* Comms::instance = nullptr;
std::mutex Comms::listMutex;
uint8_t Comms::selfAddress[6] = {0};
const char* Comms::TAG_COMMS = "Comms";

Comms::Comms() {
    // Remove all the WiFi init code - it's now in app_main
    // Just do ESP-NOW specific initialization

    instance = this;

    esp_err_t err = esp_now_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG_COMMS, "esp_now_init failed: %s", esp_err_to_name(err));
        return;
    }

    uint8_t mac[6];
    if (esp_read_mac(mac, ESP_MAC_WIFI_STA) == ESP_OK) {
        ESP_LOGI(TAG_COMMS, "Self MAC: %02X:%02X:%02X:%02X:%02X:%02X",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        for(int i=0; i<6; i++){
            selfAddress[i] = mac[i];
        }
    } else {
        ESP_LOGE(TAG_COMMS, "Failed to read self MAC");
    }
    esp_now_register_recv_cb(Comms::onDataRecv);

    ESP_LOGI(TAG_COMMS, "ESP-NOW initialized");
}

void Comms::onDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len){
    
    MultiHopPacket* packet = (MultiHopPacket*)data; 
    //Filtering packets that are not meant for us could be done here
    if (packet->magic != 35) {
        return; // Not our packet
    }
    ESP_LOGW(TAG_COMMS, "The magic number is: %d", packet->magic);

    std::string msg(packet->payload, packet->payloadLen);  // Construct from pointer + length
    ESP_LOGI(TAG_COMMS, "Recieved %d bytes: %s", packet->payloadLen, msg.c_str());
    Message newMessage;
    memcpy(newMessage.macAddress, packet->srcMAC, 6);
    newMessage.message = msg;
    const std::lock_guard<std::mutex> lock(listMutex);
    instance->messages->push_back(newMessage);
    //ESP_LOGI(TAG_COMMS, "ATTEMPTING TO READ info: %02X", recv_info->src_addr[0]);
}



int Comms::sendMsg(uint8_t* address, std::string message){
    {
    /*if (message.size() > MAX_MSG_LEN) {
        ESP_LOGE(TAG_COMMS, "Message too long (%u > %u)", (unsigned)message.size(), (unsigned)MAX_MSG_LEN);
        return ESP_ERR_INVALID_SIZE;
    }*/

    // Ensure peer exists (for minimal effort: try add blindly if not present)
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, address, 6);
    peerInfo.channel = 0;
    peerInfo.ifidx   = WIFI_IF_STA;
    peerInfo.encrypt = false;

    if (!esp_now_is_peer_exist(address)) {
        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
            ESP_LOGE(TAG_COMMS, "Failed to add peer for send");
            return ESP_FAIL;
        }
    }
    MultiHopPacket packet;
    memcpy(packet.srcMAC, selfAddress, 6);
    memcpy(packet.dstMAC, address, 6);
    packet.ttl = 2; //changeme
    packet.msgID = esp_random();
    packet.payloadLen = message.length();
    memcpy(packet.payload, message.c_str(), packet.payloadLen);


    esp_err_t err = esp_now_send(address, (uint8_t*)&packet, packet.payloadLen+16);  //16 for packet structure
    if (err != ESP_OK) {
        ESP_LOGE(TAG_COMMS, "esp_now_send failed: %s", esp_err_to_name(err));
        return err;
    }

    std::cout << "sent message: " << message << std::endl;
    return ESP_OK;
}
}
void Comms::broadcastMsg(std::string message){
    uint8_t broadcastAddress[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    sendMsg(broadcastAddress, message);
    std::cout << "Broadcasted message: " << message << std::endl;
}

void Comms::setMessageList(std::list<Message>* messageList){
    messages = messageList;
}

std::mutex& Comms::getMutex(){
    return listMutex;
}