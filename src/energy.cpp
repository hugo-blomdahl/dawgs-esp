#include "energy.hpp"
#include <stdint.h>
#include <cmath>
#include <ctime>
#include <esp_log.h>
#include <algorithm>
#include <sys/time.h>

static const char* TAG = "Energy";

// Constructor - initializes default values
Energy::Energy()
    : currentMode(BatteryMode::ACTIVE),
      batteryPercentage(100.0f),
      baseConsumptionRate(1.0f / (ACTIVE_LIFETIME / 100.0f)), // Correct consumption rate per minute
      chargingRate(100.0f / CHARGE_TIME), // Charging rate per minute (100% / 180 minutes)
      isCharging(false)
{
    // Initialize lastUpdate with current time in milliseconds
    struct timeval tv;
    gettimeofday(&tv, NULL);
    lastUpdate = tv.tv_sec * 1000 + tv.tv_usec / 1000; // Store as milliseconds

    // Calculate base consumption: 100% / 360 minutes = 0.2778% per minute in active mode
    // For low power mode: 100% / 720 minutes = 0.1389% per minute
}

// Initialize ADC settings - runs at system startup
void Energy::begin() {
    ESP_LOGI(TAG, "Energy system initialized.");
    ESP_LOGI(TAG, "Starting battery percentage: %.2f%%", (double)batteryPercentage);
    ESP_LOGI(TAG, "Active mode: %.2f minutes remaining at 100 %% at start.", (double)ACTIVE_LIFETIME);
    ESP_LOGI(TAG, "Low power mode: %.2f minutes remaining at 100 %% at start.", (double)LOW_POWER_LIFETIME);
    ESP_LOGI(TAG, "Charging time: %.2f minutes from 0 %% to 100%%.", (double)CHARGE_TIME);
}

// Reads battery status and returns it as a struct
Energy::BatteryStatus Energy::read() {
    // Get current time in milliseconds
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t now = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    
    // Calculate elapsed time since last update
    uint64_t elapsedMs = now - lastUpdate;
    float deltaMinutes = elapsedMs / 60000.0f;  // Convert milliseconds to minutes
    
    // Only update if more than 0.1 minute (6 seconds) has passed
    if (deltaMinutes > 0.1f) {  
        update(deltaMinutes);          // Update battery status
        lastUpdate = now;              // Update timestamp
    }
    
    // Create and fill BatteryStatus struct with current information
    BatteryStatus status;
    status.voltage = simulateVoltage();                          // Simulated battery voltage
    status.percentage = batteryPercentage;                       // Current battery percentage
    status.timeRemaining = getRemainingMinutes(currentMode);     // Remaining time in current mode
    status.currentMode = currentMode;                            // Current operating mode
    status.needsCharging = (getRemainingMinutes(BatteryMode::ACTIVE) < WARNING_THRESHOLD); // Check if charging needed
    
    // Set text representation of the current mode for display purposes
    switch (currentMode) {
        case BatteryMode::ACTIVE:
            status.modeString = "Active";
            break;
        case BatteryMode::LOW_POWER:
            status.modeString = "Low Power";
            break;
        case BatteryMode::CHARGING:
            status.modeString = "Charging";
            break;
    }
    
    return status; // Return the complete battery status
}

// Sets battery mode (ACTIVE or LOW_POWER)
void Energy::setMode(BatteryMode mode) {
    // Only allow switching between ACTIVE and LOW_POWER modes
    if (mode == BatteryMode::ACTIVE || mode == BatteryMode::LOW_POWER) {
        currentMode = mode; // Update current mode
        ESP_LOGI(TAG, "Battery mode set to %s", 
                (mode == BatteryMode::ACTIVE) ? "Active" : "Low Power");
    }
}

// Starts the charging process
void Energy::startCharging() {
    isCharging = true;                // Set charging flag
    currentMode = BatteryMode::CHARGING; // Switch to charging mode
    ESP_LOGI(TAG, "Charging started at %.1f%%", (double)batteryPercentage);
}

// Stops the charging process
void Energy::stopCharging() {
    isCharging = false;               // Clear charging flag
    // Return to active mode as default when charging stops
    currentMode = BatteryMode::ACTIVE;
    ESP_LOGI(TAG, "Charging stopped at %.1f%%", (double)batteryPercentage);
}

