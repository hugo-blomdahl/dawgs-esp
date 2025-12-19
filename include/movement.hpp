#pragma once
//#include "tcpClient.hpp"
#include <string>
#include <cmath>

class Movement {
public:
    //Movement(TcpClient* tcp) : tcpClient(tcp) {}

    void setSpeed(int speedInPercent);
    void rotate(float wheelRotationInDegree);
    void moveForward();
    void moveBackward();
    void standby();

private:
    //motor/hjulkonstanter
    const float WHEEL_DIAMETER = 0.065f; //meter
    const float WHEEL_CIRC = M_PI * WHEEL_DIAMETER; //hjulomkrets
    const float MOTOR_RPM = 230.0f; //obelastad
    const float MOTOR_RPS = MOTOR_RPM / 60.0f; //rev/s
    const float MAX_SPEED = MOTOR_RPS * WHEEL_CIRC; //m/s
    const float WHEEL_BASE = 0.10f; //avstånd mellan hjulens mitt
    const float SIM_STEP = 0.1f; //simuleringstid per steg (sek)

    int motorSpeed = 0;
    float angle = 0;
    //TcpClient* tcpClient;
};
