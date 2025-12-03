#include "activityPlanner.hpp"
#include "energy.hpp"
#include "communication.hpp"

ActivityPlanner::States state = ActivityPlanner::States::idle;
Comms* communication;
Energy* energy;

bool isLeader = false;
bool isThereLeader = false;
uint8_t leaderMACaddress;
bool performingRoute = false; // changes when route is done

// state alarm
bool alarmed = false;


// start up / assingNewLeader


// state createAndAssignRoute
std::list<uint8_t> routeRequests;

std::vector<ActivityPlanner::point> route;
std::vector<ActivityPlanner::nodeFriend> nodeFriends;
std::list<ActivityPlanner::message> messages; 


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
    if(energy->read().percentage < 10.0) state = chargeBattery;
    else if(alarmed) state = alarm;
    else if(!isThereLeader) state = assignNewLeader;
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

            communication->broadcastMsg("NL"); 


            break;
        case createAndAssignRoute: { //the node is leader which creates and assignsroutes
            // ToDo
            std::vector<ActivityPlanner::point> newRoute = createRoute();
            uint8_t MACaddress = routeRequests.back();
            routeRequests.pop_back();
            assignRoute(&MACaddress,newRoute);

            } break;
        case routeComplete: {//the node is finished with a route and requests a new route
            // ToDo
            std::string message = "RC";
            communication->sendMsg(&leaderMACaddress,message);
            
            } break;
        case performRoute:  //the node follows a route and performs its routine tasks
            // ToDo
            // kolla med alva

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

void ActivityPlanner::assignRoute(uint8_t* address, std::vector<point> route){
    std::string newRoute = "";
    while(!route.empty()){
        newRoute += route.back();
        route.pop_back();
    }
    communication->sendMsg(&address,newRoute);
}

void ActivityPlanner::sendLog(std::string log){

}

void processMsg(){
    while(!messages.empty()){
        ActivityPlanner::message message = messages.front();    // pops the first message from the list
        std::string messageStr = message.message;               // saves the message data
        uint8_t MACaddress = message.MACaddress;                // saves the receiver data
        messages.pop_front();                                   // removes the first message from the list

        char delimiter = ';';                                   
        std::string messageType = messageStr.substr(0,messageType.find(delimiter)); // gets the message type

        if(messageType == "NN"){ // New Node
            // ToDo: check if any other friend nodes have the same mac address
           
            ActivityPlanner::nodeFriend newNode = {MACaddress, false};
            nodeFriends.push_back(newNode);
        } else if(messageType == "RC"){ // Route Complete
            if(isLeader) routeRequests.push_back(MACaddress);
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