#pragma once
#include <stdint.h>
#include <string>
//
class Movement {
    private:
        float x = 0;       // position X
        float y = 0;       // position Y
        float angle = 0;   // rotation i grader (0–359)
        int motorSpeed = 0;

    public:
        void setSpeed(int speedInPercent);
        void rotate(int wheelRotationInPercent);
        void calculateRotation();
        void moveForward();
        void moveBackward();
        void standby();
        void getCoordinate();
};