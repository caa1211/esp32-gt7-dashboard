#pragma once
#include <esp_wifi.h>

// WiFi.SSID() reports the currently associated AP, not saved credentials.
// Call only after the STA driver has been initialized.
inline bool dashboardHasSavedWifi() {
    wifi_config_t config{};
    return esp_wifi_get_config(WIFI_IF_STA, &config) == ESP_OK && config.sta.ssid[0] != 0;
}
