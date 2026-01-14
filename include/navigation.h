#pragma once
#include <string>
#include <vector>
#include "esp_log.h"
#include "cJSON.h" 
#include "movement.hpp" 

enum ActionType {
    ACTION_FORWARD,  // "F"
    ACTION_BACKWARD, // "B"
    ACTION_ROTATE    // "R"
};

struct NavAction {
    ActionType type;
    float value; // m eller grader
};

struct SimpleNode {
    int id;  
    std::string art; // För konstverksnamn
    
    std::vector<NavAction> route5Actions; 
    std::vector<NavAction> route6Actions; 
};

class Navigation {

    private:
        Movement* movementController;

        std::vector<SimpleNode> mapNodes;
        
        static const int MAX_ROUTE_CAPACITY = 100; 
        int currentRoutePath[MAX_ROUTE_CAPACITY];  
        int currentRouteLength;

        int activeRouteType;    
        int currentPathIndex;  

        std::string lastId; 

        void parsePathArray(cJSON* nodeObj, const char* arrayName, std::vector<NavAction>& targetVector);
        void executePathSequence(const std::vector<NavAction>& actions);

    public:
        Navigation(Movement* movement);

        bool loadEmbeddedMap();
        bool loadMapFromJson(const std::string& jsonString);


        std::string getNodeArt(int qrId);

    
        void calibrateFromQR(int qrIndex); 
        
    
        void setRoute(int routeType, const int* pathIndices, int length);
};