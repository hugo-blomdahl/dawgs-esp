#include "activityPlanner.h"

ActivityPlanner::States state = ActivityPlanner::States::idle;


ActivityPlanner::ActivityPlanner(){
    

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