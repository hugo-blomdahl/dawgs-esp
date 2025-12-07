#pragma once

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"

class TcpClient {
public:
    TcpClient(const char* ssid, const char* password, const char* serverIp, int serverPort);
    ~TcpClient();

    void start();

private:
    const char* ssid;
    const char* password;
    const char* serverIp;
    int serverPort;
    int sock;

    EventGroupHandle_t wifiEventGroup;
    static const int WIFI_GOT_IP_BIT = BIT1;

    static const int WIFI_CONNECTED_BIT = BIT0;
    TaskHandle_t clientTaskHandle;

    void initWiFi();
    int connectToServer();
    void clientTask();

    static void wifiEventHandler(void* arg, esp_event_base_t event_base,
                                 int32_t event_id, void* event_data);
    static void clientTaskWrapper(void* pvParameters);
};
