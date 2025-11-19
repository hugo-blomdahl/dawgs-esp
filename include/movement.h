#pragma once
#include <stdint.h>
#include <string>
class Movement {
    public:
        void setSpeed(int speed);
        void rotate(int wheelRotation);
        void calculateRotation();
        void moveForward();
        void moveBackward();
        void standby();
};