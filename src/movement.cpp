#include "movement.hpp"
#include <stdint.h>

//Har inga Pins att användas på riktigt
#define LEFT_WHEEL_A //clockvis
#define LEFT_WHEEL_B //anti-clockvis
#define RIGHT_WHEEL_A //clockvis
#define RIGHT_WHEEL_B //anti-clockvis

void Movement::setSpeed(int speedInPercent){
    //sätter hastigheten
    //motorSpeed = speed; //?
    if(speedInPercent < 0){
        //speedInPercent = 0;
    }
    else if(speedInPercent > 100){
        //speedInPercent = 100;
    }else{
        //Serial.print("ERROR setSpeed");
    }
    //motorSpeed = speedInPercent;
}
void Movement::rotate(int wheelRotationInPercent){
    //roterar vänster eller höger hjul beroende på parameter
        if(wheelRotationInPercent > 0){
        //roterar till höger
        //LW_A 1
        //LW_B 0
        
        //RW_A 0
        //RW_B 1
        //angle += wheelRotationInPercent; //positivt

    }else if(wheelRotationInPercent < 0){
        //roterar till vänster
        //LW_A 0
        //LW_B 1

        //RW_A 1
        //RW_B 0
        //angle += wheelRotationInPercent; //negativt

    }else{
        //gö inget
        //standby();
    }

    // begränsa 0–359
    if (angle >= 360) {
        //angle -= 360;
    }
    else if (angle < 0) {
        //angle += 360;
    }
    else {
        // vinkel redan okej eller ågot är fel
    }

}

void Movement::moveForward(){
    //förflyttas framåt
    //LW_A 1
    //LW_B 0

    //RW_A 1
    //RW_B 0
    float rad = angle * 3.14 / 180;
    
    if(motorSpeed > 0){
        //x += motorSpeed * cos(rad); //tror inte cos och sin fungerar
        //y += motorSpeed * sin(rad);
    }else{
        //standby();
    }
}

void Movement::moveBackward(){
    //förflyttas backåt
    //LW_A 0
    //LW_B 1

    //RW_A 0
    //RW_B 1
    //float rad = angle * 3.14 / 180;
    
    if(motorSpeed > 0){
        //x -= motorSpeed * cos(rad); //tror inte cos och sin fungerar
        //y -= motorSpeed * sin(rad);
    }else{
        //standby();
    }
}

void Movement::standby(){
    //står stilla
    //LW_A 0
    //LW_B 0
    //RW_A 0
    //RW_B 0
    //motorSpeed = 0;
    
}

void Movement::getCoordinate() {
    //Serial.print("X: "); 
    //Serial.print(x);
    //Serial.print(" Y: "); 
    //Serial.print(y);
    //Serial.print(" Angle: "); 
    //Serial.println(angle);
}