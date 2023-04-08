#pragma once

#include "../settings.h"
#include "stroke.model.h"

class FlywheelService
{
    static Settings::precision constexpr angularDisplacementPerImpulse = (2 * PI) / Settings::impulsesPerRevolution;

    volatile unsigned long lastDeltaTime = 0;
    volatile unsigned long cleanDeltaTime = 0;
    volatile unsigned long lastRawImpulseTime = 0;
    volatile unsigned long lastCleanImpulseTime = 0;
    volatile Settings::precision totalAngularDisplacement = 0;

    volatile unsigned long impulseCount = 0UL;
    unsigned long long totalTime = 0ULL;

    volatile bool isDataChanged = false;

public:
    FlywheelService();

    static void setup();
    bool hasDataChanged() const;
    RowingDataModels::FlywheelData getData();
    void processRotation(unsigned long now);
};