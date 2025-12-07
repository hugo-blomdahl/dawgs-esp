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
}
void Movement::rotate(float wheelRotationInDegree){
    //roterar vänster eller höger hjul beroende på parameter
        if(wheelRotationInDegree > 0){
        //roterar till höger pivot turn
        //LW_A 1
        //LW_B 0
        //RW_A 0
        //RW_B 0

        angle += wheelRotationInDegree; //positivt

    }else if(wheelRotationInDegree < 0){
        //roterar till vänster pivot turn
        //LW_A 1
        //LW_B 0
        //RW_A 0
        //RW_B 0

        angle += wheelRotationInDegree; //negativt

    }else{
        //gö inget
        //standby();
    }

    // begränsa vinkel 0–359
    if (angle >= 360) {
        angle -= 360;
    }
    else if (angle < 0) {
        angle += 360;
    }
    else {
        //vinkel okej eller något är fel
    }
    std::cout << "Angle: " << angle << std::endl;
}

void Movement::moveForward(){
    if(motorSpeed > 0){
        //förflyttas framåt
        //LW_A 1
        //LW_B 0
    
        //RW_A 1
        //RW_B 0
        std::cout << "Forward: " << motorSpeed << std::endl;
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
        std::cout << "Backward: " << motorSpeed << std::endl;
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
}
