#pragma once
#include "tcpClient.hpp"
#include <esp_log.h>
#include "navigation.h"

class TestClient : public TcpClient{
    private:
    Navigation* navigation;

    public:
        TestClient(const char* ip, int port, EventGroupHandle_t events, int bit, Navigation* navigation)
            : TcpClient(ip, port, events, bit) {
                this->navigation = navigation;
            }
    
    protected:
        //overwrites
        void handleMessage(const char* data, int len) override {
            static const char *TAG = "TestClient";
            std::string command(data);

            if (command.find("qr ") == 0){
                ESP_LOGI(TAG, "fick qr test!", nullptr);
                std::string numStr = command.substr(3);
                    int val = std::stoi(numStr);
                    navigation->calibrateFromQR(val);
                
            }
            else {
                //use default implementation
                TcpClient::handleMessage(data,len);
            }
        }
};