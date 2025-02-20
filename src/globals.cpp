#include "Arduino.h"

#include <sstream>
#include <iomanip>

#include "globals.h"

Preferences preferences;

EEPROMService eepromService(preferences);
BluetoothService bleService(eepromService);
NetworkService networkService(eepromService);
FlywheelService flywheelService;
StrokeService strokeService;
PowerManagerService powerManagerService;

PeripheralsController peripheralController(bleService, networkService, eepromService);
StrokeController strokeController(strokeService, flywheelService);
PowerManagerController powerManagerController(powerManagerService);

IRAM_ATTR void rotationInterrupt()
{
    // execution time general: 1-5, max: 520 and a few 120
    // auto start = micros();
    flywheelService.processRotation(micros());
    // auto stop = micros();

    // Serial.print("rotationInterrupt: ");
    // Serial.println(stop - start);
}

void attachRotationInterrupt()
{
    attachInterrupt(digitalPinToInterrupt(Configurations::hallSensorPinNumber), rotationInterrupt, FALLING);
}

void detachRotationInterrupt()
{
    detachInterrupt(digitalPinToInterrupt(Configurations::hallSensorPinNumber));
}

void printPrefix(Print *const _logOutput, int logLevel)
{
    printTimestamp(_logOutput);
    printLogLevel(_logOutput, logLevel);
}

void printTimestamp(Print *const _logOutput)
{
    unsigned long const msecs = micros();
    unsigned long const secs = msecs / msecsPerSec;
    unsigned long const microSeconds = msecs % msecsPerSec;
    unsigned long const seconds = secs % secsPerMin;
    unsigned long const minutes = (secs / secsPerMin) % secsPerMin;
    unsigned long const hours = (secs % secsPerDay) / secsPerHour;

    // NOLINTBEGIN
    char timestamp[20];
    sprintf(timestamp, "%02d:%02d:%02d.%06d ", hours, minutes, seconds, microSeconds);
    _logOutput->print(timestamp);
    // NOLINTEND
}

void printLogLevel(Print *const _logOutput, const int logLevel)
{
    switch (logLevel)
    {
    default:
    case 0:
        _logOutput->print(" SILENT - ");
        break;
    case 1:
        _logOutput->print("  FATAL - ");
        break;
    case 2:
        _logOutput->print("  ERROR - ");
        break;
    case 3:
        _logOutput->print("WARNING - ");
        break;
    case 4:
        _logOutput->print("   INFO - ");
        break;
    case 5:
        _logOutput->print("  TRACE - ");
        break;
    case 6:
        _logOutput->print("VERBOSE - ");
        break;
    }
}

std::string hexStr(const uint8_t * data, int len)
{
    std::stringstream ss;
    ss << std::hex;
    for(int i=0;i<len;++i)
        ss << std::setw(2) << std::setfill('0') << (int)data[i];
    std::string hexStr = ss.str();
    std::transform(hexStr.begin(), hexStr.end(), hexStr.begin(), ::toupper);
    return hexStr;
}
