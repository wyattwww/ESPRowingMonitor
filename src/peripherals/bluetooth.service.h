#pragma once

#include <array>

#include "NimBLEDevice.h"

#include "../utils/EEPROM.service.h"
#include "../utils/enums.h"

class BluetoothService
{
    class ServerCallbacks : public NimBLEServerCallbacks
    {
        BluetoothService &bleService;

    public:
        explicit ServerCallbacks(BluetoothService &_bleService);

        void onConnect(NimBLEServer* pServer) override;
        void onDisconnect(NimBLEServer* pServer) override;
    };

    class ControlPointCallbacks : public NimBLECharacteristicCallbacks
    {
        BluetoothService &bleService;

    public:
        explicit ControlPointCallbacks(BluetoothService &_bleService);

        void onWrite(NimBLECharacteristic *pCharacteristic) override;
    };

    EEPROMService &eepromService;
    ServerCallbacks serverCallbacks;
    ControlPointCallbacks controlPointCallbacks;

    //CSC

    static unsigned char constexpr sensorLocationFlag = SensorLocations::Other;

    static unsigned short constexpr cscFeaturesFlag = CSCFeaturesFlags::CrankRevolutionDataSupported |
                                                      CSCFeaturesFlags::WheelRevolutionDataSupported;
    static unsigned char const cscMeasurementFeaturesFlag = cscFeaturesFlag;
    static unsigned short const cyclingSpeedCadenceSvcUuid = 0x1816;
    static unsigned short const cscMeasurementUuid = 0x2A5B;
    static unsigned short const cscControlPointUuid = 0x2A55;
    static unsigned short const cscFeatureUuid = 0x2A5C;
    static unsigned short const bleAppearanceCyclingSpeedCadence = 1157;

    static unsigned short const pscMeasurementFeaturesFlag =
        PSCMeasurementFeaturesFlags::CrankRevolutionDataPresent |
        PSCMeasurementFeaturesFlags::WheelRevolutionDataPresent;
    static unsigned int constexpr pscFeaturesFlag =
        PSCFeaturesFlags::WheelRevolutionDataSupported |
        PSCFeaturesFlags::CrankRevolutionDataSupported;
    static unsigned short const cyclingPowerSvcUuid = 0x1818;
    static unsigned short const pscMeasurementUuid = 0x2A63;
    static unsigned short const pscControlPointUuid = 0x2A66;
    static unsigned short const pscFeatureUuid = 0x2A65;
    static unsigned short const bleAppearanceCyclingPower = 1156;

    static unsigned short const sensorLocationUuid = 0x2A5D;
    inline static std::string const dragFactorUuid = "CE060031-43E5-11E4-916C-0800200C9A66";
    inline static std::string const dragFactorSvcUuid = "CE060030-43E5-11E4-916C-0800200C9A66";

    static unsigned short const batterySvcUuid = 0x180F;
    static unsigned short const batteryLevelUuid = 0x2A19;

    static unsigned short const deviceInfoSvcUuid = 0x180A;
    static unsigned short const modelNumberSvcUuid = 0x2A24;
    static unsigned short const serialNumberSvcUuid = 0x2A25;
    static unsigned short const softwareNumberSvcUuid = 0x2A28;
    static unsigned short const manufacturerNameSvcUuid = 0x2A29;

    NimBLECharacteristic *batteryLevelCharacteristic = nullptr;
    NimBLECharacteristic *cscMeasurementCharacteristic = nullptr;
    NimBLECharacteristic *pscMeasurementCharacteristic = nullptr;
    NimBLECharacteristic *dragFactorCharacteristic = nullptr;

    //FTMS
    static unsigned short const fitnessServiceUuid = 0x1826;
	static unsigned short const fitnessMachineFeatureCharacteristicUuid = 0x2ACC;
	static unsigned short const fitnessMachineStatusCharacteristicUuid = 0x2ADA;
	static unsigned short const rowerDataCharacteristicUuid = 0x2AD1;
	static unsigned short const fitnessControlCharacteristicUuid = 0x2AD9;
    static unsigned short const fitnessTrainingStatusCharacteristicUuid = 0x2AD3;

    static unsigned short const bleAppearanceFitnessMachine = 1158;

    static unsigned short const userDataServiceUUID = 0x181C;
	static unsigned short const weightDataCharacteristicUUID = 0x2A98;

	static unsigned short const reqOpCode = 0x00;
	static unsigned short const resetOpCode = 0x01;
	static unsigned short const startResumeOpCode = 0x07;
	static unsigned short const stopPauseOpCode = 0x08;
	static unsigned short const weightOpCode = 0x12;
	static unsigned short const successOpCode = 0x80;

    NimBLECharacteristic *ftmsMachineFeatureCharacteristic = nullptr;
    NimBLECharacteristic *ftmsTrainingStatusCharacteristic = nullptr;
    NimBLECharacteristic *ftmsMachineStatusCharacteristic = nullptr;
    NimBLECharacteristic *ftmsRowerDataCharacteristic = nullptr;
    NimBLECharacteristic *ftmsControlCharacteristic = nullptr;

    void setupBleDevice();
    void setupServices();
    NimBLEService *setupCscServices(NimBLEServer *server);
    NimBLEService *setupPscServices(NimBLEServer *server);
    NimBLEService *setupFtmsServices(NimBLEServer *server);
    void setupAdvertisement() const;
    void updateDeviceName();

public:
    explicit BluetoothService(EEPROMService &_eepromService);

    void setup();
    static void startBLEServer();
    static void stopServer();
    void notifyBattery(unsigned char batteryLevel) const;
    void notifyCsc(unsigned short revTime, unsigned int revCount, unsigned short strokeTime, unsigned short strokeCount) const;
    void notifyPsc(unsigned short revTime, unsigned int revCount, unsigned short strokeTime, unsigned short strokeCount, short avgStrokePower) const;
    void notifyFtms(const unsigned short strokeRate, const unsigned short strokeCount,
                                    const long long distance, const float pace, const short power, const short caloriesTotal, 
                                    const short caloriesPerHour, const short caloriesPerMin, const long long elapsedTime) const;
    void notifyDragFactor(unsigned short distance, unsigned char dragFactor) const;
    static bool isAnyDeviceConnected();
};
