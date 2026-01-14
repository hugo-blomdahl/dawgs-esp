#ifndef ENERGY_HPP // Include guard to prevent multiple inclusions
#define ENERGY_HPP

#include <string>
#include <vector>

/** 
 * This is header file creates a class for battery simulation and management.
 * Simulates a battery that can be in active mode or low power mode
 * Supports charging and discharging with "realistic" time calculations.
 * */

class Energy {
public:
    // Enum for battery modes
    enum class BatteryMode {
        ACTIVE,
        LOW_POWER,
        CHARGING
    };

    // Struct to hold battery status information
    struct BatteryStatus { 
        float voltage;
        float percentage;
        float timeRemaining;
        BatteryMode currentMode;
        bool needsCharging;
        std::string modeString;
    };

    // Struct for roun information 
    struct RoundInfo {
        int roundNbr;
        std::string roundColor;
        float activeMinutes;
        float lowPowerMinutes;
        float requiredPercentageActive;
        float requiredPercentageLowPower;
    };

    // Constructor and methods
    Energy();                   // Constructor
    void begin();               // Initialize ADC settings
    BatteryStatus read();       // Read battery status

    // Methods for battery management
    void setMode(BatteryMode mode);                 // Set the current battery mode 
    void startCharging();                           // Start charging the battery   
    void stopCharging();                            // Stop charging the battery        
    void update(float minutes);                     // Update battery status based on elapsed time
    float getRemainingMinutes(BatteryMode mode);    // Get remaining minutes in the specified mode
    std::vector<RoundInfo> getAvailableRounds();    // Get information about available rounds
    
    // Internal helper methods
    private:
        float simulateVoltage();                // Simulate voltage reading
        void updateRemainingTime();             // Update remaining time based on current mode
        void checkChargingWarning();            // Check if battery needs charging

    // Member variables
    private:
        BatteryMode currentMode;                // Current battery mode
        float batteryPercentage;                // Current battery percentage (0-100)
        float baseConsumptionRate;              // Base consumption rate per minute
        float chargingRate;                     // Charging rate per minute
        bool isCharging;                        // Flag indicating if the battery is charging
        uint64_t lastUpdate;                    // Timestamp of last update (milliseconds)
        static constexpr float ACTIVE_LIFETIME = 360.0f;            // 6 hours in minutes
        static constexpr float LOW_POWER_LIFETIME = 720.0f;         // 12 hours in minutes
        static constexpr float CHARGE_TIME = 180.0f;                // 3 hours in minutes
        static constexpr float WARNING_THRESHOLD = 40.0f;           // Warning threshold percentage
};

#endif // ENERGY_HPP