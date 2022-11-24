#include "ArduinoLog.h"
#include "NimBLEDevice.h"

#include "bluetooth.service.h"
#include "settings.h"

using std::array;
using std::to_string;

BluetoothService::ControlPointCallbacks::ControlPointCallbacks(BluetoothService &_bleService) : bleService(_bleService) {}

void BluetoothService::ControlPointCallbacks::onWrite(NimBLECharacteristic *pCharacteristic)
{
    NimBLEAttValue message = pCharacteristic->getValue();

    Log.verboseln("Incoming connection");

    if (message.length() == 0)
    {
        Log.infoln("Invalid request, no Op Code");
        array<uint8_t, 3> errorResponse = {
            static_cast<byte>(PSCOpCodes::ResponseCode),
            static_cast<byte>(0),
            static_cast<byte>(PSCResponseOpCodes::OperationFailed)};
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
                static_cast<byte>(PSCOpCodes::ResponseCode),
                static_cast<byte>(message[0]),
                static_cast<byte>(response)};

        pCharacteristic->setValue(temp);
    }
    break;

    case static_cast<int>(PSCOpCodes::ChangeBleService):
    {
        Log.infoln("Change BLE Service");

        if (message.length() == 2 && message[1] >= 0 && message[1] <= 1)
        {
            Log.infoln("New BLE Service: %s", message[1] == static_cast<byte>(BleServiceFlag::CscService) ? "CSC" : "CPS");
            bleService.eepromService.setBleServiceFlag(static_cast<BleServiceFlag>(message[1]));
            array<uint8_t, 3> temp = {
                static_cast<byte>(PSCOpCodes::ResponseCode),
                static_cast<byte>(message[0]),
                static_cast<byte>(PSCResponseOpCodes::Successful)};
            pCharacteristic->setValue(temp);
            pCharacteristic->indicate();

            Log.verboseln("Restarting device in 5s");
            delay(5000);
            esp_restart();

            break;
        }

        array<uint8_t, 3> temp = {
            static_cast<byte>(PSCOpCodes::ResponseCode),
            static_cast<byte>(message[0]),
            static_cast<byte>(PSCResponseOpCodes::InvalidParameter)};

        pCharacteristic->setValue(temp);
    }
    break;

    default:
    {
        Log.infoln("Not Supported Op Code: %d", message[0]);
        array<uint8_t, 3> response = {
            static_cast<byte>(PSCOpCodes::ResponseCode),
            static_cast<byte>(message[0]),
            static_cast<byte>(PSCResponseOpCodes::UnsupportedOpCode)};
        pCharacteristic->setValue(response);
    }
    break;
    }

    Log.verboseln("Send indicate");
    pCharacteristic->indicate();
}

BluetoothService::BluetoothService(EEPROMService &_eepromService) : eepromService(_eepromService), controlPointCallbacks(*this)
{
}

bool BluetoothService::isAnyDeviceConnected() const
{
    return NimBLEDevice::getServer()->getConnectedCount() > 0;
}

void BluetoothService::updateLed()
{
    // execution time: 1-5 micro sec
    // auto start = micros();
    if (isAnyDeviceConnected())
    {
        ledState = HIGH;
    }
    else
    {
        ledState = !ledState;
    }

    digitalWrite(GPIO_NUM_2, ledState);
    // auto end = micros();
    // Serial.print("led: ");
    // Serial.println(end - start);
}

void BluetoothService::setup()
{
    setupBleDevice();
    setupConnectionIndicatorLed();
}

void BluetoothService::startBLEServer() const
{
    NimBLEDevice::getAdvertising()->start();
    Log.verboseln("Waiting a client connection to notify...");
}

void BluetoothService::stopServer() const
{
    NimBLEDevice::getAdvertising()->stop();
}

void BluetoothService::notifyBattery(byte batteryLevel) const
{
    batteryLevelCharacteristic->setValue(batteryLevel);
    if (batteryLevelCharacteristic->getSubscribedCount() > 0)
    {
        batteryLevelCharacteristic->notify();
    }
}

void BluetoothService::notifyDragFactor(unsigned short distance, byte dragFactor) const
{
    dragFactorCharacteristic->setValue("DF=" + to_string(dragFactor) + ", Dist=" + to_string(distance));
    if (dragFactorCharacteristic->getSubscribedCount() > 0)
    {
        dragFactorCharacteristic->notify();
    }
}

