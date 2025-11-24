#include "energy.hpp"
#include <stdint.h>
/** This implementation file contains the definitions of the methods declared in Energy.hpp. 
 * The begin() method configures the ADC (Analog-to-Digital Converter) settings necessary for reading the battery voltage. 
 * The readVoltage() method retrieves the raw ADC value and converts it to a voltage reading. 
 * The read() method calculates the battery's capacity percentage and estimates the active and standby times based on the battery's characteristics.
 */


Energy::Energy() {} // Constructor for the Energy class

void Energy::begin() { // Method to initialize ADC settings
}

Energy::BatteryStatus Energy::read() { // Method to read battery status
    BatteryStatus batteryLevel;
    batteryLevel.voltage = readVoltage();
    return batteryLevel;
}

float readVoltage() { // Method to read voltage
    return 0.0f;
}