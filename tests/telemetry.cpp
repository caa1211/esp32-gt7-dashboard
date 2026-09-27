#include "../src/TelemetrySource.h"
#include "../src/SimHubProtocol.h"
#include <assert.h>
#include <string>

static bool parse(const std::string &input) {
    if (input.size() >= SimHubProtocol::maxLength) return false;
    char buffer[SimHubProtocol::maxLength];
    memcpy(buffer, input.c_str(), input.size() + 1);
    SimHubProtocol::Frame frame;
    return SimHubProtocol::parse(buffer, frame);
}
static std::string replaceField(const std::string &line, unsigned index, const std::string &value) {
    size_t a = 0;
    while (index--) a = line.find(';', a) + 1;
    size_t b = line.find(';', a);
    return line.substr(0, a) + value + (b == std::string::npos ? "" : line.substr(b));
}
int main() {
    const std::string valid = "DSH1;42;1;140;4;5600;68;90;01:24.631;01:25.104;01:24.382;4.5;5;3;10;80;68;36;0;1;0;82;79;76;78";
    assert(parse(valid));
    assert(parse("DSH1;0;0;--;--;--;--;--;--;--;--;--;--;--;--;--;--;--;--;--;--;--;--;--;--"));
    assert(!parse(valid + ";"));
    assert(!parse(valid.substr(0, valid.rfind(';'))));
    assert(!parse(replaceField(valid, 0, "DSH2")));
    assert(!parse(replaceField(valid, 1, "4294967296")));
    assert(!parse(replaceField(valid, 2, "--")));
    assert(!parse(replaceField(valid, 2, "0.5")));
    assert(!parse(replaceField(valid, 3, "NaN")));
    assert(!parse(replaceField(valid, 3, "140garbage")));
    assert(!parse(replaceField(valid, 4, "12")));
    assert(!parse(replaceField(valid, 8, "01:60.000")));
    assert(!parse(replaceField(valid, 8, "01")));
    assert(!parse(replaceField(valid, 15, "101")));
    assert(!parse(replaceField(valid, 16, "-1")));
    assert(!parse(replaceField(valid, 24, "inf")));
    assert(!parse(replaceField(valid, 8, std::string(400, '0'))));
    assert(parse(replaceField(valid, 21, "-10.5")));
    assert(parse(replaceField(valid, 4, "R")));
    assert(parse(replaceField(valid, 1, "4294967295")));

    SimHubProtocol::GearFilter gearFilter;
    assert(std::string(gearFilter.apply("3", 100)) == "3");
    assert(std::string(gearFilter.apply("N", 150)) == "3");
    assert(std::string(gearFilter.apply("N", 849)) == "3");
    assert(std::string(gearFilter.apply("4", 850)) == "4");
    assert(std::string(gearFilter.apply("N", 900)) == "4");
    assert(std::string(gearFilter.apply("N", 1650)) == "N");
    gearFilter.reset();
    assert(std::string(gearFilter.apply("N", 2000)) == "N");
    assert(std::string(gearFilter.apply("--", 2100)) == "--");

    TelemetrySelector selector;
    assert(selector.update(0) == TelemetrySource::None);
    selector.simhub.received = true; selector.simhub.time = 100;
    assert(selector.update(100) == TelemetrySource::None); // idle SimHub cannot acquire
    selector.simhub.running = true;
    assert(selector.update(100) == TelemetrySource::SimHub);
    selector.gt7.received = true; selector.gt7.running = true; selector.gt7.time = 200;
    assert(selector.update(200) == TelemetrySource::SimHub); // no preemption
    selector.simhub.running = false;
    assert(selector.update(300) == TelemetrySource::GT7); // running source replaces idle source
    selector.gt7.running = false; selector.simhub.running = true; selector.simhub.time = 350;
    assert(selector.update(350) == TelemetrySource::SimHub); // switching works both ways
    selector.simhub.running = false; selector.gt7.running = true;
    assert(selector.update(400) == TelemetrySource::GT7);
    selector.gt7.running = false;
    selector.gt7.time = 100;
    assert(selector.update(4099) == TelemetrySource::GT7); // idle source stays selected without replacement
    assert(selector.update(4100) == TelemetrySource::None); // both sources expired
    selector.gt7.running = true; selector.gt7.time = 4100;
    selector.active = TelemetrySource::None; selector.simhub.running = true; selector.simhub.time = 4100;
    assert(selector.update(4100) == TelemetrySource::GT7); // initial tie
    selector.mode = TelemetryMode::SimHub;
    assert(selector.update(4100) == TelemetrySource::SimHub);
    selector.mode = TelemetryMode::GT7; selector.gt7.received = false;
    assert(selector.update(4100) == TelemetrySource::None); // manual does not fall back
    selector.mode = TelemetryMode::Auto; selector.active = TelemetrySource::None;
    selector.simhub.time = 0xfffffff0u;
    assert(selector.update(20) == TelemetrySource::SimHub); // millis wraparound
    assert(selector.update(4000) == TelemetrySource::None);
}
