#include "activityPlanner.h"




ActivityPlanner::ActivityPlanner(){
    if(true) state = idle;

    switch (state){
        case chargeBattery:

            break;
        case alarm:

            break;
        case assignNewLeader:

            break;
        case createAndAssignRoute:

            break;
        case routeComplete:

            break;
        case performRoute:

            break;
        case idle:

            break;
    }
}


std::vector<ActivityPlanner::point> ActivityPlanner::createRoute(){
    std::vector<point> assignedRoute;
    assignedRoute.push_back(point());

    return assignedRoute;
}

void ActivityPlanner::assignRoute(uint8_t* address, std::vector<point>* route){

}

void ActivityPlanner::sendLog(std::string* log){

}