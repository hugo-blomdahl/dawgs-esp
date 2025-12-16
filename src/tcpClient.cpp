#include "tcpClient.hpp"
#include <string.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

static const char *TAG = "TcpClient";
static const int RECONNECT_DELAY_MS = 5000;

TcpClient::TcpClient(const char* serverIp, int serverPort, EventGroupHandle_t wifiEvents, const int connectedBit)
    : serverIp(serverIp), serverPort(serverPort), sock(-1), clientTaskHandle(nullptr), wifiEventGroup(wifiEvents),
    WIFI_CONNECTED_BIT(connectedBit) {  
}



TcpClient::~TcpClient() {
    if (clientTaskHandle) {
        vTaskDelete(clientTaskHandle);
    }
    if (sock >= 0) {
        close(sock);
    }
}

/*void TcpClient::wifiEventHandler(void* arg, esp_event_base_t event_base,
                                 int32_t event_id, void* event_data) {
    TcpClient* client = static_cast<TcpClient*>(arg);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WiFi disconnected, reconnecting...");
        xEventGroupClearBits(client->wifiEventGroup, WIFI_CONNECTED_BIT);
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(client->wifiEventGroup, WIFI_CONNECTED_BIT);
    }
}*/

/*void TcpClient::wifiEventHandler(void* arg, esp_event_base_t event_base,
                                 int32_t event_id, void* event_data) {
    TcpClient* client = static_cast<TcpClient*>(arg);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "WiFi started, connecting...");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t* disconnected = (wifi_event_sta_disconnected_t*) event_data;
        ESP_LOGW(TAG, "WiFi disconnected, reason: %d, reconnecting...", disconnected->reason);
        xEventGroupClearBits(client->wifiEventGroup, WIFI_CONNECTED_BIT | WIFI_GOT_IP_BIT);
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI(TAG, "WiFi connected, waiting for IP...");
        xEventGroupSetBits(client->wifiEventGroup, WIFI_CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(client->wifiEventGroup, WIFI_GOT_IP_BIT);
    }
}*/


/*
void TcpClient::initWiFi() {
    ESP_LOGI(TAG, "Configuring WiFi STA connection...");

    // Don't create netif - it's already created in app_main

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &TcpClient::wifiEventHandler,
                                                        this,
                                                        nullptr));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &TcpClient::wifiEventHandler,
                                                        this,
                                                        nullptr));

    wifi_config_t wifi_config = {};
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    ESP_LOGI(TAG, "Connecting to SSID: %s", ssid);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_connect());

    ESP_LOGI(TAG, "WiFi connection initiated");
}
*/



int TcpClient::connectToServer() {
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(serverIp);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(serverPort);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return -1;
    }

    ESP_LOGI(TAG, "Connecting to %s:%d", serverIp, serverPort);
    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket connect failed: errno %d", errno);
        close(sock);
        sock = -1;
        return -1;
    }

    ESP_LOGI(TAG, "Connected to server");
    return sock;
}

/*void TcpClient::clientTask() {
    char rx_buffer[256];
    std::string line_buffer;

    while (1) {
        ESP_LOGI(TAG, "Waiting for WiFi and IP...");
        xEventGroupWaitBits(wifiEventGroup, 
                           WIFI_CONNECTED_BIT, 
                           false, true, portMAX_DELAY);

        ESP_LOGI(TAG, "WiFi ready, connecting to server...");


        if (connectToServer() < 0) {
            ESP_LOGE(TAG, "Connection failed, retrying in %d ms", RECONNECT_DELAY_MS);
            vTaskDelay(RECONNECT_DELAY_MS / portTICK_PERIOD_MS);
            continue;
        }

        uint8_t mac[6];
        esp_wifi_get_mac(WIFI_IF_STA, mac);
        
        char macStr[20];
        // Format MAC as XX:XX:XX:XX:XX:XX followed by newline
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X\n",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        ESP_LOGI(TAG, "Sending Handshake: %s", macStr);
        int sent = send(sock, macStr, strlen(macStr), 0);
        
        if (sent < 0) {
             ESP_LOGE(TAG, "Failed to send MAC handshake");
             close(sock);
             continue;
        }

        while (1) {
            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);

            if (len < 0) {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
                break;
            } else if (len == 0) {
                ESP_LOGW(TAG, "Connection closed by server");
                break;
            }

            for (int i = 0; i < len; i++) {
                if (rx_buffer[i] == '\n') {
                    if (!line_buffer.empty()) {
                        printf("RECEIVED: %s\n", line_buffer.c_str());
                        line_buffer.clear();
                    }
                } else if (rx_buffer[i] != '\r') {
                    line_buffer += rx_buffer[i];
                }
            }
        }

        if (sock >= 0) {
            close(sock);
            sock = -1;
        }
        ESP_LOGI(TAG, "Reconnecting in %d ms", RECONNECT_DELAY_MS);
        vTaskDelay(RECONNECT_DELAY_MS / portTICK_PERIOD_MS);
    }
}
*/

void TcpClient::clientTask() {
    char rx_buffer[256];
    
    while (1) {
        ESP_LOGI(TAG, "Waiting for WiFi (Shared Event Group)...");
        
        // Wait for the MAIN app to tell us WiFi is ready
        xEventGroupWaitBits(wifiEventGroup, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

        ESP_LOGI(TAG, "WiFi is up. Attempting TCP connection to %s...", serverIp);

        if (connectToServer() < 0) {
            vTaskDelay(pdMS_TO_TICKS(RECONNECT_DELAY_MS));
            continue;
        }

        // --- Send Handshake (MAC) ---
        uint8_t mac[6];
        esp_wifi_get_mac(WIFI_IF_STA, mac); // This is safe to call from multiple threads
        char macStr[30];
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X\n",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        send(sock, macStr, strlen(macStr), 0);

        // --- Receive Loop ---
        while (1) {
            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len <= 0) {
                ESP_LOGE(TAG, "Connection closed or error");
                break;
            }
            rx_buffer[len] = 0; // Null terminate
            ESP_LOGI(TAG, "Received from %s: %s", serverIp, rx_buffer);
        }

        if (sock >= 0) { close(sock); sock = -1; }
        vTaskDelay(pdMS_TO_TICKS(RECONNECT_DELAY_MS));
    }
}

void TcpClient::clientTaskWrapper(void* pvParameters) {
    static_cast<TcpClient*>(pvParameters)->clientTask();
}

void TcpClient::start() {
    xTaskCreate(clientTaskWrapper, "tcp_client", 4096, this, 5, &clientTaskHandle);
}
