#include "GT7DerivedMetrics.h"
#include <math.h>

void GT7AbsEstimator::update(float vehicleKmh,float brakePercent,
                             const float wheelAngularSpeed[4],
                             const float tyreRadius[4],
                             uint32_t now)
{
    if(vehicleKmh < minSpeedKmh || brakePercent < minBrakePercent){
        active = (now-lastTriggerTime)<sustainMs;
        return;
    }

    float maxSlip=0;
    for(int i=0;i<4;i++){
        float wheelKmh=fabs(wheelAngularSpeed[i])*tyreRadius[i]*3.6f;
        float slip=vehicleKmh-wheelKmh;
        if(slip>maxSlip) maxSlip=slip;
    }

    if(maxSlip>triggerDiffKmh){
        lastTriggerTime=now;
    }

    active=(maxSlip>triggerDiffKmh)||((now-lastTriggerTime)<sustainMs);
}

bool GT7AbsEstimator::isActive() const { return active; }

float GT7AbsEstimator::intensity(uint32_t now) const{
    if(!active) return 0;
    return 70.0f+sinf(now/20.0f)*20.0f;
}

void GT7FuelEstimator::reset(){
    lapStartFuel=-1.0f;
    lastFuel=-1.0f;
    lastLap=-1;
    avgFuelPerLap=0.0f;
    remaining=-1.0f;
    validSamples=0;
}

void GT7FuelEstimator::update(float fuelLevel,int currentLap){
    if(currentLap < 0 || fuelLevel < 0.0f) return;

    if(lastLap < 0){
        lastLap=currentLap;
        lapStartFuel=fuelLevel;
        lastFuel=fuelLevel;
        return;
    }

    if(currentLap < lastLap){
        reset();
        lastLap=currentLap;
        lapStartFuel=fuelLevel;
        lastFuel=fuelLevel;
        return;
    }

    // Pit refuel during the same lap: restart the baseline.
    if(fuelLevel > lastFuel + 0.5f){
        lapStartFuel=fuelLevel;
    }

    if(currentLap > lastLap){
        const float used=lapStartFuel-fuelLevel;
        if(used > 0.01f){
            if(validSamples == 0) avgFuelPerLap=used;
            else avgFuelPerLap=avgFuelPerLap*0.7f+used*0.3f;
            ++validSamples;
        }
        lastLap=currentLap;
        lapStartFuel=fuelLevel;
    }

    lastFuel=fuelLevel;
    remaining=(avgFuelPerLap > 0.01f) ? fuelLevel/avgFuelPerLap : -1.0f;
}

float GT7FuelEstimator::remainingLaps() const{return remaining;}

void GT7DeltaTracker::reset(){currentDelta=0;projectedDelta=0;}
void GT7DeltaTracker::beginLap(){}
void GT7DeltaTracker::sample(float,float,float,uint32_t){}
void GT7DeltaTracker::endLap(uint32_t){}
float GT7DeltaTracker::delta() const{return currentDelta;}
float GT7DeltaTracker::deltaP() const{return projectedDelta;}

void GT7DerivedMetrics::reset(){
    fuel.reset();
    delta.reset();
}