// Updates battery status based on elapsed time
void Energy::update(float deltaTimeMinutes) {
    if (isCharging) {
        // Charging logic: battery increases over time
        float chargeAmount = chargingRate * deltaTimeMinutes; // Calculate charge amount
        batteryPercentage += chargeAmount; // Add charge to battery
        
        // Cap at maximum 100% to prevent overcharging
        if (batteryPercentage > 100.0f) {
            batteryPercentage = 100.0f;
            // Auto-stop charging when fully charged
            stopCharging();
            ESP_LOGI(TAG, "Battery fully charged!");
        }
        
        // Log charging progress for debugging
        ESP_LOGD(TAG, "Charging: +%.3f%% -> %.1f%%", (double)chargeAmount, (double)batteryPercentage);
    } else {
        // Discharging logic based on current mode
        // Calculate consumption rate: percentage per minute
        float consumptionRate = (currentMode == BatteryMode::ACTIVE) 
            ? (100.0f / ACTIVE_LIFETIME)    // 0.2778% per minute in active mode
            : (100.0f / LOW_POWER_LIFETIME); // 0.1389% per minute in low power mode
        
        float dischargeAmount = consumptionRate * deltaTimeMinutes; // Calculate discharge amount
        batteryPercentage -= dischargeAmount; // Reduce battery percentage
        
        // Cap at minimum 0% to prevent negative battery
        if (batteryPercentage < 0.0f) {
            batteryPercentage = 0.0f;
            ESP_LOGW(TAG, "Battery depleted!");
        }
        
        // Log discharging progress for debugging
        ESP_LOGD(TAG, "Discharging: -%.3f%% -> %.1f%%", (double)dischargeAmount, (double)batteryPercentage);
    }
    
    // Check if charging is needed based on warning threshold
    checkChargingWarning();
}

// Returns remaining time in minutes for specific mode
float Energy::getRemainingMinutes(BatteryMode mode) {
    // If asking for charging time
    if (mode == BatteryMode::CHARGING) {
        // Calculate time left to charge to 100%
        float percentToFull = 100.0f - batteryPercentage;
        return (percentToFull > 0) ? (percentToFull / chargingRate) : 0.0f;
    }
    
    // For discharging modes: calculate based on current percentage and consumption rate
    // Determine which lifetime to use based on requested mode
    float lifetime = (mode == BatteryMode::ACTIVE) ? ACTIVE_LIFETIME : LOW_POWER_LIFETIME;
    float minutesPerPercent = lifetime / 100.0f; // Minutes of operation per 1% battery
    return batteryPercentage * minutesPerPercent; // Total remaining minutes
}

// Returns list of rounds the robot can complete with current battery level
std::vector<Energy::RoundInfo> Energy::getAvailableRounds() {
    std::vector<RoundInfo> availableRounds; // Create empty list for available rounds
    
    // Define all rounds with their time requirements and calculated percentage requirements
    std::vector<RoundInfo> allRounds = {
        // Format: {round number, color name, active minutes, low power minutes, 
        //          required % active, required % low power}
        {1, "Green", 185.0f, 185.0f, 56.94f, 28.47f},    // 3h 5min + 20min
        {2, "Red", 150.833f, 150.833f, 41.90f, 20.95f},  // 2h 10min 50s + 20min
        {3, "Purple", 133.333f, 133.333f, 37.04f, 18.52f}, // 1h 53min 20s + 20min
        {4, "Blue", 88.333f, 88.333f, 24.54f, 12.27f},   // 1h 8min 20s + 20min
        {5, "Yellow", 141.667f, 141.667f, 39.35f, 19.68f}, // 2h 1min 40s + 20min
        {6, "Pink", 235.833f, 235.833f, 65.51f, 32.75f}  // 3h 35min 50s + 20min
    };
    
    // Calculate required percentage multiplier based on current mode
    // This converts minutes to percentage based on battery lifetime
    float requiredPercentMultiplier = (currentMode == BatteryMode::ACTIVE) 
        ? (100.0f / ACTIVE_LIFETIME) // 0.2778 for active mode
        : (100.0f / LOW_POWER_LIFETIME); // 0.1389 for low power mode
    
    // Check each round to see if robot has enough battery
    for (const auto& round : allRounds) {
        // Calculate required percentage for this round in current mode
        float requiredPercent = round.activeMinutes * requiredPercentMultiplier;
        
        // If we have enough battery percentage, add this round to available list
        if (batteryPercentage >= requiredPercent) {
            availableRounds.push_back(round);
        }
    }
    
    return availableRounds; // Return list of rounds robot can complete
}

// Simulates battery voltage based on percentage
float Energy::simulateVoltage() {
    // Simulates a LiPo battery: ~3.7V nominal, 4.2V fully charged, 3.0V discharged
    if (batteryPercentage >= 100.0f) return 4.2f; // Fully charged voltage
    if (batteryPercentage <= 0.0f) return 3.0f;   // Fully discharged voltage
    
    // Linear interpolation between 3.0V and 4.2V based on battery percentage
    return 3.0f + (batteryPercentage / 100.0f) * 1.2f;
}

// Checks if charging warning is needed based on remaining active time
void Energy::checkChargingWarning() {
    // Calculate remaining minutes in active mode
    float remainingActiveMinutes = getRemainingMinutes(BatteryMode::ACTIVE);
    
    // If less than warning threshold and not already charging, issue warning
    if (remainingActiveMinutes < WARNING_THRESHOLD && !isCharging) {
        ESP_LOGW(TAG, "BATTERY WARNING: Only %.1f minutes remaining in active mode!", 
                (double)remainingActiveMinutes);
        ESP_LOGW(TAG, "Please charge the battery (current: %.1f%%)", (double)batteryPercentage);
    }
}

// Updates remaining time based on current mode (kept for backward compatibility)
void Energy::updateRemainingTime() {
    // This function is intentionally empty as remaining time is calculated on demand
    // in getRemainingMinutes() based on current battery percentage
}