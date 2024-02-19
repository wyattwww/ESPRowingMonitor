#include "settings.h"

#include <string>
#include <ElegantOTA.h>

#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"

#include "../rower/stroke.model.h"
#include "../utils/EEPROM.service.h"
#include "../utils/configuration.h"
#include "../utils/enums.h"

class NetworkService
{
    EEPROMService &eepromService;

    AsyncWebServer server;
    AsyncWebSocket webSocket;

    AsyncWebServer otaServer;

    unsigned long lastCleanupTime = 0UL;

    void handleWebSocketMessage(const void *arg, uint8_t *data, size_t len) const;
    static std::vector<string> parseOpCode(std::string requestOpCommand);
    static bool replaceInPlace( std::string& str, std::string const& replaceThis, std::string const& withThis );

public:
    static bool isServerStarted;

    explicit NetworkService(EEPROMService &_eepromService);
    void setup();
    void update();
    void stopServer();

    void notifyClients(const RowingDataModels::RowingMetrics rowingMetrics, 
                        unsigned char batteryLevel, 
                        BleServiceFlag bleServiceFlag, 
                        ArduinoLogLevel logLevel, 
                        const float flywheelInertia,
                        const bool autoDragFactor,
                        const int dragFactor,
                        const float magicNumber);
    bool isAnyDeviceConnected() const;
};