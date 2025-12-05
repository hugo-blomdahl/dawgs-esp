#include "navigation.h"
#include <string.h>   
#include <cmath>  
#include <iostream>

Navigation::Navigation()
: routeLength(0),
  currentTargetIndex(0),
  headingRad(0.0f)
{
    coordinates[0] = 0.0f;
    coordinates[1] = 0.0f;

    for (int i = 0; i < MAX_ROUTE_LEN; ++i) {
        routeIDs[i].clear();
    }

    initGyroCalibration();
}

void Navigation::initGyroCalibration(){}

void Navigation::calibrateFromQR(const std::string& qrId){ //strukturera om så att korrigering sker även om oväntat QR avläses
    
    if (qrId.empty()) return;
    
    if (qrId == routeIDs[currentTargetIndex]) {

       
        currentTargetIndex++; //rätt QR-kod hittad, vidare till nästa

        // TODO: Själva korrigeringen
        
    } else {
        // oväntat QR avläst, kanske loggas? 
        std::cout << qrId << std::endl;
    } 

}

void Navigation::readGyro(){}

void Navigation::readEncoders(){}

void Navigation::readAccelerometer(){}

void Navigation::getCoordinates(float coords[2]) const{
        coords[0] = coordinates[0];
        coords[1] = coordinates[1];
}

float Navigation::getHeadingDeg() const {
    return headingRad * 180.0f / static_cast<float>(M_PI);
}

void Navigation::updateRoute(const std::string route[], int length) {
    routeLength = 0;
    currentTargetIndex = 0;

    if (length <= 0) return;

    int maxLen = (length < MAX_ROUTE_LEN) ? length : MAX_ROUTE_LEN;

    for (int i = 0; i < maxLen; ++i) {
        routeIDs[i] = route[i]; 
        routeLength++;
    }
}


