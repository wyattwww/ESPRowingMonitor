#pragma once

#include <array>

#include "../utils/configuration.h"
#include "../utils/ols-linear-series.h"
#include "../utils/series.h"
#include "../utils/ts-quadratic-series.h"
#include "stroke.model.h"

class StrokeService
{
    static Configurations::precision constexpr angularDisplacementPerImpulse = (2 * PI) / Configurations::impulsesPerRevolution;
    static Configurations::precision constexpr sprocketRadius = Configurations::sprocketRadius / 100;
    static const unsigned char driveHandleForcesMaxCapacity = UCHAR_MAX;

    // rower state
    CyclePhase cyclePhase = CyclePhase::Stopped;
    unsigned long long rowingTotalTime = 0ULL;
    unsigned long long rowingImpulseCount = 0UL;
    Configurations::precision rowingTotalAngularDisplacement = 0;

    // Drive related
    unsigned long long driveStartTime = 0ULL;
    unsigned int driveDuration = 0;
    Configurations::precision driveStartAngularDisplacement = 0;
    Configurations::precision driveTotalAngularDisplacement = 0;

    // Recovery related
    unsigned long long recoveryStartTime = 0;
    unsigned int recoveryDuration = 0;
    Configurations::precision recoveryStartAngularDisplacement = 0;
    Configurations::precision recoveryTotalAngularDisplacement = 0;
    Configurations::precision recoveryStartDistance = 0;

    // metrics
    Configurations::precision distancePerAngularDisplacement = 0;
    Configurations::precision distance = 0;
    unsigned short strokeCount = 0;
    unsigned long long strokeTime = 0ULL;
    unsigned long long revTime = 0ULL;
    Configurations::precision avgStrokePower = 0;

    std::array<Configurations::precision, Configurations::dragCoefficientsArrayLength> dragCoefficients{};

    Configurations::precision totalAngularDisplacement = 0;

    unsigned long long startTime = 0;
    unsigned int strokeCalories = 0;
    unsigned int totalCalories = 0;
    unsigned int totalCaloriesPerHour = 0;
    unsigned int totalCaloriesPerMinute = 0;


    // advance metrics
    Configurations::precision currentAngularVelocity = 0;
    Configurations::precision currentAngularAcceleration = 0;
    Configurations::precision currentTorque = 0;
    vector<Configurations::precision> driveHandleForces{};

    vector<Series> angularVelocityMatrix{};
    vector<Series> angularAccelerationMatrix{};

    OLSLinearSeries deltaTimes = OLSLinearSeries(Configurations::impulseDataArrayLength);
    OLSLinearSeries deltaTimesSlopes = OLSLinearSeries(Configurations::impulseDataArrayLength);
    OLSLinearSeries recoveryDeltaTimes = OLSLinearSeries();
    TSQuadraticSeries angularDistances = TSQuadraticSeries(Configurations::impulseDataArrayLength);

    Series cyclePower = Series(Configurations::numOfPhasesForAveragingScreenData);
    Series cycleDuration = Series(Configurations::numOfPhasesForAveragingScreenData);
    unsigned long long calories = 0;

    bool isFlywheelUnpowered() const;
    bool isFlywheelPowered() const;
    void calculateDragCoefficient();
    void calculateAvgStrokePower();
    void startHallSensor() const;

    void driveStart();
    void driveUpdate();
    void driveEnd();
    void recoveryStart();
    void recoveryUpdate();
    void recoveryEnd();
    
    unsigned long long getElapsedTime();

    Configurations::precision lastDistance = 0;
    int lastRevTime = 0;

public:
    StrokeService();

    Configurations::precision dragCoefficient = 101.0 / 1e6;

    RowingDataModels::RowingMetrics getData();
    void processData(RowingDataModels::FlywheelData data);
    unsigned int getTotalCalories();
    void resetStartTime();
};