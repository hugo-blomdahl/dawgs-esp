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
            handleMessage(rx_buffer, len);
        }

        if (sock >= 0) { close(sock); sock = -1; }
        vTaskDelay(pdMS_TO_TICKS(RECONNECT_DELAY_MS));
    }
}

void TcpClient::handleMessage(const char* data, int len) {
    ESP_LOGI(TAG, "Received from %s: %s", serverIp, data);
}

void TcpClient::clientTaskWrapper(void* pvParameters) {
    static_cast<TcpClient*>(pvParameters)->clientTask();
}

void TcpClient::start() {
    xTaskCreate(clientTaskWrapper, "tcp_client", 4096, this, 5, &clientTaskHandle);
}

int TcpClient::sendString(const std::string& msg) {
    if (sock < 0) {
        return -1;
    }

    int err = send(sock, msg.c_str(), msg.length(), 0);
    if (err < 0) {
        return -1;
    }

    return err;
}
