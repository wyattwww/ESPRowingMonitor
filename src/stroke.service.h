#pragma once

#include <array>

#include "stroke.model.h"
#include "settings.h"

enum class CyclePhase
{
    Stopped,
    Recovery,
    Drive
};

class StrokeService
{
    static double constexpr ANGULAR_DISPLACEMENT_PER_IMPULSE = (2 * PI) / Settings::IMPULSES_PER_REVOLUTION ;

    volatile unsigned long lastRevTime = 0;
    volatile unsigned long lastStrokeTime = 0;
    volatile unsigned int impulseCount = 0;
    volatile unsigned int revCount = 0;
    volatile unsigned short strokeCount = 0;

    volatile double avgStrokePower = 0;
    volatile double dragCoefficient = 0;
    volatile unsigned int lastDriveDuration = 0;

    volatile unsigned long driveStartTime = 0;
    volatile unsigned int driveStartImpulseCount = 0;
    volatile unsigned int driveDuration = 0;

    volatile double recoveryStartAngularVelocity = 0.0;
    volatile unsigned long recoveryStartTime = 0;
    volatile unsigned int recoveryStartImpulseCount = 0;
    volatile unsigned int recoveryDuration = 0;

    volatile unsigned long previousDeltaTime = 0;
    volatile unsigned long previousRawRevTime = 0;
    volatile unsigned long lastDataReadTime = 0;

    volatile CyclePhase cyclePhase = CyclePhase::Stopped;
    std::array<volatile unsigned long, Settings::DELTA_TIME_ARRAY_LENGTH> cleanDeltaTimes{};
    std::array<double, Settings::DRAG_COEFFICIENTS_ARRAY_LENGTH> dragCoefficients{};

    bool isFlywheelUnpowered() const;
    bool isFlywheelPowered() const;
    void calculateDragCoefficient();
    void calculateAvgStrokePower();

public:
    StrokeService();

    void setup() const;
    bool hasDataChanged();
    StrokeModel::CscData getData() const;
    void processRotation(unsigned long now);
};