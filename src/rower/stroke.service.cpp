#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>

#include "Arduino.h"
#include "ArduinoLog.h"

#include "globals.h"
#include "stroke.service.h"

using std::accumulate;
using std::any_of;
using std::array;
using std::minmax;
using std::partial_sort_copy;

using RowingDataModels::RowingMetrics;

StrokeService::StrokeService()
{
    angularVelocityMatrix.reserve(Configurations::impulseDataArrayLength);
    angularAccelerationMatrix.reserve(Configurations::impulseDataArrayLength);

    deltaTimes.push(0, 0);
    angularDistances.push(0, 0);
}

bool StrokeService::isFlywheelUnpowered() const
{
    if constexpr (Configurations::strokeDetectionType != StrokeDetectionType::Slope)
    {
        if (currentTorque < Configurations::minimumDragTorque || (deltaTimesSlopes.size() >= Configurations::impulseDataArrayLength && std::abs(deltaTimesSlopes.slope()) < Configurations::minimumRecoverySlopeMargin))
        {
            return true;
        }
    }

    if constexpr (Configurations::strokeDetectionType != StrokeDetectionType::Torque)
    {
        if (deltaTimes.slope() > Configurations::minimumRecoverySlope)
        {
            return true;
        }
    }

    return false;
}

bool StrokeService::isFlywheelPowered() const
{
    return currentTorque > Configurations::minimumPoweredTorque && deltaTimes.slope() < 0;
}

void StrokeService::calculateDragCoefficient()
{
    if( !eepromService.isAutoDragFactor() ) 
    {
        dragCoefficient = eepromService.getDragFactor() / 1e6;
        return;
    }
    
    if (recoveryDuration > Configurations::maxDragFactorRecoveryPeriod || recoveryDeltaTimes.size() < Configurations::impulseDataArrayLength)
    {
        //Log.traceln("calculateDragCoefficient ret 1");
        return;
    }

    if (recoveryDeltaTimes.goodnessOfFit() < Configurations::goodnessOfFitThreshold)
    {
        //Log.traceln("calculateDragCoefficient ret 2, fit: %D", recoveryDeltaTimes.goodnessOfFit());
        return;
    }

    auto rawNewDragCoefficient = (recoveryDeltaTimes.slope() * eepromService.getFlywheerInertia()) / angularDisplacementPerImpulse;

    if (rawNewDragCoefficient > Configurations::upperDragFactorThreshold ||
        rawNewDragCoefficient < Configurations::lowerDragFactorThreshold)
    {
        //Log.traceln("calculateDragCoefficient ret 3, rawNewDragCoefficient: %D", rawNewDragCoefficient);
        return;
    }

    //Log.traceln("calculateDragCoefficient ret 4, rawNewDragCoefficient: %D", rawNewDragCoefficient);

    if (Configurations::dragCoefficientsArrayLength > 1)
    {
        char i = Configurations::dragCoefficientsArrayLength - 1;
        while (i > 0)
        {
            dragCoefficients[i] = dragCoefficients[i - 1];
            i--;
        }
        dragCoefficients[0] = rawNewDragCoefficient;

        array<Configurations::precision, Configurations::dragCoefficientsArrayLength> sortedArray{};

        partial_sort_copy(dragCoefficients.cbegin(), dragCoefficients.cend(), sortedArray.begin(), sortedArray.end());
        rawNewDragCoefficient = sortedArray[sortedArray.size() / 2];
    }

    dragCoefficient = rawNewDragCoefficient;
}

void StrokeService::calculateAvgStrokePower()
{
    avgStrokePower = dragCoefficient * pow((recoveryTotalAngularDisplacement + driveTotalAngularDisplacement) / ((driveDuration + recoveryDuration) / 1e6), 3);
}

unsigned long long StrokeService::getElapsedTime() {
    if( startTime == 0 ) {
        //Log.traceln("Not started yet");
        return 0;
    }
    unsigned long long elapsed = (millis()-startTime);
    //Log.traceln("Elapsed:  %u", elapsed);
    return elapsed / 1e3L;
}

void StrokeService::resetStartTime() 
{
    startTime = 0;
}

void StrokeService::driveStart()
{
    if( startTime == 0 ) {
        startTime = millis();
    }
    cyclePhase = CyclePhase::Drive;
    driveStartTime = rowingTotalTime;
    driveStartAngularDisplacement = rowingTotalAngularDisplacement;

    driveHandleForces.clear();
    driveHandleForces.push_back(currentTorque / sprocketRadius);

    if constexpr (Configurations::strokeDetectionType != StrokeDetectionType::Slope)
    {
        deltaTimesSlopes.reset();
        deltaTimesSlopes.push(rowingTotalTime, deltaTimes.slope());
    }
}

