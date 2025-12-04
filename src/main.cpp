extern "C" void app_main();

#include "receiverHook.hpp"
#include "esp_log.h"

static const char* TAG = "Main";

void app_main() {
    ReceiverHook receiver(1, 5, -1, 115200);
    //pin 5, tar endast emot, baud rate 115200

    ESP_LOGI(TAG, "Receiver running...");

    uint8_t buffer[256];

    while (true) {
        if (receiver.available()) {
            int len = receiver.read(buffer, sizeof(buffer));
            if (len > 0) {
                ESP_LOGI(TAG, "Received %d bytes", len);
                printf("%.*s", len, buffer); // print raw text
            }
        }
    }
}