void BluetoothService::notifyCsc(unsigned short revTime, unsigned int revCount, unsigned short strokeTime, unsigned short strokeCount) const
{
    if (cscMeasurementCharacteristic->getSubscribedCount() > 0)
    {
        // execution time: 0-1 microsec
        // auto start = micros();
        array<uint8_t, 11> temp = {
            cscMeasurementFeaturesFlag,

            static_cast<byte>(revCount),
            static_cast<byte>(revCount >> 8),
            static_cast<byte>(revCount >> 16),
            static_cast<byte>(revCount >> 24),

            static_cast<byte>(revTime),
            static_cast<byte>(revTime >> 8),

            static_cast<byte>(strokeCount),
            static_cast<byte>(strokeCount >> 8),
            static_cast<byte>(strokeTime),
            static_cast<byte>(strokeTime >> 8)};

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

void BluetoothService::notifyPsc(unsigned short revTime, unsigned int revCount, unsigned short strokeTime, unsigned short strokeCount, short avgStrokePower) const
{
    if (pscMeasurementCharacteristic->getSubscribedCount() > 0)
    {
        // execution time: 0-1 microsec
        // auto start = micros();
        array<uint8_t, 14> temp = {
            static_cast<byte>(pscMeasurementFeaturesFlag),
            static_cast<byte>(pscMeasurementFeaturesFlag >> 8),

            static_cast<byte>(avgStrokePower),
            static_cast<byte>(avgStrokePower >> 8),

            static_cast<byte>(revCount),
            static_cast<byte>(revCount >> 8),
            static_cast<byte>(revCount >> 16),
            static_cast<byte>(revCount >> 24),
            static_cast<byte>(revTime),
            static_cast<byte>(revTime >> 8),

            static_cast<byte>(strokeCount),
            static_cast<byte>(strokeCount >> 8),
            static_cast<byte>(strokeTime),
            static_cast<byte>(strokeTime >> 8),
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

void BluetoothService::setupBleDevice()
{
    Log.verboseln("Initializing BLE device");

    auto deviceName = "Concept2 (" + std::string(eepromService.getBleServiceFlag() == BleServiceFlag::CscService ? "CSC)" : "CPS)");
    NimBLEDevice::init(deviceName);
    NimBLEDevice::setPower(ESP_PWR_LVL_N6);

    Log.verboseln("Setting up Server");

    NimBLEDevice::createServer();

    setupServices();
    setupAdvertisement();
}

void BluetoothService::setupServices()
{
    Log.verboseln("Setting up BLE Services");
    auto server = NimBLEDevice::getServer();
    auto batteryService = server->createService(batterySvcUuid);
    auto deviceInfoService = server->createService(deviceInfoSvcUuid);

    auto measurementService = eepromService.getBleServiceFlag() == BleServiceFlag::CscService ? setupCscServices(server) : setupPscServices(server);

    Log.verboseln("Setting up BLE Characteristics");

    batteryLevelCharacteristic = batteryService->createCharacteristic(batteryLevelUuid, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);

    deviceInfoService
        ->createCharacteristic(manufacturerNameSvcUuid, NIMBLE_PROPERTY::READ)
        ->setValue("ZOCO BODY FIT");
    deviceInfoService
        ->createCharacteristic(modelNumberSvcUuid, NIMBLE_PROPERTY::READ)
        ->setValue("AR-C2");
    deviceInfoService
        ->createCharacteristic(serialNumberSvcUuid, NIMBLE_PROPERTY::READ)
        ->setValue("20220104");
    deviceInfoService
        ->createCharacteristic(softwareNumberSvcUuid, NIMBLE_PROPERTY::READ)
        ->setValue("0.1.0");

    Log.verboseln("Starting BLE Service");

    batteryService->start();
    measurementService->start();
    deviceInfoService->start();
    server->start();
}

NimBLEService *BluetoothService::setupCscServices(NimBLEServer *server)
{
    Log.infoln("Setting up Cycling Speed and Cadence Profile");

    auto cscService = server->createService(cyclingSpeedCadenceSvcUuid);
    cscMeasurementCharacteristic = cscService->createCharacteristic(cscMeasurementUuid, NIMBLE_PROPERTY::NOTIFY);

    dragFactorCharacteristic = cscService->createCharacteristic(dragFactorUuid, NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ);

    cscService
        ->createCharacteristic(cscFeatureUuid, NIMBLE_PROPERTY::READ)
        ->setValue((uint8_t *)&cscFeaturesFlag, 2);

    cscService
        ->createCharacteristic(sensorLocationUuid, NIMBLE_PROPERTY::READ)
        ->setValue(&sensorLocationFlag, 1);

    cscService->createCharacteristic(cscControlPointUuid, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::INDICATE)->setCallbacks(&controlPointCallbacks);

    return cscService;
}

NimBLEService *BluetoothService::setupPscServices(NimBLEServer *server)
{
    Log.infoln("Setting up Cycling Power Profile");
    auto pscService = server->createService(cyclingPowerSvcUuid);
    pscMeasurementCharacteristic = pscService->createCharacteristic(pscMeasurementUuid, NIMBLE_PROPERTY::NOTIFY);

    dragFactorCharacteristic = pscService->createCharacteristic(dragFactorUuid, NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ);

    pscService
        ->createCharacteristic(pscFeatureUuid, NIMBLE_PROPERTY::READ)
        ->setValue((uint8_t *)&pscFeaturesFlag, 4);

    pscService
        ->createCharacteristic(sensorLocationUuid, NIMBLE_PROPERTY::READ)
        ->setValue(&sensorLocationFlag, 1);

    pscService->createCharacteristic(pscControlPointUuid, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::INDICATE)->setCallbacks(&controlPointCallbacks);

    return pscService;
}

void BluetoothService::setupAdvertisement() const
{
    auto pAdvertising = NimBLEDevice::getAdvertising();
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
}

void BluetoothService::setupConnectionIndicatorLed() const
{
    pinMode(GPIO_NUM_2, OUTPUT);
}