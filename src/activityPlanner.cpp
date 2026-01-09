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
    if(isStartUp) timeSinceStartUp = esp_timer_get_time() - timeAtStartUp;
    if(isLeader) printf("isLeader");
    
    //update data
    
    // ToDo: check detection

    //if-statements to go into state
    if(false) state = chargeBattery;
    else if(alarmed) state = alarm;
    else if(isStartUp) state = assignNewLeader;
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
            if(isStartUp && timeSinceStartUp > 10*1000){
                bool proposeToBeLeader = false;
                printf("isStartUp && timeSinceStartUp > 10*1000");

                //kolla igenom
                for(int i = 0; i < 6; i++){
                    for(auto node : nodeFriends){
                        if(communication->selfAddress[i] < node.macAddress[i]) {
                            proposeToBeLeader = true;
                        }
                        else {
                            proposeToBeLeader = false;
                        }
                    }
                    if(proposeToBeLeader) {
                        communication->broadcastMsg("PL");
                        isStartUp = false;
                        printf("proposed");
                        break;
                    }
                }
                printf("didn't propose");
                
            } else if(isStartUp && !broadcastedMAC){
                std::string message = "";
                if(isLeader) std::string message = "NN;true";
                else std::string message = "NN;false";
                communication->broadcastMsg(message);
                broadcastedMAC = true;
            } 
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

        if(messageType == "NN"){ // existing node to register New Node
            // NN;isLeader
            bool breakOut = false;
            for(auto node : nodeFriends){
                if(node.macAddress == message.macAddress) {
                    breakOut = true;
                    break;
                }
            }
            if(breakOut) break;

            ActivityPlanner::nodeFriend tempNode = ActivityPlanner::nodeFriend();
            memcpy(tempNode.macAddress, message.macAddress, 6);
            if(messageStr.substr(1,delimiterPos) == "false") tempNode.isLeader = false;
            else if (messageStr.substr(1,delimiterPos) == "true") tempNode.isLeader = true;

            nodeFriends.push_back(tempNode);
            if(isLeader) communication->sendMsg(message.macAddress,"NNRI:true");
            else communication->sendMsg(message.macAddress,"NNRI;false");
        } else if(messageType == "NNRI"){ // New node Receives return node Information
            // NNRI;isLeader
            bool breakOut = false;
            for(auto node : nodeFriends){
                if(node.macAddress == message.macAddress) {
                    breakOut = true;
                    break;
                }
            }
            if(breakOut) break;

            ActivityPlanner::nodeFriend tempNode = ActivityPlanner::nodeFriend();
            memcpy(tempNode.macAddress, message.macAddress, 6);
            if(messageStr.substr(1,delimiterPos) == "false") tempNode.isLeader = false;
            else if (messageStr.substr(1,delimiterPos) == "true") tempNode.isLeader = true;

            nodeFriends.push_back(tempNode);
        } else if(messageType == "RC"){ // Route Complete
            if(isLeader){
                uint8_t *address = 0;
                memcpy(address, message.macAddress, 6);
                routeRequests.push_back(address);
            }
        } else if(messageType == "NR"){ // New Route
            std::vector<std::string> route;
            //iterate throug message and save in vector
            //send to navigation.cpp
            
        } else if(messageType == "NL"){ // No Leader
            isThereALeader = false;
            int batteryLevel = trunc(energy->read().percentage);
            std::string battery = "BP;" + batteryLevel;
            communication->broadcastMsg(battery);
        } else if(messageType == "BP"){ // Battery Percentage received
            for(auto node : nodeFriends){
                if(node.macAddress == message.macAddress) {
                    float battery = std::stof(messageStr.substr(1,delimiterPos));
                    node.percentage = battery;
                    break;
                }
            }
        } else if(messageType == "PL"){ // node receives a Proposal to be Leader from another (receiver recives proposal, can accept or reject)
            int proposedBattery = 0;
            int battery = trunc(energy->read().percentage);
            for(auto node : nodeFriends){
                if(node.macAddress == message.macAddress) {
                    proposedBattery = node.percentage;
                    break;
                }
            }
            if(proposedBattery > battery) {
                communication->sendMsg(message.macAddress,"AL");
            }else {
                communication->sendMsg(message.macAddress,"RL");
            }

        } else if(messageType == "AL"){ // node who proposed to be Leader recives Accept
            for(auto node : nodeFriends){
                if(node.macAddress == message.macAddress) {
                    node.accept = true;
                    break;
                }
            }
        } else if(messageType == "RL"){ // node who proposed to be Leader recives Reject
            for(auto node : nodeFriends){
                if(node.macAddress == message.macAddress) {
                    node.accept = false;
                    break;
                }
            }
        } else if(messageType == "CL"){ // node recives info about Current (New) Leader
            for(auto node : nodeFriends){
                if(node.macAddress == message.macAddress) node.isLeader = true;
                else node.isLeader = false;
            }
            memcpy(leaderMACaddress,message.macAddress,6);
            isThereALeader = true;
            isStartUp = false;
        }
        std::cout << "Processed message: " << messageStr << std::endl;
        messages.pop_front();           // removes the first message from the list
    }
}