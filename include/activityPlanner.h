#ifndef ACTIVITYPLANNER_HPP // Include guard to prevent multiple inclusions
#define ACTIVITYPLANNER_HPP

#include <string>
#include <vector>

class ActivityPlanner {
    public: 

    ActivityPlanner();                                                  //the stace machine
    void sendLog(std::string log);                                     //function to send logs, is this correct parameter format for strings  

    private:
    struct point {
        int x;              //x coordinate
        int y;              //y coordinate
        bool checked;       //have the node been here
        bool isThere;       //is the artwork still there
    };

    std::vector<point> route;

    enum States {
        chargeBattery,              //go to charging station and charge
        alarm,                      //the node alarms to a server
        assignNewLeader,            //LastResortCom, when new leader needs to be assigned 
        createAndAssignRoute,       //the node is leader which creates and assignsroutes
        routeComplete,              //the node is finished with a route and requests a new route
        performRoute,               //the node follows a route and performs its routine tasks
        idle                        //either idle or is somehow broken or is lost
    };

    States state = idle;

    std::vector<point> createRoute();                                   //function to create route, what kind of return variable?????
    void assignRoute(uint8_t* address, std::vector<point>* route);      //function to assign route, check how the address works  

};

#endif