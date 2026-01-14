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
            else if (command == "route 5"){
                int route[] = {9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50};
                //navigation->setRoute(5, route, 42);
            }
            else {
                //use default implementation
                TcpClient::handleMessage(data,len);
            }
        }
};