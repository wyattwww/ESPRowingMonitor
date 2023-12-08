#include "ArduinoLog.h"

#include "EEPROM.service.h"

EEPROMService::EEPROMService(Preferences &_preferences) : preferences(_preferences)
{
}

// TODO: determine if the preferences.end() function call is necessary in my case
void EEPROMService::setup()
{
    Log.infoln("Setup EEPROM Data store");
    if (!preferences.begin("bikeSettings"))
    {
        Log.warningln("Error opening EEPROM using default bike settings");
    }

    if (!preferences.isKey(logLevelAddress))
    {
        Log.infoln("Setting LogLevel to default");
        preferences.putUChar(logLevelAddress, static_cast<unsigned char>(Configurations::defaultLogLevel));
    }
    if (!preferences.isKey(bleServiceFlagAddress))
    {
        Log.infoln("Setting BleServiceFlag to default");
        preferences.putUChar(bleServiceFlagAddress, static_cast<unsigned char>(Configurations::defaultBleServiceFlag));
    }
    if (!preferences.isKey(magicNumAddress))
    {
        Log.infoln("Setting magic number to default to %D", Configurations::concept2MagicNumber);
        preferences.putFloat(magicNumAddress, static_cast<float>(Configurations::concept2MagicNumber));
    }  
    if (!preferences.isKey(inertiaAddress))
    {
        Log.infoln("Setting flywheelInertia to default to %D", Configurations::flywheelInertia);
        preferences.putFloat(inertiaAddress, static_cast<float>(Configurations::flywheelInertia));
    }
    if (!preferences.isKey(dragFactorAddress))
    {
        Log.infoln("Setting drag factor to default to %d", 101);
        preferences.putInt(dragFactorAddress, 101);
    }
    if (!preferences.isKey(autoDragFactorAddress))
    {
        Log.infoln("Setting auto drag factor to default to %d", 1);
        preferences.putBool(autoDragFactorAddress, 1);
    }

    logLevel = static_cast<ArduinoLogLevel>(preferences.getUChar(logLevelAddress, static_cast<unsigned char>(Configurations::defaultLogLevel)));
    bleServiceFlag = static_cast<BleServiceFlag>(preferences.getUChar(bleServiceFlagAddress, static_cast<unsigned char>(Configurations::defaultBleServiceFlag)));
    magicNumber = static_cast<float>(preferences.getFloat(magicNumAddress, static_cast<float>(Configurations::concept2MagicNumber)));
    flywheelInertia = static_cast<float>(preferences.getFloat(inertiaAddress, static_cast<float>(Configurations::flywheelInertia)));
    dragFactor = static_cast<int>(preferences.getInt(dragFactorAddress, 101));
    autoDragFactor = static_cast<bool>(preferences.getBool(autoDragFactorAddress, true));

    Log.infoln("%s: %d", logLevelAddress, logLevel);
    Log.infoln("%s: %d", bleServiceFlagAddress, bleServiceFlag);
    Log.infoln("%s: %D", magicNumAddress, magicNumber);
    Log.infoln("%s: %D", inertiaAddress, flywheelInertia);
    Log.infoln("%s: %d", dragFactorAddress, dragFactor);
    Log.infoln("%s: %d", autoDragFactorAddress, autoDragFactor);
}

void EEPROMService::setLogLevel(ArduinoLogLevel newLogLevel)
{
    int intLogLevel = static_cast<int>(newLogLevel);
    if (intLogLevel < 0 || intLogLevel > 6)
    {
        Log.errorln("Invalid LogLevel setting, should be between 0-6");
        return;
    }

    preferences.putUChar(logLevelAddress, intLogLevel);
    Log.setLevel(intLogLevel);
    logLevel = newLogLevel;
}

void EEPROMService::setBleServiceFlag(BleServiceFlag newServiceFlag)
{
    int intBleServiceFlag = static_cast<int>(newServiceFlag);
    if (intBleServiceFlag < 0 || intBleServiceFlag > 2)
    {
        Log.errorln("Invalid BLE Service setting, should be between 0 or 2");
        return;
    }

    bleServiceFlag = newServiceFlag;
    preferences.putUChar(bleServiceFlagAddress, intBleServiceFlag);
}

void EEPROMService::setMagicNumber(float magicNum)
{
    if (magicNum < 1 || magicNum > 4)
    {
        Log.errorln("Invalid magic number setting, should be between 1 and 4: %D", magicNum);
        return;
    }

    magicNumber = magicNum;
    preferences.putFloat(magicNumAddress, magicNumber);
}

void EEPROMService::setFlywheelInertia(float inertia)
{
    if (inertia <= 0 || inertia >= 2)
    {
        Log.errorln("Invalid flywheel inertia setting, should be between 0 and 2: %D", inertia);
        return;
    }

    flywheelInertia = inertia;
    preferences.putFloat(inertiaAddress, flywheelInertia);
}

void EEPROMService::setAutoDragFactor(bool isAuto)
{
    autoDragFactor = isAuto;
    preferences.putBool(autoDragFactorAddress, autoDragFactor);
}

void EEPROMService::setDragFactor(int drag)
{
    if( drag < LOWER_DRAG_FACTOR_THRESHOLD || drag > UPPER_DRAG_FACTOR_THRESHOLD ) {
        Log.errorln("Invalid drag factor, should be between 75 and 250: %d", drag);
        return;
    }
    dragFactor = drag;
    preferences.putInt(dragFactorAddress, dragFactor);
}

void EEPROMService::resetAll() 
{
    setMagicNumber(Configurations::concept2MagicNumber);
    setFlywheelInertia(Configurations::flywheelInertia);
    setAutoDragFactor(true);
    setDragFactor(101);
}

BleServiceFlag EEPROMService::getBleServiceFlag() const
{
    return bleServiceFlag;
}

ArduinoLogLevel EEPROMService::getLogLevel() const
{
    return logLevel;
}

float EEPROMService::getMagicNumber() const 
{
    return magicNumber;
}

float EEPROMService::getFlywheerInertia() const 
{
    return flywheelInertia;
}

bool EEPROMService::isAutoDragFactor() const
{
    return autoDragFactor;
}

int EEPROMService::getDragFactor() const
{
    return dragFactor;
}