#pragma once
#include <stdint.h>
#include <string>
class Movement {
    public:
        void setSpeed(uint8_t* speed);
        void rotate(uint8_t* wheelRotation);
        void calculateRotation();
        void moveForward();
        void moveBackward();
        void standby();
};