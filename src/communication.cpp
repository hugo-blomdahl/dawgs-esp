#include "communication.hpp"

uint8_t Comms::selfAddress[6] = {0};
const char* Comms::TAG_COMMS = "Comms";

Comms::Comms(){

        esp_err_t err;
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_COMMS, "esp_wifi_init failed: %s", esp_err_to_name(err));
        return;
    }

    err = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_COMMS, "esp_wifi_set_storage failed: %s", esp_err_to_name(err));
        return;
    }

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_COMMS, "esp_wifi_set_mode failed: %s", esp_err_to_name(err));
        return;
    }

    err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG_COMMS, "esp_wifi_start failed: %s", esp_err_to_name(err));
        return;
    }

    err = esp_now_init();
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

    ESP_LOGI(TAG_COMMS, "ESP-NOW initialized");
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

    esp_err_t err = esp_now_send(address,
                                 reinterpret_cast<const uint8_t*>(message.data()),
                                 message.size());
    if (err != ESP_OK) {
        ESP_LOGE(TAG_COMMS, "esp_now_send failed: %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}
}
void Comms::broadcastMsg(std::string message){
    uint8_t broadcastAddress[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    sendMsg(broadcastAddress, message);
}

void Comms::initCommunication(){

}

