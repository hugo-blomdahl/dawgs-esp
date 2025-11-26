#pragma once


class Navigation{

    private:

        static const int MAX_ROUTE_LEN = 16;      // max antal stopp i en rutt
        static const int MAX_QR_ID_LEN = 16;      // max längd på ett QR-id typ "monaLisa"
        int  routeLength;      
        int  currentTargetIndex;  

        char routeIDs[MAX_ROUTE_LEN][MAX_QR_ID_LEN];
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
    void getCoordinates(float coords[2]) const; // förväntar oss en array med 2 floats (x & y kordinater )
    float getHeadingDeg() const;  // för logg/UI

    void updateRoute(const char* route[], int length);



};