void StrokeService::driveUpdate()
{
    if (driveHandleForces.size() > driveHandleForcesMaxCapacity)
    {
        driveHandleForces.clear();
        Log.warningln("driveHandleForces variable data point size exceeded max capacity indicating an extremely long drive phase. With plausible stroke detection settings this should not happen. Resetting variable to avoid crash...");
    }
    driveHandleForces.push_back(currentTorque / sprocketRadius);
    if constexpr (Configurations::strokeDetectionType != StrokeDetectionType::Slope)
    {
        deltaTimesSlopes.push(rowingTotalTime, deltaTimes.slope());
    }
}

void StrokeService::driveEnd()
{
    // It seems that we lost power to the flywheel lets check if drive time was sufficient for detecting a stroke (i.e. drivePhaseDuration exceeds debounce time). So we can conclude the "Drive" phase as there is no more drive detected to the flywheel (e.g. for calculating power etc.)

    driveDuration = rowingTotalTime - driveStartTime;
    driveTotalAngularDisplacement = rowingTotalAngularDisplacement - driveStartAngularDisplacement;
    strokeCount++;
    strokeTime = rowingTotalTime;
}

void StrokeService::recoveryStart()
{
    cyclePhase = CyclePhase::Recovery;
    recoveryStartTime = rowingTotalTime;
    recoveryStartAngularDisplacement = rowingTotalAngularDisplacement;
    recoveryStartDistance = distance;
    recoveryDeltaTimes.reset();
    recoveryDeltaTimes.push(rowingTotalTime, deltaTimes.yAtSeriesBegin());
}

void StrokeService::recoveryUpdate()
{
    recoveryDeltaTimes.push(rowingTotalTime, deltaTimes.yAtSeriesBegin());
}

void StrokeService::recoveryEnd()
{
    recoveryDuration = rowingTotalTime - recoveryStartTime;
    recoveryTotalAngularDisplacement = rowingTotalAngularDisplacement - recoveryStartAngularDisplacement;
    calculateDragCoefficient();
    calculateAvgStrokePower();

    distancePerAngularDisplacement = pow((dragCoefficient * 1e6) / eepromService.getMagicNumber(), 1 / 3.0);
    distance = recoveryStartDistance + distancePerAngularDisplacement * (distance == 0 ? rowingTotalAngularDisplacement : recoveryTotalAngularDisplacement);
    if (distance > 0)
    {
        revTime = rowingTotalTime;
    }

    auto const secInMicroSec = 1e6;
    auto const rowTimeInSec = rowingTotalTime/secInMicroSec;

    cycleDuration.push(driveDuration);
    cyclePower.push(avgStrokePower);

    // based on:   http://eodg.atm.ox.ac.uk/user/dudhia/rowing/physics/ergometer.html#section11
    if( cyclePower.size() >= Configurations::numOfPhasesForAveragingScreenData ) 
    {
        strokeCalories = (4 * cyclePower.median() + 350) * (cycleDuration.median()) / 4200;
        unsigned int totalCal = calories + strokeCalories;
        calories = totalCal;

        //Log.traceln("StrokeCal: %u, totalCal: %u", strokeCalories, totalCal);
    
        totalCalories = calories > 0 ? calories / secInMicroSec : 0; // kcal
        totalCaloriesPerMinute = 0;
        totalCaloriesPerHour = 0;
    }
    else {
        totalCalories = 0;
        totalCaloriesPerMinute = 0;
        totalCaloriesPerHour = 0;
    }
}

RowingDataModels::RowingMetrics StrokeService::getData()
{
    unsigned long long elapsed = getElapsedTime();
    Configurations::precision pace = 0;
    if( (revTime - lastRevTime)> 0 ) {
        pace = (distance - lastDistance) / 100 / ((revTime - lastRevTime) / 1e6),
        //Log.infoln("Data pace: %D", pace); 

        lastRevTime = revTime;
        lastDistance = distance;
    }

    return RowingDataModels::RowingMetrics{
        .distance = distance,
        .lastRevTime = revTime,
        .lastStrokeTime = strokeTime,
        .strokeCount = strokeCount,
        .driveDuration = driveDuration,
        .recoveryDuration = recoveryDuration,
        .avgStrokePower = avgStrokePower,
        .dragCoefficient = dragCoefficient,
        .driveHandleForces = driveHandleForces,
        .elapsedTime = elapsed,
        .totalCalories = totalCalories,
        .totalCaloriesPerHour = totalCaloriesPerHour,
        .totalCaloriesPerMinute = totalCaloriesPerMinute,
        .pace = pace};
}

