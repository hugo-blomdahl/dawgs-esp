#pragma once
#include <string>


class Navigation{

    private:

        static const int MAX_ROUTE_LEN = 16;      // max antal stopp i en rutt
        int  routeLength;      
        int  currentTargetIndex;  

        std::string routeIDs[MAX_ROUTE_LEN];
        float coordinates[2];
        float headingRad;
        
        
        void initGyroCalibration();
        void calibrateFromQR(const char* qrId);
        void readGyro();
        void readEncoders();
        void readAccelerometer(); 
        

        // void fusionPos(?, time interval)  // sensor fusion mellan gyro + enkodrar 
        // kanske ha en getConfidenceValue(); // updateConfidence ??

        // void setRout()
        




    public:
    Navigation();
    void calibrateFromQR(const std::string& qrId); 
    float getHeadingDeg() const;  // för logg/UI
    void getCoordinates(float coords[2]) const;
    void updateRoute(const std::string route[], int length);



};