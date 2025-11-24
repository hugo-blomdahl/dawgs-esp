#ifndef ENERGY_HPP // Include guard to prevent multiple inclusions
#define ENERGY_HPP

/** This header file defines the Energy class, which encapsulates the functionality related to battery monitoring. 
 * It includes a nested Status struct to hold the battery's voltage, capacity percentage, and time metrics. 
 * The class methods include a constructor, begin() for initialization, and read() for obtaining the battery status. 
 * */

class Energy {
public:
    
    Energy(); // Constructor declaration
    void begin(); // Method to initialize the battery monitoring
    Status read(); // Method to read the battery status

private:

    float readVoltage(); // Method to read the current voltage of the battery

    struct BatteryStatus { // Struct to hold battery status information
    float voltage;
    float percentage;
    float timeRemaining;
    };
};

#endif // ENERGY_HPP