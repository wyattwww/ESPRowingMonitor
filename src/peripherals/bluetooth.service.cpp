#include <string>

#include "ArduinoLog.h"
#include "NimBLEDevice.h"

#include "../utils/configuration.h"
#include "bluetooth.service.h"
#include "globals.h"

using std::array;
using std::to_string;

BluetoothService::ServerCallbacks::ServerCallbacks(BluetoothService &_bleService) : bleService(_bleService) {}

void BluetoothService::ServerCallbacks::onConnect(NimBLEServer* pServer)
{
    Log.traceln("Bluetooth server connected");
}

void BluetoothService::ServerCallbacks::onDisconnect(NimBLEServer* pServer)
{
    Log.traceln("Bluetooth server disconnected");
}

BluetoothService::ControlPointCallbacks::ControlPointCallbacks(BluetoothService &_bleService) : bleService(_bleService) {}

void BluetoothService::ControlPointCallbacks::onWrite(NimBLECharacteristic *const pCharacteristic)
{
    NimBLEAttValue message = pCharacteristic->getValue();

    Log.verboseln("Incoming connection");

    if (message.length() == 0)
    {
        Log.infoln("Invalid request, no Op Code");
        array<uint8_t, 3> errorResponse = {
            static_cast<unsigned char>(PSCOpCodes::ResponseCode),
            static_cast<unsigned char>(0),
            static_cast<unsigned char>(PSCResponseOpCodes::OperationFailed)};
        pCharacteristic->setValue(errorResponse);
        pCharacteristic->indicate();

        return;
    }

    Log.infoln("Op Code: %d; Length: %d", message[0], message.length());

    switch (message[0])
    {

    case static_cast<int>(PSCOpCodes::SetLogLevel):
    {
        Log.infoln("Set LogLevel");

        auto response = PSCResponseOpCodes::InvalidParameter;
        if (message.length() == 2 && message[1] >= 0 && message[1] <= 6)
        {
            Log.infoln("New LogLevel: %d", message[1]);
            bleService.eepromService.setLogLevel(static_cast<ArduinoLogLevel>(message[1]));
            response = PSCResponseOpCodes::Successful;
        }

        array<uint8_t, 3>
            temp = {
                static_cast<unsigned char>(PSCOpCodes::ResponseCode),
                static_cast<unsigned char>(message[0]),
                static_cast<unsigned char>(response)};

        pCharacteristic->setValue(temp);
    }
    break;

    case static_cast<int>(PSCOpCodes::ChangeBleService):
    {
        Log.infoln("Change BLE Service");

        if (message.length() == 2 && message[1] >= 0 && message[1] <= 1)
        {
            if( message[1] == static_cast<unsigned char>(BleServiceFlag::CscService) ) {
                Log.infoln("New BLE Service: %s", "CSC");
            }
            else if( message[1] == static_cast<unsigned char>(BleServiceFlag::CpsService) ) {
                Log.infoln("New BLE Service: %s", "CPS");
            }
            else if( message[1] == static_cast<unsigned char>(BleServiceFlag::FtmsService) ) {
                Log.infoln("New BLE Service: %s", "FTMS");
            }

            bleService.eepromService.setBleServiceFlag(static_cast<BleServiceFlag>(message[1]));
            array<uint8_t, 3> temp = {
                static_cast<unsigned char>(PSCOpCodes::ResponseCode),
                static_cast<unsigned char>(message[0]),
                static_cast<unsigned char>(PSCResponseOpCodes::Successful)};
            pCharacteristic->setValue(temp);
            pCharacteristic->indicate();

            Log.verboseln("Restarting device in 5s");
            delay(5000);
            esp_restart();

            break;
        }

        array<uint8_t, 3> temp = {
            static_cast<unsigned char>(PSCOpCodes::ResponseCode),
            static_cast<unsigned char>(message[0]),
            static_cast<unsigned char>(PSCResponseOpCodes::InvalidParameter)};

        pCharacteristic->setValue(temp);
    }
    break;

    default:
    {
        Log.infoln("Not Supported Op Code: %d", message[0]);
        array<uint8_t, 3> response = {
            static_cast<unsigned char>(PSCOpCodes::ResponseCode),
            static_cast<unsigned char>(message[0]),
            static_cast<unsigned char>(PSCResponseOpCodes::UnsupportedOpCode)};
        pCharacteristic->setValue(response);
    }
    break;
    }

    Log.verboseln("Send indicate");
    pCharacteristic->indicate();
}

BluetoothService::BluetoothService(EEPROMService &_eepromService) : eepromService(_eepromService), controlPointCallbacks(*this), serverCallbacks(*this)
{
}

bool BluetoothService::isAnyDeviceConnected()
{
    return NimBLEDevice::getServer()->getConnectedCount() > 0;
}