unsigned int StrokeService::getTotalCalories()
{
    return totalCalories;
}

void StrokeService::processData(const RowingDataModels::FlywheelData data)
{
    deltaTimes.push(data.totalTime, data.deltaTime);
    angularDistances.push(data.totalTime / 1e6, data.totalAngularDisplacement);

    if (angularVelocityMatrix.size() >= Configurations::impulseDataArrayLength)
    {
        angularVelocityMatrix.erase(angularVelocityMatrix.begin());
    }
    if (angularAccelerationMatrix.size() >= Configurations::impulseDataArrayLength)
    {
        angularAccelerationMatrix.erase(angularAccelerationMatrix.begin());
    }

    angularVelocityMatrix.push_back(Series(Configurations::impulseDataArrayLength));
    angularAccelerationMatrix.push_back(Series(Configurations::impulseDataArrayLength));

    unsigned char i = 0;
    while (i < angularVelocityMatrix.size())
    {
        angularVelocityMatrix[i].push(angularDistances.firstDerivativeAtPosition(i));
        angularAccelerationMatrix[i].push(angularDistances.secondDerivativeAtPosition(i));
        i++;
    }

    currentAngularVelocity = angularVelocityMatrix[0].median();
    currentAngularAcceleration = angularAccelerationMatrix[0].median();

    currentTorque = eepromService.getFlywheerInertia() * currentAngularAcceleration + dragCoefficient * pow(currentAngularVelocity, 2);

    // If rotation delta exceeds the max debounce time and we are in Recovery Phase, the rower must have stopped. Setting cyclePhase to "Stopped"
    if (cyclePhase == CyclePhase::Recovery && rowingTotalTime - recoveryStartTime > Configurations::rowingStoppedThresholdPeriod)
    {
        recoveryEnd();
        driveHandleForces.clear();
        cyclePhase = CyclePhase::Stopped;
        driveDuration = 0;
        avgStrokePower = 0;

        return;
    }

    if (cyclePhase == CyclePhase::Stopped)
    {
        // We are currently in the "Stopped" phase, as power was not applied for a long period of time or the device just started. Since rotation was detected we check if cleanDeltaTimes array is filled (i.e. whether we have sufficient data for determining the next phase) and whether power is being applied to the flywheel
        if (
            deltaTimes.size() < Configurations::impulseDataArrayLength || !isFlywheelPowered())
        {
            return;
        }

        rowingImpulseCount++;
        rowingTotalTime += deltaTimes.yAtSeriesBegin();
        revTime = rowingTotalTime;
        rowingTotalAngularDisplacement += angularDisplacementPerImpulse;

        // Since we detected power, setting to "Drive" phase and increasing rotation count and registering rotation time
        driveStart();

        return;
    }

    rowingImpulseCount++;
    rowingTotalTime += deltaTimes.yAtSeriesBegin();
    rowingTotalAngularDisplacement += angularDisplacementPerImpulse;

    distance += distancePerAngularDisplacement * (distance == 0 ? rowingTotalAngularDisplacement : angularDisplacementPerImpulse);
    if (distance > 0)
    {
        revTime = rowingTotalTime;
    }
    // we implement a finite state machine that goes between "Drive" and "Recovery" phases while paddling on the machine. This allows a phase-change if sufficient time has passed and there is a plausible flank
    if (cyclePhase == CyclePhase::Drive)
    {
        // We are currently in the "Drive" phase, lets determine what the next phase is (if we come from "Stopped" phase )
        if (rowingTotalTime - driveStartTime > Configurations::strokeDebounceTime && isFlywheelUnpowered())
        {
            driveEnd();
            recoveryStart();

            return;
        }

        driveUpdate();

        return;
    }

    if (cyclePhase == CyclePhase::Recovery)
    {
        // We are currently in the "Recovery" phase, lets determine what the next phase is
        if (rowingTotalTime - recoveryStartTime > Configurations::strokeDebounceTime && isFlywheelPowered())
        {
            // Here we can conclude the "Recovery" phase (and the current stroke cycle) as drive to the flywheel is detected (e.g. calculating drag factor)
            recoveryEnd();
            driveStart();
            return;
        }

        recoveryUpdate();
        return;
    }
}