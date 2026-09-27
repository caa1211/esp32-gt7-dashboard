#pragma once
#include <stdint.h>

enum class TelemetryMode : uint8_t { Auto = 0, GT7 = 1, SimHub = 2 };
enum class TelemetrySource : uint8_t { None, GT7, SimHub };

// Keep transport liveness separate from driving/session state. An idle source
// cannot acquire Auto, but menus or a stopped car do not steal an existing lock.
class TelemetrySelector {
public:
    static constexpr uint32_t timeoutMs = 4000;
    struct Status {
        bool received = false;
        bool running = false;
        uint32_t time = 0;
        bool alive(uint32_t now) const { return received && uint32_t(now - time) < timeoutMs; }
    };
    Status gt7, simhub;
    TelemetryMode mode = TelemetryMode::Auto;
    TelemetrySource active = TelemetrySource::None;

    TelemetrySource update(uint32_t now) {
        if (mode == TelemetryMode::GT7)
            return active = gt7.alive(now) ? TelemetrySource::GT7 : TelemetrySource::None;
        if (mode == TelemetryMode::SimHub)
            return active = simhub.alive(now) ? TelemetrySource::SimHub : TelemetrySource::None;
        if (active == TelemetrySource::GT7 && gt7.alive(now)) {
            if (!gt7.running && simhub.alive(now) && simhub.running)
                return active = TelemetrySource::SimHub;
            return active;
        }
        if (active == TelemetrySource::SimHub && simhub.alive(now)) {
            if (!simhub.running && gt7.alive(now) && gt7.running)
                return active = TelemetrySource::GT7;
            return active;
        }
        if (gt7.alive(now) && gt7.running) return active = TelemetrySource::GT7;
        if (simhub.alive(now) && simhub.running) return active = TelemetrySource::SimHub;
        return active = TelemetrySource::None;
    }
};
