#pragma once
#include <WiFi.h>
#include <WiFiManager.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "TelemetrySource.h"
#include "DashboardWiFiCredentials.h"

// WiFiManager can wait during HTTP scans and credential saves even in its
// nonblocking portal mode. Keep that work off the USB/render loop. Only this
// task owns WiFiManager; commands/status cross queues, never shared Strings.
class DashboardNetwork {
public:
    enum class Action : uint8_t { Configure, Reset, StartPortal, StopPortal };
    struct Command { Action action; TelemetryMode mode; bool firstRun; };
    struct Status { bool connected; bool portal; bool configured; char message[64]; };
    bool begin() {
        commands = xQueueCreate(8, sizeof(Command));
        statuses = xQueueCreate(1, sizeof(Status));
        if (commands && statuses &&
            xTaskCreatePinnedToCore(taskEntry, "dash-wifi", 6144, this, 1, nullptr, 0) == pdPASS) return true;
        if (commands) vQueueDelete(commands);
        if (statuses) vQueueDelete(statuses);
        commands = statuses = nullptr;
        return false;
    }
    bool send(Action action, TelemetryMode mode, bool firstRun = false) {
        Command command{action, mode, firstRun};
        return commands && xQueueSend(commands, &command, 0) == pdTRUE;
    }
    bool receive(Status &status) { return statuses && xQueueReceive(statuses, &status, 0) == pdTRUE; }
private:
    QueueHandle_t commands = nullptr, statuses = nullptr;
    static void taskEntry(void *self) { static_cast<DashboardNetwork *>(self)->run(); }
    void run() {
        WiFiManager manager;
        manager.setDebugOutput(false);
        manager.setConfigPortalBlocking(false);
        manager.setConnectTimeout(2);
        manager.setSaveConnectTimeout(2);
        manager.setConfigPortalTimeout(180);
        manager.setBreakAfterConfig(true);
        manager.setClass("invert");
        manager.setTitle("GT7 DASH SETUP");
        std::vector<const char *> menu = {"wifi", "info", "exit"};
        manager.setMenu(menu);
        TelemetryMode mode = TelemetryMode::SimHub;
        bool enabled = false, offerSetup = false;
        uint32_t attemptTime = 0;
        const auto stopPortal = [&]() {
            if (manager.getConfigPortalActive()) manager.stopConfigPortal();
            if (WiFi.getMode() != WIFI_OFF) WiFi.softAPdisconnect(true);
        };
        const auto startPortal = [&]() {
            if (enabled && !manager.getConfigPortalActive()) manager.startConfigPortal("GT7-DASH-SETUP");
        };
        for (;;) {
            Command command;
            while (xQueueReceive(commands, &command, 0) == pdTRUE) {
                if (command.action == Action::Configure) {
                    stopPortal();
                    mode = command.mode;
                    enabled = mode == TelemetryMode::GT7;
                    offerSetup = enabled && command.firstRun;
                    if (!enabled) WiFi.mode(WIFI_OFF);
                    else {
                        WiFi.mode(WIFI_STA); WiFi.setAutoReconnect(true);
                        attemptTime = millis();
                        if (dashboardHasSavedWifi()) WiFi.begin();
                        else if (offerSetup) { startPortal(); offerSetup = false; }
                    }
                } else if (command.action == Action::Reset) {
                    stopPortal(); WiFi.persistent(true); WiFi.mode(WIFI_STA);
                    esp_wifi_set_storage(WIFI_STORAGE_FLASH);
                    WiFi.setAutoReconnect(false); WiFi.disconnect(true, true);
                    offerSetup = false;
                    enabled = false;
                    WiFi.mode(WIFI_OFF);
                } else if (command.action == Action::StartPortal) {
                    // Wi-Fi setup is independent from the telemetry mode.  A
                    // SimHub-only user may configure credentials now and
                    // switch to Auto/GT7 later.
                    enabled = true;
                    WiFi.mode(WIFI_STA);
                    WiFi.setAutoReconnect(true);
                    startPortal(); offerSetup = false;
                } else {
                    stopPortal(); offerSetup = false;
                    if (!dashboardHasSavedWifi()) { enabled = false; WiFi.mode(WIFI_OFF); }
                }
            }
            if (manager.getConfigPortalActive()) manager.process();
            const bool connected = enabled && WiFi.status() == WL_CONNECTED;
            if (connected) offerSetup = false;
            if (offerSetup && !connected && uint32_t(millis() - attemptTime) > 8000) {
                startPortal(); offerSetup = false;
            }
            if (enabled && !connected && !manager.getConfigPortalActive() &&
                uint32_t(millis() - attemptTime) > 15000 && dashboardHasSavedWifi()) {
                attemptTime = millis(); WiFi.begin();
            }
            Status status{};
            status.connected = connected;
            status.portal = manager.getConfigPortalActive();
            status.configured = dashboardHasSavedWifi();
            String message = !enabled ? "Wi-Fi not used" : status.portal ? "Wi-Fi setup is open" :
                connected ? WiFi.localIP().toString() : dashboardHasSavedWifi() ?
                "Wi-Fi disconnected / retrying" : "GT7 Wi-Fi not configured";
            message.toCharArray(status.message, sizeof(status.message));
            xQueueOverwrite(statuses, &status);
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
};
