#include "activityPlanner.hpp"


bool performingRoute = false; // changes when route is done

// state alarm
bool alarmed = false;

// start up / assingNewLeader
bool isStartUp = true;

// state createAndAssignRoute
std::list<uint8_t*> routeRequests;
std::list<Message> messages;
std::vector<ActivityPlanner::nodeFriend> nodeFriends;


ActivityPlanner::ActivityPlanner(Comms* aComms, TcpClient* aLoggClient, TcpClient* aVisualClient){
    loggClient = aLoggClient;
    visualClient = aVisualClient;
    communication = aComms;
    communication->setMessageList(&messages);
    energy = new Energy();
    energy->begin();

    state = idle;
    isLeader = false;
    isThereALeader = false;
    performingRoute = false;
    alarmed = false;
    isStartUp = true;
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
    loggClient->sendString(log);
}

void ActivityPlanner::sendVisual(std::string log){
    visualClient->sendString(log);
}


void ActivityPlanner::processMsg(){
    std::lock_guard<std::mutex> lock(communication->getMutex());
    while(!messages.empty()){
        
        Message message = messages.front();    // pops the first message from the list
        std::string messageStr = message.message;               // saves the message data

        char delimiter = ';';                                   
        //this is wrong? cant fall find on yourself std::string messageType = messageStr.substr(0,messageType.find(delimiter)); // gets the message type
        size_t delimiterPos = messageStr.find(delimiter);

        std::string messageType;
        if(delimiterPos != std::string::npos){
            messageType = messageStr.substr(0, delimiterPos);
        } else {
            messageType = messageStr; //if not found just put whole message
        }
        if(messageType == "NN"){ // New Node
            
        } else if(messageType == "RC"){ // Route Complete

        } else if(messageType == "NR"){ // New Route
            
        } else if(messageType == "NL"){ // No Leader
            
        } else if(messageType == "BP"){ // Battery Percentage received
            
        } else if(messageType == "PL"){ // Proposal to be Leader
            
        } else if(messageType == "AR"){ // Accept new Leader (proposed leader receives)

        } else if(messageType == "RL"){ // Reject new Leader (proposed leader receives)
            
        } else if(messageType == "NL"){ // New Leader

        }
        std::cout << "Processed message: " << messageStr << std::endl;
        messages.pop_front();           // removes the first message from the list
    }
}