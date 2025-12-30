#include "navigation.h"
#include "tcpClient.hpp"
#include <string>
#include <charconv>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"

#define UART_RX_PORT    UART_NUM_1
#define UART_RX_BAUD    115200
#define UART_RX_PIN     5
#define UART_RX_BUF_SZ  1024
#define READ_CHUNK_SZ   256
static const char *TAG_QR_RECEIVER = "QR_RECV";

struct TaskParams {
    Navigation* navigation;
    TcpClient* tcpClient; // The new object you need
};

static void uart_reader_task(void* arg) {
    TaskParams* params = static_cast<TaskParams*>(arg);
    Navigation* navigation = params->navigation;
    TcpClient* tcpClient = params->tcpClient;
    delete params;
    uint8_t* buf = (uint8_t*) malloc(READ_CHUNK_SZ);
    if (!buf) { 
        ESP_LOGE(TAG_QR_RECEIVER, "Kunde inte allokera buffert.");
        vTaskDelete(NULL); 
        return; 
    }

    std::string line;

    while (true) {
        int len = uart_read_bytes(UART_RX_PORT, buf, READ_CHUNK_SZ, pdMS_TO_TICKS(200));
        if (len > 0) {
            for (int i = 0; i < len; ++i) {
                char c = static_cast<char>(buf[i]);
                if (c == '\n') {
                    if (!line.empty()) {
                        int qrIndex = 0;
                        auto [ptr, ec] = std::from_chars(line.data(), line.data() + line.size(), qrIndex);
                        if (ec == std::errc::invalid_argument) {
                            ESP_LOGE(TAG_QR_RECEIVER, "QR ID '%s' är ej ett giltigt heltal.", line.c_str());
                        } else if (ec == std::errc::result_out_of_range) {
                            ESP_LOGE(TAG_QR_RECEIVER, "QR ID '%s' är utom omfång för int.", line.c_str());
                        } else {
                            navigation->calibrateFromQR(qrIndex);
                        }

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

void startUARTReader(Navigation* navigation, TcpClient* tcpClient) {
    uart_config_t uart_cfg = {};
    uart_cfg.baud_rate = UART_RX_BAUD;
    uart_cfg.data_bits = UART_DATA_8_BITS;
    uart_cfg.parity = UART_PARITY_DISABLE;
    uart_cfg.stop_bits = UART_STOP_BITS_1;

    uart_driver_install(UART_RX_PORT, UART_RX_BUF_SZ, 0, 0, NULL, 0);
    uart_param_config(UART_RX_PORT, &uart_cfg);
    uart_set_pin(UART_RX_PORT, UART_PIN_NO_CHANGE, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE); 

    TaskParams* params = new TaskParams;

    // 2. Assign your objects
    params->navigation = navigation;
    params->tcpClient = tcpClient;

    xTaskCreate(uart_reader_task, "uart_reader", 4096, params, 5, NULL);
}
