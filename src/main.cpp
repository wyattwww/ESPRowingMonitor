#include "ArduinoLog.h"
#include "globals.h"
#include "test.array.h"

void setup()
{
    auto const baudRate = 1500000;
    Serial.begin(baudRate);
    while (!Serial && !(bool)Serial.available())
    {
    }

    Log.begin(static_cast<int>(Configurations::defaultLogLevel), &Serial, false);
    Log.setPrefix(printPrefix);

    eepromService.setup();
    Log.setLevel(static_cast<int>(eepromService.getLogLevel()));

    peripheralController.begin();
    StrokeController::begin();
    powerManagerController.begin();

    peripheralController.notifyBattery(powerManagerController.getBatteryLevel());
    int dragFactor = static_cast<int>(strokeController.getDragFactor());
    if( !eepromService.isAutoDragFactor() ) {
        dragFactor = eepromService.getDragFactor();
    }
    if( dragFactor == 0 ) {
        dragFactor = strokeService.dragCoefficient * 1e6;
    }
    peripheralController.notifySwellSyncStatus(eepromService.getFlywheerInertia(), eepromService.isAutoDragFactor(), dragFactor, eepromService.getMagicNumber());
}

// execution time
// - not connected 30 microsec
// - connected  microsec 2000-4900
void loop()
{
    if( SIMULATE_STROKES && BluetoothService::isAnyDeviceConnected() ) {
        simulateRotation();
    }

    if( !peripheralController.isAnyDeviceConnected() ) {
        strokeService.resetStartTime();
    }

    strokeController.update();
    peripheralController.update(powerManagerController.getBatteryLevel());
    powerManagerController.update(strokeController.getRawImpulseTime(), peripheralController.isAnyDeviceConnected());

    // auto start = micros();

    // execution time
    // - not connected 20-30 microsec
    // - connected 1900-2200 microsec
    // auto start = micros();
    auto const now = millis();
    auto const minUpdateInterval = 1500;
    if (strokeController.getStrokeCount() != strokeController.getPreviousStrokeCount() || now - lastUpdateTime > minUpdateInterval)
    {
        peripheralController.updateData(strokeController.getAllData());

        int dragFactor = static_cast<int>(strokeController.getDragFactor());
        if( !eepromService.isAutoDragFactor() ) {
            dragFactor = eepromService.getDragFactor();
        }
        if( dragFactor == 0 ) {
            dragFactor = strokeService.dragCoefficient * 1e6;
        }
        peripheralController.notifySwellSyncStatus(eepromService.getFlywheerInertia(), eepromService.isAutoDragFactor(), dragFactor, eepromService.getMagicNumber());
        
        lastUpdateTime = now;
    }

    if (strokeController.getStrokeCount() != strokeController.getPreviousStrokeCount())
    {
        Log.infoln("pace: %D", strokeController.getPace());
        Log.infoln("batteryLevel: %d", powerManagerController.getBatteryLevel());
        Log.infoln("bleServiceFlag: %d", eepromService.getBleServiceFlag());
        Log.infoln("logLevel: %d", eepromService.getLogLevel());
        Log.infoln("magicNumber: %D", eepromService.getMagicNumber());
        Log.infoln("flywheelInertia: %D", eepromService.getFlywheerInertia());
        Log.infoln("revTime: %u", strokeController.getLastRevTime());
        Log.infoln("strokeTime: %u", strokeController.getLastStrokeTime());
        Log.infoln("strokeCount: %u", strokeController.getStrokeCount());
        Log.infoln("totalCalories: %u", strokeService.getTotalCalories());
        Log.infoln("driveDuration: %D", strokeController.getDriveDuration());
        Log.infoln("recoveryDuration: %D", strokeController.getRecoveryDuration());
        Log.infoln("autoDragFactor: %d", eepromService.isAutoDragFactor());
        Log.infoln("dragFactor: %d", strokeController.getDragFactor());
        Log.infoln("power: %d", strokeController.getAvgStrokePower());
        Log.infoln("distance: %D", strokeController.getDistance() / 100.0);
        Log.infoln("elapsedTime: %u", peripheralController.getElapsedTIme());
        string forcesStr = "handleForces: [";
        for (auto const &handleForce : strokeController.getAllData().driveHandleForces)
        {
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%g", handleForce);

            forcesStr += buffer;
            forcesStr += ", ";
        }
        forcesStr += "]";
        Log.infoln(forcesStr.c_str());
        
        Log.infoln("--------");

        // execution time
        // - not connected: 173-200
        // - connected: 900-2700
        // auto start = micros();
        peripheralController.notifyDragFactor(strokeController.getDragFactor());

        strokeController.setPreviousStrokeCount();
        // auto stop = micros();
        // Serial.print("notifyDragFactor: ");
        // Serial.println(stop - start);
    }

    if (powerManagerController.getBatteryLevel() != powerManagerController.getPreviousBatteryLevel())
    {
        Log.infoln("batteryLevel: %d", powerManagerController.getBatteryLevel());
        peripheralController.notifyBattery(powerManagerController.getBatteryLevel());
        powerManagerController.setPreviousBatteryLevel();
    }
    // auto stop = micros();
    // if (stop - start > 10)
    // {
    //     Serial.print("Main loop: ");
    //     Serial.println(stop - start);
    // }
}