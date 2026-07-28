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

void GT7FuelEstimator::update(float fuelLevel, int currentLap) {
    if (currentLap < 0 || fuelLevel < 0.0f) return;

    if (lastLap < 0) {
        lastLap = currentLap;
        lapStartFuel = fuelLevel;
        lastFuel = fuelLevel;
        return;
    }

    if (currentLap < lastLap) {
        reset();
        lastLap = currentLap;
        lapStartFuel = fuelLevel;
        lastFuel = fuelLevel;
        return;
    }

    // 同一圈內進站加油，重新設定該圈的油量基準
    if (fuelLevel > lastFuel + 0.5f) {
        lapStartFuel = fuelLevel;
    }

    if (currentLap > lastLap) {
        const float used = lapStartFuel - fuelLevel;

        // 略過起跑時 0 → 1 的假圈數切換
        // 也排除過小或明顯異常的油耗樣本
        const bool completedRealLap =
            lastLap >= 1 &&
            currentLap == lastLap + 1 &&
            used >= 0.2f &&
            used <= 50.0f;

        if (completedRealLap) {
            if (validSamples == 0) {
                avgFuelPerLap = used;
            } else {
                avgFuelPerLap =
                    avgFuelPerLap * 0.7f +
                    used * 0.3f;
            }

            ++validSamples;
        }

        lastLap = currentLap;
        lapStartFuel = fuelLevel;
    }

    lastFuel = fuelLevel;

    remaining =
        (validSamples > 0 && avgFuelPerLap >= 0.2f)
            ? fuelLevel / avgFuelPerLap
            : -1.0f;
}

float GT7FuelEstimator::remainingLaps() const {
    return remaining;
}

float GT7DeltaTracker::distanceSquared(
    float x1, float y1, float z1,
    float x2, float y2, float z2)
{
    const float dx = x1 - x2;
    const float dy = y1 - y2;
    const float dz = z1 - z2;

    return dx * dx + dy * dy + dz * dz;
}

int GT7DeltaTracker::findNearestReference(
    float x, float y, float z)
{
    if(referenceCount <= 0){
        return -1;
    }

    int bestIndex = lastReferenceIndex;
    float bestDistance = INFINITY;

    // 正常情況只往目前位置附近搜尋，
    // 避免髮夾彎或賽道交叉時跳到錯誤路段。
    int start = lastReferenceIndex - 5;
    int end = lastReferenceIndex + 30;

    if(start < 0) start = 0;
    if(end >= referenceCount) end = referenceCount - 1;

    for(int i = start; i <= end; ++i){
        const float d = distanceSquared(
            x, y, z,
            reference[i].x,
            reference[i].y,
            reference[i].z
        );

        if(d < bestDistance){
            bestDistance = d;
            bestIndex = i;
        }
    }

    // 區域搜尋結果距離超過 50 公尺，
    // 可能是剛開始、封包中斷或索引跑掉，改做全域搜尋。
    constexpr float GLOBAL_SEARCH_DISTANCE_SQ = 50.0f * 50.0f;

    if(bestDistance > GLOBAL_SEARCH_DISTANCE_SQ){
        bestDistance = INFINITY;

        for(int i = 0; i < referenceCount; ++i){
            const float d = distanceSquared(
                x, y, z,
                reference[i].x,
                reference[i].y,
                reference[i].z
            );

            if(d < bestDistance){
                bestDistance = d;
                bestIndex = i;
            }
        }
    }

    return bestIndex;
}

void GT7DeltaTracker::reset()
{
    referenceCount = 0;
    currentCount = 0;
    sampleCounter = 0;

    lastReferenceIndex = 0;
    previousReferenceIndex = -1;

    bestLapTime = 0;

    currentDelta = 0.0f;
    projectedDelta = 0.0f;

    previousDelta = 0.0f;
    deltaSlope = 0.0f;

    deltaValid = false;
}

void GT7DeltaTracker::beginLap()
{
    currentCount = 0;
    sampleCounter = 0;

    lastReferenceIndex = 0;
    previousReferenceIndex = -1;

    currentDelta = 0.0f;
    projectedDelta = 0.0f;

    previousDelta = 0.0f;
    deltaSlope = 0.0f;

    deltaValid = false;
}

void GT7DeltaTracker::sample(
    float x,
    float y,
    float z,
    uint32_t lapTime)
{
    // 不需要每個 UDP 封包都存。
    ++sampleCounter;

    if(sampleCounter < SAMPLE_DIVIDER){
        return;
    }

    sampleCounter = 0;

    if(currentCount < MAX_POINTS){
        DeltaPoint &point = current[currentCount];

        point.x = x;
        point.y = y;
        point.z = z;
        point.lapTime = lapTime;

        ++currentCount;
    }

    // 第一圈還沒有參考資料，只負責記錄。
    if(referenceCount <= 1){
        deltaValid = false;
        return;
    }

    const int referenceIndex = findNearestReference(x, y, z);

    if(referenceIndex < 0){
        deltaValid = false;
        return;
    }

    lastReferenceIndex = referenceIndex;

    currentDelta =
        static_cast<float>(
            static_cast<int32_t>(lapTime) -
            static_cast<int32_t>(
                reference[referenceIndex].lapTime
            )
        ) / 1000.0f;

    deltaValid = true;

    /*
     * Delta P：
     *
     * 觀察 Delta 在參考圈索引上的變化速度，
     * 再把趨勢外推到終點。
     */
    if(previousReferenceIndex >= 0 &&
       referenceIndex > previousReferenceIndex)
    {
        const int indexDifference =
            referenceIndex - previousReferenceIndex;

        const float newSlope =
            (currentDelta - previousDelta) /
            static_cast<float>(indexDifference);

        // 平滑斜率，避免單一封包造成預測值劇烈跳動。
        deltaSlope =
            deltaSlope * 0.85f +
            newSlope * 0.15f;
    }

    const int remainingPoints =
        referenceCount - 1 - referenceIndex;

    projectedDelta =
        currentDelta +
        deltaSlope * static_cast<float>(remainingPoints);

    // 防止定位錯誤時出現非常離譜的數字。
    if(projectedDelta > 60.0f){
        projectedDelta = 60.0f;
    }else if(projectedDelta < -60.0f){
        projectedDelta = -60.0f;
    }

    previousDelta = currentDelta;
    previousReferenceIndex = referenceIndex;
}

void GT7DeltaTracker::endLap(uint32_t lapTime)
{
    // 圈無效、資料太少或時間不合理，不更新參考圈。
    if(lapTime == 0 || currentCount < 10){
        return;
    }

    const bool firstReference =
        referenceCount == 0;

    const bool newBestLap =
        bestLapTime == 0 ||
        lapTime < bestLapTime;

    if(firstReference || newBestLap){
        referenceCount = currentCount;

        if(referenceCount > MAX_POINTS){
            referenceCount = MAX_POINTS;
        }

        for(int i = 0; i < referenceCount; ++i){
            reference[i] = current[i];
        }

        bestLapTime = lapTime;
    }
}

float GT7DeltaTracker::delta() const
{
    return deltaValid ? currentDelta : NAN;
}

float GT7DeltaTracker::deltaP() const
{
    return deltaValid ? projectedDelta : NAN;
}

void GT7DerivedMetrics::reset(){
    fuel.reset();
    delta.reset();
}
