#pragma once

#include "bluetooth.service.h"
#include "network.service.h"
#include "../utils/configuration.h"
#include "utils/EEPROM.service.h"

class PeripheralsController
{
    BluetoothService &bluetoothService;
    NetworkService &networkService;
    EEPROMService &eepromService;

    static const unsigned int updateInterval = 1000;

    unsigned int lastConnectedDeviceCheckTime = 0;
    unsigned int lastBroadcastTime = 0UL;

    unsigned short lastStrokeCount = 0;
    unsigned long long lastStrokeTime = 0;
    
    unsigned char batteryLevelData = 0;
    unsigned short bleRevTimeData = 0;
    unsigned int bleRevCountData = 0;
    unsigned short bleStrokeTimeData = 0;
    unsigned short bleStrokeCountData = 0;
    short bleAvgStrokePowerData = 0;
    unsigned short bleStrokeRateData = 0;
    Configurations::precision blePaceData = 0;
    unsigned int bleCaloriesTotalData = 0;
    unsigned int bleCaloriesPerHourData = 0;
    unsigned int bleCaloriesPerMinData = 0;
    unsigned long long bleElapsedTimeData = 0;
    std::vector<Configurations::precision> bleDriveHandleForces;

    CRGB::HTMLColorCode ledColor = CRGB::Black;
    inline static CRGB leds[1];

    void notify() const;
    void updateLed(CRGB::HTMLColorCode newLedColor);
    void setupConnectionIndicatorLed() const;

public:
    PeripheralsController(BluetoothService &_bluetoothService, NetworkService &_networkService, EEPROMService &_eepromService);

    void begin();
    void update(unsigned char batteryLevel);
    void notifyBattery(unsigned char batteryLevel);
    void updateData(const RowingDataModels::RowingMetrics data);
    void notifyDragFactor(unsigned char dragFactor) const;
    void notifySwellSyncStatus(float inertia, bool isAutoDrag, int dragFactor, float magicNum) const;
    bool isAnyDeviceConnected();
    unsigned long long getElapsedTIme();
};
