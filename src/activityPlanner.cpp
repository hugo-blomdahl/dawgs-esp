#include "activityPlanner.hpp"



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
    
    performingRoute = false;
    alarmed = false;

    isStartUp = true;
    broadcastedMAC = false;

    isLeader = false;
    isThereALeader = false;
    proposeToBeLeader = false;
    propose = true;

    timeAtStartUp = esp_timer_get_time();
}
void ActivityPlanner::state_machine_(){
    processMsg();
    timeSinceStartUp = esp_timer_get_time() - timeAtStartUp;
    //if(isLeader) std::cout << "isLeader: " << std::endl;

    std::cout << "size: " << nodeFriends.size() << ", isStartUp: " << isStartUp << ", broadcastedMAC: " << broadcastedMAC << ", propose: " << propose << ", proposeL: " << proposeToBeLeader <<std::endl;
    
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
        case chargeBattery: // 0, go to charging station and charge
            // ToDo
            // find charging station point, travel there and charge
            
            break;
        case alarm: // 1, the node alarms to a server
            // ToDo
            // send to server and broadcast

            break;
        case assignNewLeader:   // 2, LastResortCom, when new leader needs to be assigned 
             // ToDo
            // if start up: find the mac address with lowest value and set leader
            // else: broadcast battery level to all other nodes: check battery level, if same with 2 or more check again with MAC-address
            if(isStartUp && !broadcastedMAC && timeSinceStartUp >= 10*1000*1000){
                communication->broadcastMsg("NN");
                broadcastedMAC = true;
            }if(isStartUp && broadcastedMAC && propose && timeSinceStartUp >= 20*1000*1000){
                
                propose = true; // Antag att vi är ledare

                for(auto& node : nodeFriends){
                    // memcmp returnerar > 0 om selfAddress är större än node.macAddress
                    if(memcmp(communication->selfAddress, node.macAddress, 6) > 0){
                        // Vi hittade någon som är mindre än oss. Vi är inte ledare.
                        propose = false;
                        break; // Avbryt direkt, vi behöver inte kolla fler
                    }
                }

                if(propose) {
                    communication->broadcastMsg("PL");
                    proposeToBeLeader = true;
                }
                propose = false; // Förhindra att vi kör detta block igen
            }if(isStartUp && proposeToBeLeader && timeSinceStartUp >= 30*1000*1000 ){
                bool breakOut = false;
                for(auto node : nodeFriends){
                    std::cout << "accept: " << node.accept <<std::endl;
                    if(!node.accept){
                        breakOut = true;
                        break;
                    }
                }
                std::cout << "breakOut: " << breakOut<<std::endl;
                if(!breakOut) {
                    isLeader = true;
                    proposeToBeLeader = false;
                    isStartUp = false;
                    communication->broadcastMsg("CL");
                }
            }

            break;
        case createAndAssignRoute: { // 3, the node is leader which creates and assignsroutes
            // ToDo

            } break;
        case routeComplete: {// 4, the node is finished with a route and requests a new route
            // ToDo
            
            } break;
        case performRoute:  // 5, the node follows a route and performs its routine tasks
            // ToDo
            // kolla med alva

            break;
        case idle:  //6, either idle or is somehow broken or is lost

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
        messages.pop_front();           // removes the first message from the list

        std::cout << "Processed message: " << messageStr << std::endl;

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
            for(auto node : nodeFriends){
                if(std::ranges::equal(node.macAddress,message.macAddress)){
                    return;
                }
            }

            ActivityPlanner::nodeFriend tempNode = ActivityPlanner::nodeFriend();
            memcpy(tempNode.macAddress, message.macAddress, 6);
            tempNode.isLeader = false;

            nodeFriends.push_back(tempNode);
            if(isLeader) communication->sendMsg(message.macAddress,"NNRI:true");
            else communication->sendMsg(message.macAddress,"NNRI;false");

            std::cout << "New Node" << std::endl;
        } else if(messageType == "NNRI"){ // New node Receives return node Information
            // NNRI;isLeader
            for(auto node : nodeFriends){
                if(std::ranges::equal(node.macAddress,message.macAddress)){
                    return;
                }
            }

            ActivityPlanner::nodeFriend tempNode = ActivityPlanner::nodeFriend();
            memcpy(tempNode.macAddress, message.macAddress, 6);
            if(messageStr.substr(1,delimiterPos) == "false") {
                tempNode.isLeader = false;
            }
            else if (messageStr.substr(1,delimiterPos) == "true") {
                tempNode.isLeader = true;
                isStartUp = false;
            }

            nodeFriends.push_back(tempNode);

            std::cout << "New Node Received" << std::endl;
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
            
        } else if(messageType == "BP"){ // Battery Percentage received
            
        } else if(messageType == "PL"){ // node receives a Proposal to be Leader from another (receiver recives proposal, can accept or reject)
            if(isStartUp){
                bool breakOut = false;
                communication->sendMsg(message.macAddress,"AL");
                /*
                for(int i = 0; i < 6; i++){
                    for(auto node : nodeFriends){
                        if(communication->selfAddress[i] < node.macAddress[i]) {
                            breakOut = true;
                            break;
                        }
                        if(breakOut) break;
                    }
                    if(breakOut) break;
                }*/
            } else{
                bool breakOut = false;
                int proposedBattery = 0;
                int battery = trunc(energy->read().percentage);
                for(auto& node : nodeFriends){
                    if(std::ranges::equal(node.macAddress,message.macAddress)) {   
                        proposedBattery = node.percentage;
                        break;
                    }
                    if(breakOut) break;
                }
                if(proposedBattery > battery) {
                    communication->sendMsg(message.macAddress,"AL");
                }else {
                    communication->sendMsg(message.macAddress,"RL");
                }
            }

        } else if(messageType == "AL"){ // node who proposed to be Leader recives Accept
            for(auto& node : nodeFriends){
                if(std::ranges::equal(node.macAddress,message.macAddress)){
                    node.accept = true;
                    std::cout << "AL" << std::endl;
                    break;
                }
            }
        } else if(messageType == "RL"){ // node who proposed to be Leader recives Reject
            for(auto& node : nodeFriends){
                if(std::ranges::equal(node.macAddress,message.macAddress)){
                    node.accept = false;
                    std::cout << "RL" << std::endl;
                    break;
                }
            }
        } else if(messageType == "CL"){ // node recives info about Current (New) Leader
            for(auto& node : nodeFriends){
                if(std::ranges::equal(node.macAddress,message.macAddress)) node.isLeader = true;
                else node.isLeader = false;
            }
            memcpy(leaderMACaddress,message.macAddress,6);
            isThereALeader = true;
            isStartUp = false;
        }
    }
    
}