void BluetoothService::setup()
{
    setupBleDevice();
    BluetoothService::startBLEServer();
}

void BluetoothService::startBLEServer()
{
    NimBLEDevice::getAdvertising()->start();
    Log.verboseln("Waiting a client connection to notify...");
}

void BluetoothService::stopServer()
{
    NimBLEDevice::getAdvertising()->stop();
}

void BluetoothService::notifyBattery(const unsigned char batteryLevel) const
{
    batteryLevelCharacteristic->setValue(batteryLevel);
    if (batteryLevelCharacteristic->getSubscribedCount() > 0)
    {
        batteryLevelCharacteristic->notify();
    }
}

void BluetoothService::notifyDragFactor(const unsigned short distance, const unsigned char dragFactor) const
{
    if( dragFactorCharacteristic == nullptr ) 
    {
        return;
    }
    std::string value = "DF=" + to_string(dragFactor) + ", Dist=" + to_string(distance);
    dragFactorCharacteristic->setValue(value);
    if (dragFactorCharacteristic->getSubscribedCount() > 0)
    {
        dragFactorCharacteristic->notify();
    }
}

void BluetoothService::notifyCsc(const unsigned short revTime, const unsigned int revCount, const unsigned short strokeTime, const unsigned short strokeCount) const
{
    if (cscMeasurementCharacteristic->getSubscribedCount() > 0)
    {
        // execution time: 0-1 microsec
        // auto start = micros();
        array<uint8_t, 11> temp = {
            cscMeasurementFeaturesFlag,

            static_cast<unsigned char>(revCount),
            static_cast<unsigned char>(revCount >> 8),
            static_cast<unsigned char>(revCount >> 16),
            static_cast<unsigned char>(revCount >> 24),

            static_cast<unsigned char>(revTime),
            static_cast<unsigned char>(revTime >> 8),

            static_cast<unsigned char>(strokeCount),
            static_cast<unsigned char>(strokeCount >> 8),
            static_cast<unsigned char>(strokeTime),
            static_cast<unsigned char>(strokeTime >> 8)};

        // auto stop = micros();
        // Serial.print("data calc: ");
        // Serial.println(stop - start);

        // execution time: 28-35 microsec
        // auto start = micros();
        cscMeasurementCharacteristic->setValue(temp);
        // auto stop = micros();
        // Serial.print("set value: ");
        // Serial.println(stop - start);

        // execution time: 1000-1600 microsec
        // start = micros();
        cscMeasurementCharacteristic->notify();
        // stop = micros();
        // Serial.print("notify: ");
        // Serial.println(stop - start);
    }
}

void BluetoothService::notifyPsc(const unsigned short revTime, const unsigned int revCount, const unsigned short strokeTime, const unsigned short strokeCount, const short avgStrokePower) const
{
    if (pscMeasurementCharacteristic->getSubscribedCount() > 0)
    {
        // execution time: 0-1 microsec
        // auto start = micros();
        array<uint8_t, 14> temp = {
            static_cast<unsigned char>(pscMeasurementFeaturesFlag),
            static_cast<unsigned char>(pscMeasurementFeaturesFlag >> 8),

            static_cast<unsigned char>(avgStrokePower),
            static_cast<unsigned char>(avgStrokePower >> 8),

            static_cast<unsigned char>(revCount),
            static_cast<unsigned char>(revCount >> 8),
            static_cast<unsigned char>(revCount >> 16),
            static_cast<unsigned char>(revCount >> 24),
            static_cast<unsigned char>(revTime),
            static_cast<unsigned char>(revTime >> 8),

            static_cast<unsigned char>(strokeCount),
            static_cast<unsigned char>(strokeCount >> 8),
            static_cast<unsigned char>(strokeTime),
            static_cast<unsigned char>(strokeTime >> 8),
        };

        // auto stop = micros();
        // Serial.print("data calc: ");
        // Serial.println(stop - start);

        // execution time: 28-35 microsec
        // auto start = micros();
        pscMeasurementCharacteristic->setValue(temp);
        // auto stop = micros();
        // Serial.print("set value: ");
        // Serial.println(stop - start);

        // execution time: 1000-1600 microsec
        // start = micros();
        pscMeasurementCharacteristic->notify();
        // stop = micros();
        // Serial.print("notify: ");
        // Serial.println(stop - start);
    }
}

