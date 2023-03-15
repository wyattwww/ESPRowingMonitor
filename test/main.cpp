#include <fstream>
#include <numeric>
#include <span>
#include <string>

#include "ArduinoLog.h"

#include "globals.h"
#include "test.array.h"

using std::string;

void loop(unsigned long now)
{
    simulateRotation(now);
    strokeController.update();
    if (strokeController.getRevCount() != strokeController.getPreviousRevCount())
    {
        Log.infoln("distance: %f", strokeController.getDistance() / 100.0);
        Log.infoln("revCount: %d", strokeController.getRevCount());
        strokeController.setPreviousRevCount();
    }

    if (strokeController.getStrokeCount() > strokeController.getPreviousStrokeCount())
    {
        Log.infoln("driveDuration: %f", strokeController.getDriveDuration());
        Log.infoln("recoveryDuration: %f", strokeController.getRecoveryDuration());
        Log.infoln("dragFactor: %d", strokeController.getDragFactor());
        Log.infoln("power: %d", strokeController.getAvgStrokePower());
        strokeController.setPreviousStrokeCount();
    }
}

int main(int argc, const char *argv[])
{
    auto args = std::span(argv, size_t(argc));

    unsigned long now = 0;
    if (argc < 2 || string(args[1]).empty())
    {
        for (auto &deltaTime : testDeltaTimes)
        {
            now += deltaTime;
            loop(now);
        }

        return 0;
    }

    unsigned long deltaTime = 0;

    std::ifstream deltaTimeStream(args[1]);
    printf("Running external file: %s\n", args[1]);

    while (deltaTimeStream >> deltaTime)
    {
        now += deltaTime;
        loop(now);
    }

    return 0;
}
