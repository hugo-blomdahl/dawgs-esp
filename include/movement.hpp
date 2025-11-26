#pragma once
#include <stdint.h>
#include <string>
//
class Movement {
    private:
        float x = 0; //position x
        float y = 0; //position y
        float angle = 0; //rotation i grader
        int motorSpeed = 0;

    public:
        void setSpeed(int speedInPercent);
        void rotate(float wheelRotationInDegree);
        void calculateRotation();
        void moveForward();
        void moveBackward();
        void standby();
        void getCoordinate();
};