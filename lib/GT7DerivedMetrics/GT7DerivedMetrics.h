#pragma once
#include <Arduino.h>

struct GT7MetricsResult {
    float deltaSeconds = 0.0f;
    float deltaPSeconds = 0.0f;
    float remainingFuelLaps = 0.0f;
    bool absActive = false;
};

class GT7AbsEstimator {
public:
    void update(float vehicleKmh,
                float brakePercent,
                const float wheelAngularSpeed[4],
                const float tyreRadius[4],
                uint32_t now);

    bool isActive() const;
    float intensity(uint32_t now) const;

private:
    uint32_t lastTriggerTime = 0;
    bool active = false;

    static constexpr float triggerDiffKmh = 7.0f;
    static constexpr float minSpeedKmh = 15.0f;
    static constexpr float minBrakePercent = 10.0f;
    static constexpr uint32_t sustainMs = 200;
};

class GT7FuelEstimator {
public:
    void reset();
    void update(float fuelPercent, int currentLap);
    float remainingLaps() const;

private:
    float lapStartFuel = -1.0f;
    float lastFuel = -1.0f;
    int lastLap = -1;
    float avgFuelPerLap = 0.0f;
    float remaining = -1.0f;
    uint16_t validSamples = 0;
};

class GT7DeltaTracker {
public:
    void reset();
    void beginLap();
    void sample(float x,float y,float z,uint32_t currentLapMs);
    void endLap(uint32_t lapTimeMs);

    float delta() const;
    float deltaP() const;

private:
    float currentDelta = 0;
    float projectedDelta = 0;
};

class GT7DerivedMetrics {
public:
    void reset();

    GT7AbsEstimator abs;
    GT7FuelEstimator fuel;
    GT7DeltaTracker delta;
};
