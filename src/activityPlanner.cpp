#include "activityPlanner.hpp"
#include "energy.hpp"

ActivityPlanner::States state = ActivityPlanner::States::idle;

bool isLeader = false;
bool leaderExists = false;

std::vector<ActivityPlanner::point> route;
std::vector<ActivityPlanner::node> nodeFriends;
std::list<std::string> messages; 


ActivityPlanner::ActivityPlanner(){
    
}

void ActivityPlanner::state_machine_(){
    processMsg();
    

    switch (state){
        case chargeBattery: //go to charging station and charge
            
            break;
        case alarm: //the node alarms to a server

            break;
        case assignNewLeader:   //LastResortCom, when new leader needs to be assigned 

            break;
        case createAndAssignRoute:  //the node is leader which creates and assignsroutes

            break;
        case routeComplete: //the node is finished with a route and requests a new route

            break;
        case performRoute:  //the node follows a route and performs its routine tasks

            break;
        case idle:  //either idle or is somehow broken or is lost

            break;
    }
}


std::vector<ActivityPlanner::point> ActivityPlanner::createRoute(){
    std::vector<point> assignedRoute;

    point newPoint = {3,3,false,true};
    assignedRoute.push_back(newPoint);

    return assignedRoute;
}

void ActivityPlanner::assignRoute(uint8_t* address, std::vector<point>* route){

}

void ActivityPlanner::sendLog(std::string log){

}

void processMsg(){
    while(!messages.empty()){
        std::string message = messages.front(); 
        messages.pop_front();
        char delimiter = ';';
        std::string messageType = message.substr(0,messageType.find(delimiter));

        if(messageType == "newNode"){
            std::string macAddressStr = message.substr(1,messageType.find(delimiter));
            uint8_t macAddress = stoi(macAddressStr);

            ActivityPlanner::node newNode = {macAddress, false};
            nodeFriends.push_back(newNode);
        }
    }
}