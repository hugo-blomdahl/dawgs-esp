#pragma once


class Navigation{

    private:
        static float coordinates[];
        void initGyroCalibration();
        void calibrateFromQR();
        void readGyro();
        void readEncoders();
        void readAccelerometer(); 
        
        // kanske ha en getConfidanceValue(); ??




    public:
    Navigation();
    void getCoordinates(float coords[]); // förväntar oss en array med 2 floats (x & y kordinater )



};