void BluetoothService::notifyFtms(const unsigned short strokeRate, const unsigned short strokeCount, 
                                    const long long distance, const float pace, const short power, const short caloriesTotal, 
                                    const short caloriesPerHour, const short caloriesPerMin, const long long elapsedTime) const
{
    if (ftmsRowerDataCharacteristic->getSubscribedCount() > 0)
    {
        // execution time: 0-1 microsec
        // auto start = micros();
        array<uint8_t, 19> temp = {
            // Field flags as defined in the Bluetooth Documentation
            // Stroke Rate (default), Stroke Count (default), Total Distance (2), Instantaneous Pace (3),
            // Instantaneous Power (5), Total / Expended Energy (8), Elapsed Time (11)
            // todo: might add: Average Stroke Rate (1), Average Pace (4), Average Power (6), Heart Rate (9), 
            // Remaining Time (12)
            // 00101100
            static_cast<unsigned char>(0x2c),
            // 00001001
            static_cast<unsigned char>(0x09),

            // see https://www.bluetooth.com/specifications/specs/gatt-specification-supplement-3/
            // for some of the data types
            // Stroke Rate in stroke/minute, value is multiplied by 2 to have a .5 precision
            static_cast<unsigned char>(strokeRate),

            // Stroke Count
            static_cast<unsigned char>(strokeCount),
            static_cast<unsigned char>(strokeCount >> 8),

            // Total Distance in meters
            static_cast<unsigned char>(distance),
            static_cast<unsigned char>(distance >> 8),
            static_cast<unsigned char>(distance >> 16),
            
            // Instantaneous Pace in seconds/500m
            // if split is infinite (i.e. while pausing), should use the highest possible number (0xFFFF)
            // todo: eventhough mathematically correct, setting 0xFFFF (65535s) causes some ugly spikes
            // in some applications which could shift the axis (i.e. workout diagrams in MyHomeFit)
            // so instead for now we use 0 here
            static_cast<unsigned char>(lround(pace)),
            static_cast<unsigned char>(lround(pace) >> 8),

            // Instantaneous Power in watts
            static_cast<unsigned char>(power),
            static_cast<unsigned char>(power >> 8),

            // Energy in kcal
            // Total energy in kcal
            static_cast<unsigned char>(caloriesTotal),
            static_cast<unsigned char>(caloriesTotal >> 8),

            static_cast<unsigned char>(caloriesPerHour),
            static_cast<unsigned char>(caloriesPerHour >> 8),

            static_cast<unsigned char>(caloriesPerMin),
            
            // Elapsed Time: Seconds with a resolution of 1
            static_cast<unsigned char>(elapsedTime),
            static_cast<unsigned char>(elapsedTime >> 8),
        };

        // auto stop = micros();
        // Serial.print("data calc: ");
        // Serial.println(stop - start);

        // execution time: 28-35 microsec
        // auto start = micros();
        ftmsRowerDataCharacteristic->setValue(temp);
        // auto stop = micros();
        // Serial.print("set value: ");
        // Serial.println(stop - start);

        // execution time: 1000-1600 microsec
        // start = micros();
        ftmsRowerDataCharacteristic->notify();
        // stop = micros();
        // Serial.print("notify: ");
        // Serial.println(stop - start);
    }
}

void BluetoothService::setupBleDevice()
{
    Log.traceln("Initializing BLE device");

    std::string deviceName = Configurations::deviceName;
     
    NimBLEDevice::init(deviceName);
    updateDeviceName();
    NimBLEDevice::setPower(ESP_PWR_LVL_N6);

    Log.traceln("Setting up Server");

    NimBLEDevice::createServer();

    setupServices();
    setupAdvertisement();
}

void BluetoothService::updateDeviceName() 
{
    const uint8_t * nativeAddr = NimBLEDevice::getAddress().getNative();
    std::string devName = hexStr( nativeAddr, 4 );

    std::string deviceName =  Configurations::deviceName + " ";
    deviceName += devName.c_str();
    
    Log.traceln("BLE address: %s", deviceName.c_str());

    NimBLEDevice::setDeviceName(deviceName.c_str());
}

void BluetoothService::setupServices()
{
    Log.verboseln("Setting up BLE Services");
    auto *server = NimBLEDevice::getServer();
    auto *batteryService = server->createService(batterySvcUuid);
    auto *deviceInfoService = server->createService(deviceInfoSvcUuid);

    NimBLEService *measurementService = nullptr;
    if( eepromService.getBleServiceFlag() == BleServiceFlag::CscService ) {
        measurementService = setupCscServices(server);
    }
    else if( eepromService.getBleServiceFlag() == BleServiceFlag::CpsService ) {
        measurementService = setupPscServices(server);
    }
    else if( eepromService.getBleServiceFlag() == BleServiceFlag::FtmsService ) {
        measurementService = setupFtmsServices(server);
    } 

    Log.verboseln("Setting up BLE Characteristics");

    batteryLevelCharacteristic = batteryService->createCharacteristic(batteryLevelUuid, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);

    deviceInfoService
        ->createCharacteristic(manufacturerNameSvcUuid, NIMBLE_PROPERTY::READ)
        ->setValue("Swell City");
    deviceInfoService
        ->createCharacteristic(modelNumberSvcUuid, NIMBLE_PROPERTY::READ)
        ->setValue("Swell Monitor");
    deviceInfoService
        ->createCharacteristic(serialNumberSvcUuid, NIMBLE_PROPERTY::READ)
        ->setValue("20231015");
    deviceInfoService
        ->createCharacteristic(softwareNumberSvcUuid, NIMBLE_PROPERTY::READ)
        ->setValue("4.1.0.1");

    Log.verboseln("Starting BLE Service");

    batteryService->start();
    measurementService->start();
    deviceInfoService->start();
    server->setCallbacks(&serverCallbacks);
    server->start();
}

