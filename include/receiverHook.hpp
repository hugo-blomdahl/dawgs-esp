#pragma once
#include <cstdint>
#include <cstddef>

class ReceiverHook {
public:
    ReceiverHook(int uartNum, int rxPin, int txPin = -1, int baud = 115200);
    ~ReceiverHook();

    bool available() const;
    int read(uint8_t* buffer, size_t maxLen, int timeoutMs = 20);

private:
    int uartNum_;
};