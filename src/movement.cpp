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
}

void Movement::moveForward(){
    //förflyttas framåt
    //LW_A 1
    //LW_B 0

    //RW_A 1
    //RW_B 0
    float rad = angle * M_PI / 180; //omvandlar grader till radian
    if(motorSpeed > 0){
        x += motorSpeed * std::cos(rad); 
        y += motorSpeed * std::sin(rad);
    }else{
        standby();
    }
}

void Movement::moveBackward(){
    //förflyttas backåt
    //LW_A 0
    //LW_B 1

    //RW_A 0
    //RW_B 1
    float rad = angle * M_PI / 180; //omvandlar grader till radian
    
    if(motorSpeed > 0){
        x -= motorSpeed * std::cos(rad);
        y -= motorSpeed * std::sin(rad);
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
    
}

void Movement::getCoordinate() {
    //print(x);
    //print(y);
    //print(angle);
    std::cout << "x: " << std::floor(x) << " y: " << std::floor(y) << " Angle: " << angle << std::endl;
}