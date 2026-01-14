#include "navigation.h"
#include <string>
#include <charconv>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"

#include "activityPlanner.hpp"

#define UART_RX_PORT    UART_NUM_1
#define UART_RX_BAUD    115200
#define UART_RX_PIN     5
#define UART_RX_BUF_SZ  1024
#define READ_CHUNK_SZ   256

static const char *TAG_QR_RECEIVER = "QR_RECV";

struct UartContext {
    Navigation* nav;
    ActivityPlanner* planner;
}; 

static void uart_reader_task(void* arg) {

    UartContext* ctx = static_cast<UartContext*>(arg); // 1. Packa upp lådan
    Navigation* navigation = ctx->nav;                 // 2. Hämta nav
    ActivityPlanner* planner = ctx->planner;           // 3. Hämta planner
    delete ctx;                                        // 4. Kasta lådan (frigör minne)

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
                            
                            // hämta art info
                            std::string artName = "";
                            if (navigation != nullptr) {
                                artName = navigation->getNodeArt(qrIndex);
                            }

                            // skicka logg
                            if (planner != nullptr) {
                                char logMsg[128];

                                if (artName.empty()) {
                                    snprintf(logMsg, sizeof(logMsg), "NAV - QR %d Scanned (Calibration)\n", qrIndex);
                                } else {
                                    snprintf(logMsg, sizeof(logMsg), "DET - QR %d Scanned (%s)\n", qrIndex, artName.c_str());
                                }
                                
                                planner->sendLog(logMsg);
                            }
                            
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

void startUARTReader(Navigation* navigation, ActivityPlanner* activityPlanner) {
    uart_config_t uart_cfg = {};
    uart_cfg.baud_rate = UART_RX_BAUD;
    uart_cfg.data_bits = UART_DATA_8_BITS;
    uart_cfg.parity = UART_PARITY_DISABLE;
    uart_cfg.stop_bits = UART_STOP_BITS_1;

    uart_driver_install(UART_RX_PORT, UART_RX_BUF_SZ, 0, 0, NULL, 0);
    uart_param_config(UART_RX_PORT, &uart_cfg);
    uart_set_pin(UART_RX_PORT, UART_PIN_NO_CHANGE, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE); 

    UartContext* ctx = new UartContext{navigation, activityPlanner};

    xTaskCreate(uart_reader_task, "uart_reader", 4096, ctx, 5, NULL);
}