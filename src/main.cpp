#include "navigation.h"
#include "QRReceiver.h"
#include "freertos/FreeRTOS.h" // Needed for vTaskDelay
#include "freertos/task.h"     // Needed for vTaskDelay
extern "C" void app_main() {
    Navigation navigation;
    startUARTReader(&navigation);
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Sleep for 1 second repeatedly
    }
}