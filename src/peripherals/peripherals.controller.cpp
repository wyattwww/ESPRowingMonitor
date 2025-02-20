#include "ArduinoLog.h"

#include "../utils/configuration.h"
#include "peripherals.controller.h"

PeripheralsController::PeripheralsController(BluetoothService &_bluetoothService, NetworkService &_networkService, EEPROMService &_eepromService) : bluetoothService(_bluetoothService), networkService(_networkService), eepromService(_eepromService)
{
}

void PeripheralsController::update(unsigned char batteryLevel)
{
    if constexpr (Configurations::isWebsocketEnabled)
    {
        networkService.update();
    }

    auto const now = millis();
    if (now - lastConnectedDeviceCheckTime > Configurations::ledBlinkFrequency)
    {
        auto ledColor = CRGB::Blue;
        const auto minBattLevel = 30;
        if (batteryLevel < minBattLevel)
        {
            ledColor = CRGB::Red;
        }
        const auto maxBattLevel = 80;
        if (batteryLevel > maxBattLevel)
        {
            ledColor = CRGB::Green;
        }
        updateLed(ledColor);
        lastConnectedDeviceCheckTime = now;
    }

    if constexpr (Configurations::isBleSErviceEnabled)
    {
        if (now - lastBroadcastTime > updateInterval)
        {
            notify();
            lastBroadcastTime = now;
        }
    }
}

void PeripheralsController::begin()
{
    Log.infoln("Setting up BLE Controller");

    if constexpr (Configurations::isWebsocketEnabled)
    {
        networkService.setup();
    }

    if constexpr (Configurations::isBleSErviceEnabled)
    {
        bluetoothService.setup();
    }

    setupConnectionIndicatorLed();
}

bool PeripheralsController::isAnyDeviceConnected()
{
    return BluetoothService::isAnyDeviceConnected() || networkService.isAnyDeviceConnected();
}

void PeripheralsController::updateLed(CRGB::HTMLColorCode newLedColor)
{
    ledColor = isAnyDeviceConnected() ? newLedColor : ledColor == CRGB::Black ? newLedColor
                                                                              : CRGB::Black;
    leds[0] = ledColor;
    FastLED.show();
}

void PeripheralsController::notifyBattery(const unsigned char batteryLevel)
{
    batteryLevelData = batteryLevel;
    if constexpr (Configurations::isBleSErviceEnabled)
    {
        bluetoothService.notifyBattery(batteryLevel);
    }
}

void PeripheralsController::updateData(const RowingDataModels::RowingMetrics data)
{
    auto const secInMicroSec = 1e6L;
    bleRevTimeData = lroundl((data.lastRevTime / secInMicroSec) * (eepromService.getBleServiceFlag() == BleServiceFlag::CpsService ? 2048 : 1024)) % USHRT_MAX;
    bleRevCountData = lround(data.distance);
    bleStrokeTimeData = lroundl((data.lastStrokeTime / secInMicroSec) * 1024) % USHRT_MAX;
    bleStrokeCountData = data.strokeCount;
    bleAvgStrokePowerData = static_cast<short>(lround(data.avgStrokePower));
    blePaceData = data.pace;
    bleDriveHandleForces = data.driveHandleForces;

    if( data.strokeCount > lastStrokeCount ) 
    {
        bleStrokeRateData = ((data.strokeCount - lastStrokeCount) / ((data.lastStrokeTime - lastStrokeTime) / secInMicroSec)) * 60;
    }

    bleCaloriesTotalData = data.totalCalories;
    bleCaloriesPerHourData = data.totalCaloriesPerHour;
    bleCaloriesPerMinData = data.totalCaloriesPerMinute;
    bleElapsedTimeData = data.elapsedTime;

    lastStrokeCount = data.strokeCount;
    lastStrokeTime = data.lastStrokeTime;
    
    if constexpr (Configurations::isWebsocketEnabled)
    {
        networkService.notifyClients(data, 
                                        batteryLevelData, 
                                        eepromService.getBleServiceFlag(), 
                                        eepromService.getLogLevel(), 
                                        eepromService.getFlywheerInertia(),
                                        eepromService.isAutoDragFactor(),
                                        eepromService.getDragFactor(),
                                        eepromService.getMagicNumber());
    }
}

unsigned long long PeripheralsController::getElapsedTIme()
{
    return bleElapsedTimeData;
}

void PeripheralsController::notify() const
{
    if (eepromService.getBleServiceFlag() == BleServiceFlag::CpsService)
    {
        bluetoothService.notifyPsc(bleRevTimeData, bleRevCountData, bleStrokeTimeData, bleStrokeCountData, bleAvgStrokePowerData);
    }
    if (eepromService.getBleServiceFlag() == BleServiceFlag::CscService)
    {
        bluetoothService.notifyCsc(bleRevTimeData, bleRevCountData, bleStrokeTimeData, bleStrokeCountData);
    }
    if (eepromService.getBleServiceFlag() == BleServiceFlag::FtmsService)
    {
        bluetoothService.notifyFtms(bleStrokeRateData, bleStrokeCountData, bleRevCountData/100.0, blePaceData, bleAvgStrokePowerData, bleCaloriesTotalData, bleCaloriesPerHourData, bleCaloriesPerMinData, bleElapsedTimeData );
        bluetoothService.notifySwellSyncForces(bleDriveHandleForces);
    }
}

void PeripheralsController::notifyDragFactor(const unsigned char dragFactor) const
{
    if constexpr (Configurations::isBleSErviceEnabled)
    {
        auto const distance = pow(dragFactor / (double)Configurations::concept2MagicNumber, 1.0 / 3.0) * (2.0 * PI) * 10;
        bluetoothService.notifyDragFactor(static_cast<unsigned short>(distance), dragFactor);
    }
}

void PeripheralsController::notifySwellSyncStatus(float inertia, bool isAutoDrag, int dragFactor, float magicNum) const 
{
    if constexpr (Configurations::isBleSErviceEnabled)
    {
        bluetoothService.notifySwellSyncStatus(inertia, isAutoDrag, dragFactor, magicNum);
    }
}

void PeripheralsController::setupConnectionIndicatorLed() const
{
    FastLED.addLeds<WS2812, GPIO_NUM_5>(leds, 1);
}