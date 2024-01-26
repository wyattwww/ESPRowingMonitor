#pragma once

#include "Preferences.h"

#include "ArduinoLog.h"

#include "configuration.h"
#include "enums.h"

class EEPROMService
{
    Preferences &preferences;

    static constexpr char const *logLevelAddress = "logLevel";
    static constexpr char const *bleServiceFlagAddress = "bleService";
    static constexpr char const *magicNumAddress = "magicNum";
    static constexpr char const *inertiaAddress = "inertia5";
    static constexpr char const *autoDragFactorAddress = "autoDragFactor";
    static constexpr char const *dragFactorAddress = "dragFactor";
    static constexpr char const *ssidAddress = "wifiSSID";
    static constexpr char const *passphraseAddress = "wifiPassphrase";

    ArduinoLogLevel logLevel = Configurations::defaultLogLevel;
    BleServiceFlag bleServiceFlag = Configurations::defaultBleServiceFlag;
    float magicNumber = Configurations::concept2MagicNumber;
    float flywheelInertia = Configurations::flywheelInertia;
    bool autoDragFactor = true;
    int dragFactor = 101;
    string ssid = Configurations::ssid;
    string passphrase = Configurations::passphrase;

public:
    explicit EEPROMService(Preferences &_preferences);

    void setup();

    void setLogLevel(ArduinoLogLevel newLogLevel);
    void setBleServiceFlag(BleServiceFlag newServiceFlag);
    void setMagicNumber(float num);
    void setFlywheelInertia(float inertia);
    void setAutoDragFactor(bool isAuto);
    void setDragFactor(int drag);
    void setSSID(string id);
    void setPassphrase(string pw);
    void resetAll();

    BleServiceFlag getBleServiceFlag() const;
    ArduinoLogLevel getLogLevel() const;
    float getMagicNumber() const;
    float getFlywheerInertia() const;
    bool isAutoDragFactor() const;
    int getDragFactor() const;
    string getSSID() const;
    string getPassphrase() const;
};