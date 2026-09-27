#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace SimHubProtocol {
// DSH1;sequence;running;speed;gear;rpm;rpm%;redline%;current;last;best;
// remainingLaps;position;lap;total;fuel%;throttle%;brake%;tc;abs;
// invalidLap;temperatureFL;FR;RL;RR
static constexpr unsigned fieldCount = 25;
static constexpr unsigned maxLength = 384;
static constexpr uint32_t neutralHoldMs = 750;

class GearFilter {
public:
    const char *apply(const char *gear, uint32_t now) {
        if (!gear || strcmp(gear, "--") == 0) {
            neutralPending = false;
            return "--";
        }
        if (strcmp(gear, "N") == 0 && haveGear && strcmp(lastGear, "N") != 0) {
            if (!neutralPending) {
                neutralPending = true;
                neutralStarted = now;
            }
            if (uint32_t(now - neutralStarted) < neutralHoldMs) return lastGear;
        } else {
            neutralPending = false;
        }
        lastGear[0] = gear[0];
        lastGear[1] = 0;
        haveGear = true;
        return lastGear;
    }
    void reset() {
        lastGear[0] = 'N'; lastGear[1] = 0;
        haveGear = false; neutralPending = false; neutralStarted = 0;
    }
private:
    char lastGear[2] = {'N', 0};
    bool haveGear = false, neutralPending = false;
    uint32_t neutralStarted = 0;
};

struct Frame {
    char *fields[fieldCount];
    double values[fieldCount];
};
inline bool number(const char *s, double &value) {
    if (!s || !*s) return false;
    for (const char *p = s; *p; ++p)
        if ((*p < '0' || *p > '9') && *p != '-' && *p != '+' && *p != '.') return false;
    char *end = nullptr;
    value = strtod(s, &end);
    return end != s && *end == 0 && isfinite(value);
}
inline bool lapTime(const char *s) {
    if (strcmp(s, "--") == 0) return true;
    unsigned minutes = 0, seconds = 0, fraction = 0;
    const char *p = s;
    while (*p >= '0' && *p <= '9') { ++minutes; ++p; }
    if (minutes == 0 || minutes > 3 || *p++ != ':') return false;
    for (int i = 0; i < 2; ++i) {
        if (*p < '0' || *p > '9') return false;
        seconds = seconds * 10 + (*p++ - '0');
    }
    if (seconds >= 60 || *p++ != '.') return false;
    while (*p >= '0' && *p <= '9') { ++fraction; ++p; }
    return *p == 0 && fraction > 0 && fraction <= 3;
}
// Destructive split into a bounded caller-owned buffer; atomic commit by caller.
inline bool parse(char *line, Frame &out) {
    if (strlen(line) >= maxLength) return false;
    unsigned count = 1;
    out.fields[0] = line;
    for (char *p = line; *p; ++p) {
        if (*p == ';') {
            if (count == fieldCount) return false;
            *p = 0;
            out.fields[count++] = p + 1;
        }
    }
    if (count != fieldCount || strcmp(out.fields[0], "DSH1") != 0) return false;
    for (unsigned i = 1; i < fieldCount; ++i) {
        const char *s = out.fields[i];
        out.values[i] = NAN;
        if (i >= 8 && i <= 10) { if (!lapTime(s)) return false; continue; }
        if (i == 4) {
            if (strcmp(s, "--") && strcmp(s, "N") && strcmp(s, "R") &&
                !(strlen(s) == 1 && s[0] >= '1' && s[0] <= '9')) return false;
            continue;
        }
        if (i > 2 && strcmp(s, "--") == 0) continue;
        double v;
        if (!number(s, v)) return false;
        double lo = 0, hi = 100;
        if (i == 1) hi = 4294967295.0;
        else if (i == 2 || (i >= 18 && i <= 20)) hi = 1;
        else if (i == 3) hi = 1500;
        else if (i == 5) hi = 30000;
        else if (i >= 11 && i <= 14) hi = 9999;
        else if (i >= 21) { lo = -100; hi = 1000; }
        if (v < lo || v > hi) return false;
        if ((i == 1 || i == 2 || (i >= 12 && i <= 14) || (i >= 18 && i <= 20)) && floor(v) != v) return false;
        out.values[i] = v;
    }
    return true;
}
}
