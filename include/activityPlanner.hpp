#ifndef ACTIVITYPLANNER_HPP // Include guard to prevent multiple inclusions
#define ACTIVITYPLANNER_HPP

#include <string>
#include <vector>
#include <list>
#include <esp_timer.h>

class ActivityPlanner {
    public: 

    ActivityPlanner();               //constructor                                       
    void state_machine_();           //the stace machine         
    void sendLog(std::string log);   //function to send logs, is this correct parameter format for strings  

    enum States {
        chargeBattery,              //go to charging station and charge
        alarm,                      //the node alarms to a server
        assignNewLeader,            //LastResortCom, when new leader needs to be assigned 
        createAndAssignRoute,       //the node is leader which creates and assignsroutes
        routeComplete,              //the node is finished with a route and requests a new route
        performRoute,               //the node follows a route and performs its routine tasks
        idle,                       //either idle or is somehow broken or is lost
    };

    struct nodeFriend{
        uint8_t macAddress; 
        float percentage; 
        bool isLeader;
    };

    struct message {
        uint8_t MACaddress;
        std::string message;
    };

    private:
    void processMsg();
    void createRoute(std::string* route[]);                       //function to create route, what kind of return variable?????
    void assignRoute(uint8_t* address, std::string route[]);      //function to assign route, check how the address works  

};

#endif