#include "activityPlanner.hpp"
#include "energy.hpp"
#include "communication.hpp"

ActivityPlanner::States state = ActivityPlanner::States::idle;
Comms* communication;
Energy* energy;

bool isLeader = false;
bool isThereALeader = false;
uint8_t leaderMACaddress;
bool performingRoute = false; // changes when route is done

// state alarm
bool alarmed = false;

// start up / assingNewLeader
bool isStartUp = true;

// state createAndAssignRoute
std::list<uint8_t> routeRequests;
std::list<ActivityPlanner::message> messages;
std::vector<ActivityPlanner::nodeFriend> nodeFriends;

ActivityPlanner::ActivityPlanner(){
    communication = new Comms();
    communication->initCommunication();

    energy = new Energy();
    energy->begin();
}
void ActivityPlanner::state_machine_(){
    processMsg();
    
    //update data
    
    // ToDo: check detection

    //if-statements to go into state
    if(false) state = chargeBattery;
    else if(alarmed) state = alarm;
    else if(!isThereALeader || isStartUp) state = assignNewLeader;
    else if(!routeRequests.empty()) state = createAndAssignRoute;
    else if(!performingRoute) state = routeComplete;
    else if(performingRoute) state = performRoute;
    else state = idle;

    switch (state){
        case chargeBattery: //go to charging station and charge
            // ToDo
            // find charging station point, travel there and charge
            
            break;
        case alarm: //the node alarms to a server
            // ToDo
            // send to server and broadcast

            break;
        case assignNewLeader:   //LastResortCom, when new leader needs to be assigned 
            // ToDo
            // if start up: find the mac address with lowest value and set leader
            // else: broadcast battery level to all other nodes: check battery level, if same with 2 or more check again with MAC-address

            break;
        case createAndAssignRoute: { //the node is leader which creates and assignsroutes
            // ToDo

            } break;
        case routeComplete: {//the node is finished with a route and requests a new route
            // ToDo
            
            } break;
        case performRoute:  //the node follows a route and performs its routine tasks
            // ToDo
            // kolla med alva

            break;
        case idle:  //either idle or is somehow broken or is lost

            break;
    }
}


void ActivityPlanner::createRoute(std::string* route[]){
    
}

void ActivityPlanner::assignRoute(uint8_t* address, std::string* route[]){

}

void ActivityPlanner::sendLog(std::string log){

}

void processMsg(){
    while(!messages.empty()){
        ActivityPlanner::message message = messages.front();    // pops the first message from the list
        std::string messageStr = message.message;               // saves the message data
        uint8_t senderMacAddress = message.MACaddress;                // saves the receiver data
        messages.pop_front();                                   // removes the first message from the list

        std::printf(messageStr.c_str());
        char delimiter = ';';                                   
        std::string messageType = messageStr.substr(0,messageType.find(delimiter)); // gets the message type

        if(messageType == "NN"){ // New Node
            
        } else if(messageType == "RC"){ // Route Complete

        } else if(messageType == "NR"){ // New Route
            
        } else if(messageType == "NL"){ // No Leader
            
        } else if(messageType == "BP"){ // Battery Percentage received
            
        } else if(messageType == "PL"){ // Proposal to be Leader
            
        } else if(messageType == "AR"){ // Accept new Leader (proposed leader receives)

        } else if(messageType == "RL"){ // Reject new Leader (proposed leader receives)
            
        } else if(messageType == "NL") { // New Leader

        }
    }
}