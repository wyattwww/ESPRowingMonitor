#include <ctime>
#include <string>
#include <vector>

#include "Arduino.h"
#include "ArduinoLog.h"
#include "LittleFS.h"

#include "network.service.h"

using std::string;
using std::to_string;
using std::vector;

NetworkService::NetworkService(EEPROMService &_eepromService) : eepromService(_eepromService), server(Configurations::port), webSocket("/ws") {}

void NetworkService::update()
{
    auto const now = millis();
    auto const cleanupInterval = 5000;
    if (now - lastCleanupTime > cleanupInterval)
    {
        lastCleanupTime = now;
        webSocket.cleanupClients(2);
    }

    if (!isServerStarted && WiFiClass::status() == WL_CONNECTED)
    {
        Log.infoln("Connected to the WiFi network");
        Log.infoln("Local ESP32 IP:  %p", WiFi.localIP());

        webSocket.onEvent([&](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
                          {
            switch (type)
            {
            case WS_EVT_CONNECT:
                Log.traceln("WebSocket client #%u connected from %p", client->id(), client->remoteIP());
                break;
            case WS_EVT_DISCONNECT:
                Log.traceln("WebSocket client #%u disconnected", client->id());
                break;
            case WS_EVT_DATA:
                handleWebSocketMessage(arg, data, len);
                break;
            case WS_EVT_PONG:
            case WS_EVT_ERROR:
                break;
            } });

        server.addHandler(&webSocket);

        if constexpr (Configurations::isWebGUIEnabled)
        {
            if (LittleFS.begin())
            {
                Log.traceln("Serving up static Web GUI page");
                auto const lastModified = LittleFS.open("/www/index.html").getLastWrite();
                string formattedDate = "Thu, 01 Jan 1970 00:00:00 GMT";
                std::strftime(formattedDate.data(), 29, "%a, %d %b %Y %H:%M:%S GMT", std::localtime(&lastModified));
                server.serveStatic("/", LittleFS, "/www/")
                    .setLastModified(formattedDate.c_str())
                    .setDefaultFile("index.html");
            }
        }
        server.begin();
        isServerStarted = true;
    }
}

void NetworkService::setup()
{
    auto const deviceName = Configurations::deviceName + "-(" + string(eepromService.getBleServiceFlag() == BleServiceFlag::CscService ? "CSC)" : "CPS)");
    WiFi.setHostname(deviceName.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.onEvent([&](WiFiEvent_t event, WiFiEventInfo_t info)
                 {
        	Log.traceln("Wifi disconnected, trying to reconnect");
            WiFi.reconnect(); },
                 WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.begin(Configurations::ssid.c_str(), Configurations::passphrase.c_str());
    Log.infoln("Connecting to wifi: %s", Configurations::ssid.c_str());

    auto connectionTimeout = 1;
    Log.infoln(".");
    while (WiFiClass::status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);

        if (connectionTimeout == 0)
        {
            Serial.print("\n");
            Log.infoln("Was not able to connect to the Wifi retrying in the background");
            break;
        }

        connectionTimeout--;
    }
    Serial.print("\n");
}

void NetworkService::stopServer()
{
    Log.traceln("Stopping web server and Wifi");
    webSocket.closeAll();
    server.end();
    WiFi.disconnect(true);
}

bool NetworkService::isAnyDeviceConnected() const
{
    return isServerStarted && webSocket.count() > 0;
}

void NetworkService::notifyClients(const RowingDataModels::RowingMetrics rowingMetrics, 
                                    const unsigned char batteryLevel, 
                                    const BleServiceFlag bleServiceFlag, 
                                    const ArduinoLogLevel logLevel, 
                                    const float flywheelInertia, 
                                    const bool autoDragFactor,
                                    const int dragFactor,
                                    const float magicNumber)
{
    string response;
    response.append("{\"batteryLevel\":" + to_string(batteryLevel));
    response.append(",\"bleServiceFlag\":" + to_string(static_cast<unsigned char>(bleServiceFlag)));
    response.append(",\"logLevel\":" + to_string(static_cast<unsigned char>(logLevel)));
    response.append(",\"magicNumber\":" + to_string(magicNumber));
    response.append(",\"flywheelInertia\":" + to_string(flywheelInertia));
    response.append(",\"autoDragFactor\":" + to_string(autoDragFactor));
    response.append(",\"dragFactor\":" + to_string(dragFactor));
    response.append(",\"revTime\":" + to_string(rowingMetrics.lastRevTime));
    response.append(",\"elapsedTime\":" + to_string(rowingMetrics.elapsedTime));
    response.append(",\"distance\":" + to_string(rowingMetrics.distance));
    response.append(",\"strokeTime\":" + to_string(rowingMetrics.lastStrokeTime));
    response.append(",\"strokeCount\":" + to_string(rowingMetrics.strokeCount));
    response.append(",\"avgStrokePower\":" + to_string(rowingMetrics.avgStrokePower));
    response.append(",\"driveDuration\":" + to_string(rowingMetrics.driveDuration));
    response.append(",\"recoveryDuration\":" + to_string(rowingMetrics.recoveryDuration));
    response.append(",\"dragFactor\":" + to_string(rowingMetrics.dragCoefficient * 1e6));
    response.append(",\"totalCalories\":" + to_string(rowingMetrics.totalCalories));
    response.append(",\"handleForces\": [");

    for (auto const &handleForce : rowingMetrics.driveHandleForces)
    {
        response.append(to_string(handleForce) + ",");
    }

    if (rowingMetrics.driveHandleForces.size() > 0)
    {
        response.pop_back();
    }
    response.append("]}");

    webSocket.textAll(response.c_str());
}

void NetworkService::handleWebSocketMessage(const void *const arg, uint8_t *const data, const size_t len) const
{
    auto const *const info = static_cast<const AwsFrameInfo *>(arg);
    if (static_cast<bool>(info->final) && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        // NOLINTBEGIN
        data[len] = 0;
        string const request(reinterpret_cast<char *>(data));
        // NOLINTEND

        if (request.size() < 3)
        {
            Log.traceln("Invalid request size: %s", request.c_str());
        }

        auto const requestOpCommand = request.substr(1, request.size() - 2);

        //Log.infoln("Incoming WS message");

        auto const opCommand = parseOpCode(requestOpCommand);

        if (opCommand.size() != 2)
        {
            Log.traceln("Invalid request opCommand size: %d, %s", opCommand.size(), requestOpCommand.c_str());
            return;
        }
        /*else {
            Log.traceln("Request opCommand size: %d, %s", opCommand.size(), requestOpCommand.c_str());
        }*/

        //Log.infoln("Op Code: %d; Length: %d", opCommand[0], opCommand.size());

        int opCommandCode = std::stoi(opCommand[0]);

        switch (opCommandCode)
        {
        case static_cast<int>(PSCOpCodes::SetLogLevel):
        {
            Log.infoln("Set LogLevel OpCode");

            int opCommandVal = std::stoi(opCommand[1]);

            if (opCommand.size() == 2 && opCommandVal >= 0 && opCommandVal <= 6)
            {
                Log.infoln("New LogLevel: %d", opCommandVal);
                eepromService.setLogLevel(static_cast<ArduinoLogLevel>(opCommandVal));

                return;
            }

            Log.infoln("Invalid log level: %d", opCommandVal);
        }
        break;

        case static_cast<int>(PSCOpCodes::ChangeBleService):
        {
            Log.infoln("Change BLE Service");

            int opCommandVal = std::stoi(opCommand[1]);

            if (opCommand.size() == 2 && opCommandVal >= 0 && opCommandVal <= 2)
            {
                if( opCommandVal == static_cast<unsigned char>(BleServiceFlag::CscService) ) {
                    Log.infoln("New BLE Service: %s", "CSC");
                }
                else if( opCommandVal == static_cast<unsigned char>(BleServiceFlag::CpsService) ) {
                    Log.infoln("New BLE Service: %s", "CPS");
                }
                else if( opCommandVal == static_cast<unsigned char>(BleServiceFlag::FtmsService) ) {
                    Log.infoln("New BLE Service: %s", "FTMS");
                }                
                
                eepromService.setBleServiceFlag(static_cast<BleServiceFlag>(opCommandVal));

                Log.verboseln("Restarting device");
                Serial.flush();
                esp_sleep_enable_timer_wakeup(1);
                esp_deep_sleep_start();

                return;
            }

            Log.infoln("Invalid BLE service flag: %d", opCommand[1]);
        }
        break;

        case static_cast<int>(PSCOpCodes::ChangeMagicNumber):
        {
            Log.infoln("Change Magic Number");

            float opCommandVal = std::stof(opCommand[1]);

            if (opCommand.size() == 2 && opCommandVal > 1 && opCommandVal < 4)
            {
                Log.traceln("New magic num: %D", opCommandVal);

                eepromService.setMagicNumber(opCommandVal);
                //Log.verboseln("Restarting device");
                //Serial.flush();
                //esp_sleep_enable_timer_wakeup(1);
                //esp_deep_sleep_start();

                return;
            }

            Log.infoln("Invalid magic number: %D", opCommand[1]);
        }
        break;

        case static_cast<int>(PSCOpCodes::ChangeInertia):
        {
            Log.infoln("Change Inertia");

            float opCommandVal = std::stof(opCommand[1]);

            if (opCommand.size() == 2 && opCommandVal >= 0 && opCommandVal <= 1)
            {
                Log.traceln("New inertia: %D", opCommandVal);

                eepromService.setFlywheelInertia(opCommandVal);
                //Log.verboseln("Restarting device");
                //Serial.flush();
                //esp_sleep_enable_timer_wakeup(1);
                //esp_deep_sleep_start();

                return;
            }

            Log.infoln("Invalid flywheel inertia: %D", opCommand[1]);
        }
        break;

        case static_cast<int>(PSCOpCodes::ChangeDragFactor):
        {
            Log.infoln("Change Drag Factor");

            int opCommandVal = std::stoi(opCommand[1]);

            if (opCommand.size() == 2 && opCommandVal >= LOWER_DRAG_FACTOR_THRESHOLD && opCommandVal <= UPPER_DRAG_FACTOR_THRESHOLD)
            {
                Log.traceln("New drag factor: %d", opCommandVal);

                eepromService.setDragFactor(opCommandVal);
                //Log.verboseln("Restarting device");
                //Serial.flush();
                //esp_sleep_enable_timer_wakeup(1);
                //esp_deep_sleep_start();

                return;
            }

            Log.infoln("Invalid flywheel inertia: %D", opCommand[1]);
        }
        break;

        case static_cast<int>(PSCOpCodes::ChangeAutoDragFactor):
        {
            Log.infoln("Change Auto Drag Factor");

            int opCommandVal = std::stoi(opCommand[1]);

            if (opCommand.size() == 2)
            {
                Log.traceln("New auto drag factor: %d", opCommandVal);

                eepromService.setAutoDragFactor(opCommandVal);
                //Log.verboseln("Restarting device");
                //Serial.flush();
                //esp_sleep_enable_timer_wakeup(1);
                //esp_deep_sleep_start();

                return;
            }

            Log.infoln("Invalid flywheel inertia: %D", opCommand[1]);
        }
        break;

        default:
        {
            Log.infoln("Not Supported Op Code: %d", opCommand[0]);
        }
        break;
        }
    }
}

vector<string> NetworkService::parseOpCode(string requestOpCommand)
{
    auto opCommand = vector<string>();

    string parsed;
    while (!requestOpCommand.empty())
    {
        auto position = requestOpCommand.find(",");
        if (position == string::npos)
        {
            position = requestOpCommand.size();
        }
        parsed = requestOpCommand.substr(0, position);
        //Log.traceln("Parsed opCode: %s", parsed.c_str());

        replaceInPlace(parsed, "\"", "");
        requestOpCommand.erase(0, position + 1);
        if (std::any_of(parsed.begin(), parsed.end(), [](unsigned char character)
                        { return !(bool)std::isdigit(character) && !(bool)std::ispunct(character); }))
        {
            Log.traceln("Invalid opCode: %s", parsed.c_str());
            return vector<string>();
        }
        
        opCommand.push_back(parsed);
    }
    return opCommand;
}

bool NetworkService::replaceInPlace( std::string& str, std::string const& replaceThis, std::string const& withThis ) {
    bool replaced = false;
    std::size_t i = str.find( replaceThis );
    while( i != std::string::npos ) {
        replaced = true;
        str = str.substr( 0, i ) + withThis + str.substr( i+replaceThis.size() );
        if( i < str.size()-withThis.size() )
            i = str.find( replaceThis, i+withThis.size() );
        else
            i = std::string::npos;
    }
    return replaced;
}