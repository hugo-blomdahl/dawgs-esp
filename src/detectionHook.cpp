#include "include/receiverHook.hpp"

extern "C" {
    #include "driver/uart.h"
    #include "esp_log.h"
}

static const char* TAG = "ReceiverHook";

ReceiverHook::ReceiverHook(int uartNum, int rxPin, int txPin, int baud)
    : uartNum_(uartNum)
{
    uart_config_t cfg = {
        .baud_rate = baud,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // Install RX buffer only
    uart_driver_install((uart_port_t)uartNum_, 2048, 0, 0, nullptr, 0);
    uart_param_config((uart_port_t)uartNum_, &cfg);
    uart_set_pin((uart_port_t)uartNum_, txPin, rxPin,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    ESP_LOGI(TAG, "UART initialized on RX=%d", rxPin);
}

ReceiverHook::~ReceiverHook() {
    uart_driver_delete((uart_port_t)uartNum_);
}

bool ReceiverHook::available() const {
    size_t len = 0;
    uart_get_buffered_data_len((uart_port_t)uartNum_, &len);
    return len > 0;
}

int ReceiverHook::read(uint8_t* buffer, size_t maxLen, int timeoutMs) {
    return uart_read_bytes(
        (uart_port_t)uartNum_,
        buffer,
        maxLen,
        timeoutMs / portTICK_PERIOD_MS
    );
}
