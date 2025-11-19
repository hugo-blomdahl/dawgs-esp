#include "movement.h"
#include <stdint.h>

//temp pin number behållare för hjulen
#define LEFT_WHEEL_A 18 //clockvis
#define LEFT_WHEEL_B 19 //anti-clockvis
#define RIGHT_WHEEL_A 21 //clockvis
#define RIGHT_WHEEL_B 22 //anti-clockvis

//temp pin number behållare för PWM
#define LEFT_PWM  5
#define RIGHT_PWM 6

uint8_t motorSpeed = 0;

void setSpeed(int speed){
    //sätter hastigheten
    //motorSpeed = speed; //?
}
void rotate(int wheelRotation){
    //roterar vänster eller höger hjul beroende på parameter
        if(wheelRotation > 0){
        //roterar till höger
        //LW_A 1
        //LW_B 0
        //L_PMW motorSpeed

        //RW_A 0
        //RW_B 1
        //R_PMW motorSpeed

    }else if(wheelRotation < 0){
        //roterar till vänster
        //LW_A 0
        //LW_B 1
        //L_PMW motorSpeed

        //RW_A 1
        //RW_B 0
        //R_PMW motorSpeed

    }else{
        //gö inget
        //standby();
    }
}
void calculateRotation(){
    //beräknar rotationen på hjulen?
    //vanlig DC-motor med två kablar, ingen encoder?
}
void moveForward(){
    //förflyttas framåt
    //LW_A 1
    //LW_B 0
    //L_PMW motorSpeed

    //RW_A 1
    //RW_B 0
    //R_PMW motorSpeed

}
void moveBackward(){
    //förflyttas backåt
    //LW_A 0
    //LW_B 1
    //L_PMW motorSpeed

    //RW_A 0
    //RW_B 1
    //R_PMW motorSpeed

}
void standby(){
    //står stilla
    //LW_A 0
    //LW_B 0
    //RW_A 0
    //RW_B 0
    
}