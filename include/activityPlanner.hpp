#ifndef ACTIVITYPLANNER_HPP // Include guard to prevent multiple inclusions
#define ACTIVITYPLANNER_HPP

#include <string>
#include <vector>
#include <list>
#include <cmath>
#include <esp_timer.h>
#include "communication.hpp"
#include "energy.hpp"
#include "tcpClient.hpp"

class ActivityPlanner {
    public: 

    ActivityPlanner(Comms* aComms, TcpClient* aLoggClient, TcpClient* aVisualClient);               //constructor                                       
    void state_machine_();           //the stace machine  
    void sendLog(std::string log);                                     //function to send logs, is this correct parameter format for strings  
    void sendVisual(std::string log);

    std::list<Message> messages;

    enum States {
        chargeBattery,              //go to charging station and charge
        alarm,                      //the node alarms to a server
        assignNewLeader,            //LastResortCom, when new leader needs to be assigned 
        createAndAssignRoute,       //the node is leader which creates and assignsroutes
        routeComplete,              //the node is finished with a route and requests a new route
        performRoute,               //the node follows a route and performs its routine tasks
        idle                        //either idle or is somehow broken or is lost
    };

    struct nodeFriend{
        uint8_t macAddress[6]; 
        float percentage; 
        bool isLeader;
        bool accept;
    };

    private:

    TcpClient* visualClient;
    TcpClient* loggClient;


    Comms* communication;
    Energy* energy;
    bool isLeader;
    bool isThereALeader;
    bool proposeToBeLeader;
    bool propose;
    bool broadcastedMAC;
    uint8_t leaderMACaddress[6];
    bool performingRoute; // changes when route is done
    States state;

    // state alarm
    bool alarmed;

    // start up / assingNewLeader
    bool isStartUp;
    long timeSinceStartUp = 0;
    long timeAtStartUp = 0;

    // state createAndAssignRoute
    std::list<uint8_t*> routeRequests;
    std::vector<nodeFriend> nodeFriends;

    void processMsg();
    //change array to vector. use int?
    void createRoute(std::string* route[]);                        //function to create route, what kind of return variable?????
    void assignRoute(uint8_t* address, std::string* route[]);      //function to assign route, check how the address works  
};

#endif