NimBLEService *BluetoothService::setupCscServices(NimBLEServer *const server)
{
    Log.infoln("Setting up Cycling Speed and Cadence Profile");

    auto *cscService = server->createService(cyclingSpeedCadenceSvcUuid);
    cscMeasurementCharacteristic = cscService->createCharacteristic(cscMeasurementUuid, NIMBLE_PROPERTY::NOTIFY);

    dragFactorCharacteristic = cscService->createCharacteristic(dragFactorUuid, NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ);

    cscService
        ->createCharacteristic(cscFeatureUuid, NIMBLE_PROPERTY::READ)
        ->setValue(cscFeaturesFlag);

    cscService
        ->createCharacteristic(sensorLocationUuid, NIMBLE_PROPERTY::READ)
        ->setValue(sensorLocationFlag);

    cscService->createCharacteristic(cscControlPointUuid, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::INDICATE)->setCallbacks(&controlPointCallbacks);

    return cscService;
}

NimBLEService *BluetoothService::setupPscServices(NimBLEServer *const server)
{
    Log.infoln("Setting up Cycling Power Profile");
    auto *pscService = server->createService(cyclingPowerSvcUuid);
    pscMeasurementCharacteristic = pscService->createCharacteristic(pscMeasurementUuid, NIMBLE_PROPERTY::NOTIFY);

    dragFactorCharacteristic = pscService->createCharacteristic(dragFactorUuid, NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ);

    pscService
        ->createCharacteristic(pscFeatureUuid, NIMBLE_PROPERTY::READ)
        ->setValue(pscFeaturesFlag);

    pscService
        ->createCharacteristic(sensorLocationUuid, NIMBLE_PROPERTY::READ)
        ->setValue(sensorLocationFlag);

    pscService->createCharacteristic(pscControlPointUuid, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::INDICATE)->setCallbacks(&controlPointCallbacks);

    return pscService;
}

NimBLEService *BluetoothService::setupFtmsServices(NimBLEServer *const server)
{
    Log.infoln("Setting up FitnessMachine Profile");

    auto *ftmsService = server->createService(fitnessServiceUuid);

    //0x2ACC
    ftmsMachineFeatureCharacteristic = ftmsService->createCharacteristic(fitnessMachineFeatureCharacteristicUuid, NIMBLE_PROPERTY::READ);

    //0x2AD1
    ftmsRowerDataCharacteristic = ftmsService->createCharacteristic(rowerDataCharacteristicUuid, NIMBLE_PROPERTY::NOTIFY);

    //0x2ADA
    ftmsMachineStatusCharacteristic = ftmsService->createCharacteristic(fitnessMachineStatusCharacteristicUuid, NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ);

    //0x2AD3
    ftmsTrainingStatusCharacteristic = ftmsService->createCharacteristic(fitnessTrainingStatusCharacteristicUuid, NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ);
    
    //0x2AD9
    ftmsService->createCharacteristic(fitnessControlCharacteristicUuid, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::INDICATE)->setCallbacks(&controlPointCallbacks);
    
    return ftmsService;
}

void BluetoothService::setupAdvertisement() const
{
    auto *pAdvertising = NimBLEDevice::getAdvertising();
    if (eepromService.getBleServiceFlag() == BleServiceFlag::CpsService)
    {
        pAdvertising->setAppearance(bleAppearanceCyclingPower);
        pAdvertising->addServiceUUID(cyclingPowerSvcUuid);
    }
    if (eepromService.getBleServiceFlag() == BleServiceFlag::CscService)
    {
        pAdvertising->setAppearance(bleAppearanceCyclingSpeedCadence);
        pAdvertising->addServiceUUID(cyclingSpeedCadenceSvcUuid);
    }
    if (eepromService.getBleServiceFlag() == BleServiceFlag::FtmsService)
    {
        pAdvertising->setAppearance(bleAppearanceFitnessMachine);
        pAdvertising->addServiceUUID(fitnessServiceUuid);
    }
}