#pragma once
#include <string>
#include "esp_log.h"
#include <vector>


struct SimpleNode {
    float x;
    float y;
    std::string qrId;
};


class Navigation {

    private:

        static const int MAX_ROUTE_LEN = 16;      // max antal stopp i en rutt
        int  routeLength;      
        int  currentTargetIndex;
        std::string lastId = "-1";
        static const char* TAG_NAVI;

        std::string routeIDs[MAX_ROUTE_LEN];
        float coordinates[2];
        float headingRad;
        
    
        std::vector<SimpleNode> mapNodes;
        
    
        std::string lastId; 

        void updateRouteProgress(const std::string& qrId);
        
        
        

    public:
        Navigation();

        bool loadEmbeddedMap();
        
        bool loadMapFromJson(const std::string& jsonString);

        void calibrateFromQR(int qrIndex); 
        
        float getHeadingDeg() const;  
        void getCoordinates(float coords[2]) const;
        void updateRoute(const std::string route[], int length);
};