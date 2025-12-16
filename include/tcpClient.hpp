#pragma once

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"

class TcpClient {
public:
    TcpClient(const char* serverIp, int serverPort, EventGroupHandle_t wifiEvents, const int connectedBit);
    ~TcpClient();
    void start();

private:
    const char* serverIp;
    int serverPort;
    int sock;
    TaskHandle_t clientTaskHandle;

    EventGroupHandle_t wifiEventGroup;
    const int WIFI_CONNECTED_BIT;
    static void clientTaskWrapper(void* pvParamaters);
    int connectToServer();
    void clientTask();
};
