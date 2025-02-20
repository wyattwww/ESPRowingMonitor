#include "ArduinoLog.h"

#include "configuration.h"
#include "power-manager.controller.h"

PowerManagerController::PowerManagerController(PowerManagerService &_powerManagerService) : powerManagerService(_powerManagerService)
{
}

void PowerManagerController::begin()
{
    Log.infoln("Setting up power manager controller");
    powerManagerService.setup();
    batteryLevel = powerManagerService.measureBattery();
}

void PowerManagerController::update(const unsigned long lastRevTime, const bool isDeviceConnected)
{
    auto const now = micros();
    if (!SIMULATE_STROKES && !isDeviceConnected && now - lastRevTime > Configurations::deepSleepTimeout * 1000)
    {
        PowerManagerService::goToSleep();
    }

    if (now - lastBatteryMeasurementTime > Configurations::batteryMeasurementFrequency * 1000)
    {
        batteryLevel = powerManagerService.measureBattery();
        lastBatteryMeasurementTime = now;
    }
}

unsigned char PowerManagerController::getBatteryLevel() const
{
    return batteryLevel;
}

unsigned char PowerManagerController::getPreviousBatteryLevel() const
{
    return previousBatteryLevel;
}

void PowerManagerController::setPreviousBatteryLevel()
{
    previousBatteryLevel = batteryLevel;
}