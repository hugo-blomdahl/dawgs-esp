#include "navigation.h"
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

#define UART_RX_PORT    UART_NUM_1
#define UART_RX_BAUD    115200
#define UART_RX_PIN     5
#define UART_RX_BUF_SZ  1024
#define READ_CHUNK_SZ   256

// UART reader task, förväntar sig att varje QR-ID avslutas med '\n' 
static void uart_reader_task(void* arg) {
    Navigation* navigation = static_cast<Navigation*>(arg);
    uint8_t* buf = (uint8_t*) malloc(READ_CHUNK_SZ);
    if (!buf) { vTaskDelete(NULL); return; }

    std::string line;

    while (true) {
        int len = uart_read_bytes(UART_RX_PORT, buf, READ_CHUNK_SZ, pdMS_TO_TICKS(200));
        if (len > 0) {
            for (int i = 0; i < len; ++i) {
                char c = static_cast<char>(buf[i]);
                if (c == '\n') {
                    if (!line.empty()) {
                        navigation->calibrateFromQR(line); 
                        line.clear();
                    }
                } else {
                    line.push_back(c);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    free(buf);
    vTaskDelete(NULL);
}


// start funktion för UART reader task
void startUARTReader(Navigation* navigation) {
    uart_config_t uart_cfg = {};
    uart_cfg.baud_rate = UART_RX_BAUD;
    uart_cfg.data_bits = UART_DATA_8_BITS;
    uart_cfg.parity = UART_PARITY_DISABLE;
    uart_cfg.stop_bits = UART_STOP_BITS_1;

    uart_driver_install(UART_RX_PORT, UART_RX_BUF_SZ, 0, 0, NULL, 0);
    uart_param_config(UART_RX_PORT, &uart_cfg);
    uart_set_pin(UART_RX_PORT, UART_PIN_NO_CHANGE, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    xTaskCreate(uart_reader_task, "uart_reader", 4096, navigation, 5, NULL);
}
