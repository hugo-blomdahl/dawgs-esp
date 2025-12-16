#include "movement.hpp"
#include <stdint.h>
#include <cmath>
#include <iostream>

//Har inga Pins att användas på riktigt
#define LEFT_WHEEL_A //clockvis
#define LEFT_WHEEL_B //anti-clockvis
#define RIGHT_WHEEL_A //clockvis
#define RIGHT_WHEEL_B //anti-clockvis

void Movement::setSpeed(int speedInPercent){
    //sätter hastigheten

    if(speedInPercent < 0){
        speedInPercent = 0;
    }
    else if(speedInPercent > 100){
        speedInPercent = 100;
    }else{
        //gö något kanske
    }
    
    motorSpeed = speedInPercent;

    // Beräkna linjär hastighet baserat på rpm och hjul
    float linearSpeed = (motorSpeed / 100.0f) * MAX_SPEED; // m/s

    // Hur långt hjulen rör sig under detta steg
    float distanceTraveled = linearSpeed * SIM_STEP; // meter per uppdatering

    std::cout << "Moving forward: " << distanceTraveled << " m (speed " 
              << motorSpeed << "%)" << std::endl;
    /*if (tcpClient) {
    tcpClient->sendString("speed:" + std::to_string(motorSpeed) + "\n");
    }//*/
}
void Movement::rotate(float wheelRotationInDegree){
    //roterar vänster eller höger hjul beroende på parameter
        if(wheelRotationInDegree > 0){
        //roterar till höger pivot turn
        //LW_A 1
        //LW_B 0
        //RW_A 0
        //RW_B 0

        angle = wheelRotationInDegree; //positivt

    }else if(wheelRotationInDegree < 0){
        //roterar till vänster pivot turn
        //LW_A 1
        //LW_B 0
        //RW_A 0
        //RW_B 0

        angle = wheelRotationInDegree; //negativt

    }else{
        //gö inget
        //standby();
    }
    //teoretisk bågsträcka (s = L * theta)
    float Rad = angle * M_PI / 180.0f;
    float s = WHEEL_BASE * Rad; //pivot turn med ett hjul stilla
    float wheelRevs = s / WHEEL_CIRC;

    std::cout << "Rotating: " << angle 
              << " deg, wheelRevs=" << wheelRevs << std::endl;

    std::cout << "Angle: " << angle << std::endl;
    /*if (tcpClient) {
        tcpClient->sendString("rotation:" + std::to_string(angle) + "\n");
    }//*/
}

void Movement::moveForward(){
    if(motorSpeed > 0){
        //förflyttas framåt
        //LW_A 1
        //LW_B 0
    
        //RW_A 1
        //RW_B 0
        //beräkna linjär hastighet beroende på motorSpeed
        float v = (motorSpeed / 100.0f) * MAX_SPEED; //m/s
        float distance = v * SIM_STEP; //meter per uppdatering

        std::cout << "Move forward: " << distance 
                  << " m (speed " << motorSpeed << "%)" << std::endl;

        std::cout << "Forward: " << motorSpeed << std::endl;
        /*if (tcpClient) {
            tcpClient->sendString("forward\n");
        }//*/
    }else{
        standby();
    }
}

void Movement::moveBackward(){
    
    if(motorSpeed > 0){
        //förflyttas backåt
        //LW_A 0
        //LW_B 1
    
        //RW_A 0
        //RW_B 1
        //samma beräkning, men negativt för bakåt
        float v = (motorSpeed / 100.0f) * MAX_SPEED; //m/s
        float distance = -v * SIM_STEP; //meter per uppdatering

        std::cout << "Move backward: " << distance 
                  << " m (speed " << motorSpeed << "%)" << std::endl;

        std::cout << "Backward: " << motorSpeed << std::endl;
        /*if (tcpClient) {
            tcpClient->sendString("backward\n");
        }//*/
    }else{
        standby();
    }
}

void Movement::standby(){
    //står stilla
    //LW_A 0
    //LW_B 0
    //RW_A 0
    //RW_B 0
    motorSpeed = 0;
    std::cout << "Standby" << std::endl;
    /*if (tcpClient) {
        tcpClient->sendString("standby\n");
    }//*/
    
}