
#ifndef __SHCUSTOMPROTOCOL_H__
#define __SHCUSTOMPROTOCOL_H__
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
// selezione la configurazione nella cartella lgfx_user
// #include <LGFX_AUTODETECT.hpp>  // Preparare la classe "LGFX"
// #include <lgfx_user/LGFX_ESP8266_sample.hpp>
#include <lgfx_user/LGFX_ESP32_esp32-2432s028.hpp>
//  dashboard + free deck grafica
#include <Arduino.h>
#include <Preferences.h>
#include <map>
#include <BleGamepad.h> // libreria bluetooth
#include <GT7DerivedMetrics.h>
#include "version.h"

static LGFX tft;

static const int RESET_WAITING_TIME = 3;
static const int SCREEN_WIDTH = 320;
static const int SCREEN_HEIGHT = 240;
static const int X_CENTER = SCREEN_WIDTH / 2;
static const int Y_CENTER = SCREEN_HEIGHT / 2;
static const int ROWS = 5;
static const int COLS = 5;
static const int CELL_WIDTH = SCREEN_WIDTH / COLS;
static const int HALF_CELL_WIDTH = CELL_WIDTH / 2;
static const int CELL_HEIGHT = SCREEN_HEIGHT / ROWS;
static const int HALF_CELL_HEIGHT = CELL_HEIGHT / 2;
static const int COL[] = {0, CELL_WIDTH, CELL_WIDTH * 2, CELL_WIDTH * 3, CELL_WIDTH * 4, CELL_WIDTH * 6, CELL_WIDTH * 7};
static const int ROW[] = {0, CELL_HEIGHT, CELL_HEIGHT * 2, CELL_HEIGHT * 3, CELL_HEIGHT * 4, CELL_HEIGHT * 6, CELL_HEIGHT * 7};

// Explicit 320x240 dashboard grid. The side regions are mirror images and all
// central elements derive from CENTER_X; no center alignment depends on them.
namespace DashboardLayout
{
	static constexpr int WIDTH = 320;
	static constexpr int HEIGHT = 240;
	static constexpr int CENTER_X = 160;
	static constexpr int OUTER_MARGIN = 4;
	static constexpr int SIDE_COLUMN_WIDTH = 100;
	static constexpr int SIDE_WIDTH = SIDE_COLUMN_WIDTH - OUTER_MARGIN * 2;
	static constexpr int LEFT_X = OUTER_MARGIN;
	static constexpr int RIGHT_X = WIDTH - SIDE_COLUMN_WIDTH + OUTER_MARGIN;
	static constexpr int LEFT_DIVIDER_X = SIDE_COLUMN_WIDTH;
	static constexpr int RIGHT_DIVIDER_X = WIDTH - SIDE_COLUMN_WIDTH;
	static constexpr int INNER_PADDING = 1;
	static constexpr int LEFT_TEXT_X = LEFT_X + INNER_PADDING;
	static constexpr int RIGHT_TEXT_X = RIGHT_X + INNER_PADDING;

	static constexpr int RPM_BOTTOM = 48;
	static constexpr int CONTENT_TOP = 50;
	static constexpr int CONTENT_BOTTOM = 192;
	static constexpr int BOTTOM_DIVIDER_Y = 193;
	static constexpr int SIDE_PANEL_Y = CONTENT_TOP;
	static constexpr int SIDE_PANEL_H = CONTENT_BOTTOM - CONTENT_TOP + 1;
	static constexpr int LEFT_PANEL_X = 2;
	static constexpr int RIGHT_PANEL_X = RIGHT_DIVIDER_X + 1;
	static constexpr int LEFT_PANEL_W = LEFT_DIVIDER_X - LEFT_PANEL_X - 1;
	static constexpr int RIGHT_PANEL_W = WIDTH - RIGHT_PANEL_X - 2;
	static constexpr int BOTTOM_PANEL_X = 2;
	static constexpr int BOTTOM_PANEL_Y = 194;
	static constexpr int BOTTOM_PANEL_W = WIDTH - 4;
	static constexpr int BOTTOM_PANEL_H = HEIGHT - BOTTOM_PANEL_Y - 2;

	static constexpr int GEAR_CENTER_X = CENTER_X;
	static constexpr int GEAR_CENTER_Y = 108;
	static constexpr int GEAR_RADIUS = 53;
	static constexpr int GEAR_VALUE_W = 104;
	static constexpr int GEAR_VALUE_H = 100;
	static constexpr int GEAR_VALUE_X = GEAR_CENTER_X - GEAR_VALUE_W / 2;
	static constexpr int GEAR_VALUE_Y = GEAR_CENTER_Y - GEAR_VALUE_H / 2;

	static constexpr int TYRE_W = 69;
	static constexpr int TYRE_H = 25;
	static constexpr int TYRE_CORNER_RADIUS = 2;
	static constexpr int TYRE_X = CENTER_X - TYRE_W / 2;
	static constexpr int TYRE_Y = 164;
	static constexpr int PEDAL_BAR_W = 13;
	static constexpr int PEDAL_BAR_H = TYRE_H + 1;
	static constexpr int PEDAL_BAR_Y = TYRE_Y;
	// Centre each pedal bar in the free space between the tyre cluster and side panels.
	// Mirrored placement keeps both bar centres symmetric around screen X=160.
	// Preserve the previous bar centre while narrowing from 17 to 13 pixels.
	static constexpr int BRAKE_BAR_X = 106;
	static constexpr int THROTTLE_BAR_X = WIDTH - BRAKE_BAR_X - PEDAL_BAR_W;

	static constexpr int SPEED_LABEL_Y = 52;
	static constexpr int SPEED_VALUE_Y = 70;
	static constexpr int SPEED_VALUE_H = 43;
	static constexpr int SPEED_UNIT_Y = 114;
	static constexpr int LEFT_SEPARATOR_Y = 126;
	static constexpr int DELTA_LABEL_Y = 131;
	static constexpr int DELTA_VALUE_Y = 150;
	static constexpr int DELTA_VALUE_H = 38;

	static constexpr int LAP_LABEL_Y[3] = {52, 99, 146};
	static constexpr int LAP_VALUE_Y[3] = {70, 117, 164};
	static constexpr int LAP_SEPARATOR_Y[2] = {95, 142};
	static constexpr int LAP_VALUE_H = 24;

	static constexpr int BOTTOM_LABEL_Y = 196;
	static constexpr int BOTTOM_VALUE_Y = 214;
	// Keep dynamic clears above the panel's bottom border at y=237.
	static constexpr int BOTTOM_VALUE_H = 22;
	static constexpr int BOTTOM_LINE_TOP = 196;
	static constexpr int BOTTOM_LINE_H = 42;
	// Three cells per half; BOTTOM_X[3] is the exact dashboard center divider.
	static constexpr int BOTTOM_X[6] = {0, 56, 108, CENTER_X, 205, 262};
	static constexpr int BOTTOM_W[6] = {56, 52, 52, 45, 57, 58};

	static constexpr int RPM_SEGMENTS = 32;
	static constexpr int RPM_SEGMENT_PITCH = 10;
	static constexpr int RPM_SEGMENT_X = 1;
	static constexpr int RPM_SEGMENT_W = 8;
	static constexpr int RPM_SEGMENT_THICKNESS = 10;
	static constexpr int RPM_LABEL_LEFT_X = 8;
	static constexpr int RPM_LABEL_RIGHT_X = WIDTH - RPM_LABEL_LEFT_X;
}

// Keep the previous five-column dashboard available for A/B testing.
// Set to 1 in the build flags to restore it without reverting this file.
#ifndef GT7_DASHBOARD_LEGACY_UI
#define GT7_DASHBOARD_LEGACY_UI 0
#endif

// Stable persisted values. Never reorder or reuse an existing numeric value.
enum class DashboardTheme : uint8_t
{
	Classic = 0,
	GT3 = 1,
	Retro = 2,
	Radar = 3,
};

// Phase 1 renderer preview hook. Normal firmware loads the persisted theme;
// development builds can set 0 through 3 without modifying source or storage.
#ifndef GT7_DASHBOARD_THEME_PREVIEW
#define GT7_DASHBOARD_THEME_PREVIEW -1
#endif

// Canonical dashboard state shared by every renderer. Telemetry parsing and
// derived-value logic update this state; themes decide only how to present it.
struct DashboardState
{
	int rpmPercent = 50;
	int prev_rpmPercent = 50;
	int rpmRedLineSetting = 90;
	int engineRpm = 0;
	bool rpmAlertRangeValid = false;
	bool revLimitAlertActive = false;
	bool revLimitAlertWasActive = false;
	bool rpmPulseWasActive = false;
	uint8_t lastRpmPulseWhiteMix = 0;
	uint32_t lastRpmPulseFrameTime = 0;
	uint32_t rpmPulseStartTime = 0;
	String gear = "N";
	String prev_gear;
	String speed = "0";
	String currentLapTime = "00:00.00";
	String lastLapTime = "00:00.00";
	String bestLapTime = "00.00.00";
	String sessionBestLiveDeltaSeconds = "+0.000";
	String sessionBestLiveDeltaProgressSeconds = "0.00";
	String tyrePressureFrontLeft = "00.0";
	String tyrePressureFrontRight = "00.0";
	String tyrePressureRearLeft = "00.0";
	String fuelAlertActive = "False";
	String tcLevel = "0";
	String tcFilteredLevel = "0";
	String tcActive = "0";
	String absLevel = "0";
	String absFilteredLevel = "0";
	String absActive = "0";
	String isTCCutNull = "True";
	String tcTcCut = "0  0";
	String brakeBias = "0";
	String brake = "0";
	String lapInvalidated = "False";
	float tyreTemperatures[4] = {NAN, NAN, NAN, NAN};
	String gameRunning = "False";
};

std::map<String, String> prevData;
std::map<String, int32_t> prevColor;

// configuro ble per gamepad

BleGamepad bleGamepad("ESP32 Touch Gamepad", "YourCompany", 100);
BleGamepadConfiguration bleGamepadConfig;

static void showWifiConnectionFailedScreen()
{
	tft.fillScreen(TFT_BLACK);
	tft.setTextDatum(MC_DATUM);

	tft.setTextColor(TFT_RED);
	tft.drawString("WI-FI FAILED", 160, 75);

	tft.setTextColor(TFT_WHITE);
	tft.drawString("Check your password", 160, 125);
	tft.drawString("and try again", 160, 155);

	tft.setTextColor(TFT_LIGHTGREY);
	tft.drawString("Setup portal will restart", 160, 205);
}

static void showWifiSetupScreen()
{
	tft.fillScreen(TFT_BLACK);

	tft.setTextDatum(MC_DATUM);

	tft.setTextColor(TFT_CYAN, TFT_BLACK);
	tft.setTextSize(2);
	tft.drawString("WI-FI SETUP", 160, 35);

	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.setTextSize(1.5);
	tft.drawString("Connect phone to:", 160, 85);

	tft.setTextColor(TFT_YELLOW, TFT_BLACK);
	tft.setTextSize(2);
	tft.drawString("GT7-DASH-SETUP", 160, 120);

	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.setTextSize(1.5);
	tft.drawString("Open:", 160, 160);

	tft.setTextColor(TFT_GREEN, TFT_BLACK);
	tft.setTextSize(2);
	tft.drawString("192.168.4.1", 160, 195);

	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.setTextSize(1);
	tft.drawCentreString(
		String("v") + GT7_DASH_VERSION,
		SCREEN_WIDTH / 2,
		SCREEN_HEIGHT - 12,
		1);
}

int currentPage = 1;	  // Variabile per tenere traccia della pagina corrente
bool forceUpdate = false; // Variabile per forzare l'aggiornamento delle celle

class SHCustomProtocol : private DashboardState
{
private:
	Preferences dashboardPreferences;
	DashboardTheme activeDashboardTheme = DashboardTheme::GT3;
	DashboardTheme renderedDashboardTheme = DashboardTheme::GT3;
	bool dashboardPreferencesReady = false;

	uint16_t touchX, touchY; // definisce i due interi per gestione touchscreen
	int numButtons = 6;		 // Variabile per il numero di pulsanti

	static constexpr uint8_t FADE_STEP = 5;
	static constexpr uint8_t FADE_DELAY_MS = 4;
	static constexpr unsigned long SCREEN_SLEEP_TIMEOUT = 5UL * 60UL * 1000UL;
	// static constexpr unsigned long SCREEN_SLEEP_TIMEOUT = 10000;
	static constexpr uint8_t DEFAULT_BRIGHTNESS_PERCENT = 80;
	static constexpr uint8_t MIN_BRIGHTNESS_PERCENT = 20;
	static constexpr uint8_t BRIGHTNESS_STEP_PERCENT = 10;
	static constexpr unsigned long BRIGHTNESS_SAVE_DELAY_MS = 750UL;
	static constexpr bool TOUCH_SCREEN_CONTROL_ENABLED = true;

	// Connecting 畫面動畫設定
	static constexpr unsigned long CONNECT_ANIMATION_INTERVAL = 1000;
	static constexpr int CONNECT_DOT_COUNT = 5;

	uint8_t userBrightnessPercent = DEFAULT_BRIGHTNESS_PERCENT;
	uint8_t currentBrightness = 255;
	bool brightnessSavePending = false;
	unsigned long brightnessChangedTime = 0;
	unsigned long gameStoppedTime = 0;

	bool gameStoppedTimerStarted = false;
	bool screenSleeping = false;

	// 是否由使用者手動關閉
	bool screenOffByUser = false;

	// Tap the active dashboard to open the touch-driven Settings menu.
	enum class SettingsScreen : uint8_t
	{
		Closed,
		Main,
		ThemeSelection,
		DeviceSettings,
		WifiResetConfirmation,
	};

	static constexpr unsigned long SETTINGS_TIMEOUT_MS = 15000UL;
	SettingsScreen settingsScreen = SettingsScreen::Closed;
	unsigned long settingsLastInteractionTime = 0;
	int settingsPressedButton = -1;
	bool wifiResetConfirmOpen = false;
	bool wifiResetRequested = false;

	// 記錄上一筆 GT7 執行狀態，用來偵測 False -> True
	bool previousGameRunning = false;

	// 避免第一筆資料被誤判為狀態切換
	bool gameRunningInitialized = false;

	static bool isValidDashboardTheme(uint8_t value)
	{
		return value <= static_cast<uint8_t>(DashboardTheme::Radar);
	}

	static const char *dashboardThemeName(DashboardTheme theme)
	{
		switch (theme)
		{
		case DashboardTheme::Classic:
			return "CLASSIC";
		case DashboardTheme::Retro:
			return "RETRO";
		case DashboardTheme::Radar:
			return "RADAR";
		case DashboardTheme::GT3:
		default:
			return "GT3";
		}
	}

	uint8_t normalBrightness() const
	{
		return static_cast<uint8_t>(
			(userBrightnessPercent * 255U + 50U) / 100U);
	}

	void scheduleBrightnessSave()
	{
		brightnessSavePending = true;
		brightnessChangedTime = millis();
	}

	void saveBrightnessIfDue()
	{
		if (!brightnessSavePending ||
			millis() - brightnessChangedTime < BRIGHTNESS_SAVE_DELAY_MS)
			return;
		if (dashboardPreferencesReady)
			dashboardPreferences.putUChar("brightness", userBrightnessPercent);
		brightnessSavePending = false;
	}

	void invalidateDashboardRenderer()
	{
		tft.fillScreen(TFT_BLACK);
		prevData.clear();
		prevColor.clear();
		prev_gear = "";
		prev_rpmPercent = 50;
		revLimitAlertWasActive = false;
		rpmPulseWasActive = false;
		lastRpmPulseWhiteMix = 0;
		lastRpmPulseFrameTime = 0;
		rpmPulseStartTime = millis();
		connectingScreenActive = false;
		connectingAnimationStep = 0;
		lastConnectingAnimationTime = 0;
		forceUpdate = true;
	}

	void loadDashboardTheme()
	{
		dashboardPreferencesReady = dashboardPreferences.begin("gt7dash", false);
		uint8_t storedTheme = static_cast<uint8_t>(DashboardTheme::GT3);
		if (dashboardPreferencesReady)
		{
			storedTheme = dashboardPreferences.getUChar(
				"theme", static_cast<uint8_t>(DashboardTheme::GT3));
			const uint8_t storedBrightness = dashboardPreferences.getUChar(
				"brightness", DEFAULT_BRIGHTNESS_PERCENT);
			userBrightnessPercent =
				storedBrightness >= MIN_BRIGHTNESS_PERCENT && storedBrightness <= 100
				? storedBrightness
				: DEFAULT_BRIGHTNESS_PERCENT;
		}

		activeDashboardTheme = isValidDashboardTheme(storedTheme)
			? static_cast<DashboardTheme>(storedTheme)
			: DashboardTheme::GT3;

#if GT7_DASHBOARD_THEME_PREVIEW >= 0 && GT7_DASHBOARD_THEME_PREVIEW <= 3
		activeDashboardTheme =
			static_cast<DashboardTheme>(GT7_DASHBOARD_THEME_PREVIEW);
#endif

		renderedDashboardTheme = activeDashboardTheme;
	}

#if INCLUDE_GT7_WIFI
	// 最近一次真正收到新 GT7 UDP 封包的時間
	uint32_t lastGT7PacketTime = 0;
	int32_t lastGT7PacketId = -1;
	bool hasReceivedGT7Packet = false;
	bool gt7CarOnTrack = false;

	// GT7 衍生數據：ABS 與剩餘油量圈數。
	GT7DerivedMetrics derivedMetrics;

#endif

	void resetLapDifference()
	{
		previousBestLapMs = -1;
		lastProcessedLapMs = -1;
		sessionBestLiveDeltaSeconds = "+0.000";
	}

	String formatLapTimeMs(int32_t milliseconds)
	{
		if (milliseconds < 0)
		{
			return "--:--.---";
		}

		const uint32_t value = static_cast<uint32_t>(milliseconds);
		const uint32_t minutes = value / 60000UL;
		const uint32_t seconds = (value % 60000UL) / 1000UL;
		const uint32_t millisPart = value % 1000UL;

		char buffer[16];
		snprintf(
			buffer,
			sizeof(buffer),
			"%02lu:%02lu.%03lu",
			static_cast<unsigned long>(minutes),
			static_cast<unsigned long>(seconds),
			static_cast<unsigned long>(millisPart));

		return String(buffer);
	}

	String formatDeltaSeconds(float value)
	{
		if (isnan(value))
		{
			return "+0.000";
		}

		char buffer[16];
		snprintf(buffer, sizeof(buffer), "%+.3f", value);
		return String(buffer);
	}

	String formatLastBestDifference(int32_t lastLapMs, int32_t bestLapMs)
	{
		if (lastLapMs <= 0 || bestLapMs <= 0)
		{
			return "+0.000";
		}

		const float differenceSeconds =
			static_cast<float>(lastLapMs - bestLapMs) / 1000.0f;

		return formatDeltaSeconds(differenceSeconds);
	}

	int32_t parseLapTimeStringMs(String value)
	{
		value.trim();

		if (value.length() == 0 || value.indexOf('-') >= 0)
		{
			return -1;
		}

		const int colon = value.indexOf(':');
		const int dot = value.lastIndexOf('.');

		if (colon < 0 || dot < colon)
		{
			return -1;
		}

		const int32_t minutes = value.substring(0, colon).toInt();
		const int32_t seconds = value.substring(colon + 1, dot).toInt();
		String millisText = value.substring(dot + 1);

		while (millisText.length() < 3)
		{
			millisText += "0";
		}

		if (millisText.length() > 3)
		{
			millisText = millisText.substring(0, 3);
		}

		return minutes * 60000L + seconds * 1000L + millisText.toInt();
	}

	// Connecting 畫面狀態
	bool connectingScreenActive = false;
	// int connectingDotIndex = 0;
	int connectingAnimationStep = 0;
	unsigned long lastConnectingAnimationTime = 0;

public:
	/*
	CUSTOM PROTOCOL CLASS
	SEE https://github.com/zegreatclan/SimHub/wiki/Custom-Arduino-hardware-support

	GENERAL RULES :
		- ALWAYS BACKUP THIS FILE, reinstalling/updating SimHub would overwrite it with the default version.
		- Read data AS FAST AS POSSIBLE in the read function
		- NEVER block the arduino (using delay for instance)
		- Make sure the data read in "read()" function READS ALL THE DATA from the serial port matching the custom protocol definition
		- Idle function is called hundreds of times per second, never use it for slow code, arduino performances would fall
		- If you use library suspending interrupts make sure to use it only in the "read" function when ALL data has been read from the serial port.
			It is the only interrupt safe place

	COMMON FUNCTIONS :
		- FlowSerialReadStringUntil('\n')
			Read the incoming data up to the end (\n) won't be included
		- FlowSerialReadStringUntil(';')
			Read the incoming data up to the separator (;) separator won't be included
		- FlowSerialDebugPrintLn(string)
			Send a debug message to simhub which will display in the log panel and log file (only use it when debugging, it would slow down arduino in run conditions)

	*/

	// Called when starting the arduino (setup method in main sketch)
	void setup()
	{
		tft.init();
		loadDashboardTheme();
		tft.setRotation(DASHBOARD_DISPLAY_ROTATION);
		tft.setBrightness(normalBrightness());
		currentBrightness = normalBrightness();
		tft.fillScreen(TFT_BLACK);
		screenSleeping = false;
		//    Serial.begin(115200);   //x debug
		//    Serial.println("Test seriale avviato!"); //x debug
		gameStoppedTimerStarted = false;
#if INCLUDE_GT7_WIFI
		derivedMetrics.reset();
#endif

		// bleGamepadConfig.setAutoReport(true); // in false non invia i comandi a windows
		// bleGamepadConfig.setAxesMax(32760);
		// bleGamepadConfig.setIncludeSlider1(false);
		// bleGamepadConfig.setIncludeXAxis(false);
		// bleGamepadConfig.setIncludeYAxis(false);
		// bleGamepadConfig.setIncludeZAxis(false);
		// bleGamepadConfig.setIncludeRxAxis(false);
		// bleGamepadConfig.setIncludeRyAxis(false);
		// bleGamepadConfig.setIncludeRzAxis(false);
		// bleGamepadConfig.setButtonCount(numButtons); // Variabile per il numero di pulsanti
		// bleGamepad.begin(&bleGamepadConfig);
		// Serial.println("Configurazione BleGamepad completata.");   //x debug
	}

#if INCLUDE_GT7_WIFI
	bool readGT7Wifi()
	{
		gt7Packet = gt7Telem.readData();

		const int32_t packetId =
			gt7Packet.packetContent.packetId;

		/*
		 * readData() 沒收到新資料時會保留上一筆 Packet。
		 * 因此用 packetId 是否改變，判斷這一圈是否真的收到新封包。
		 */
		if (hasReceivedGT7Packet && packetId == lastGT7PacketId)
		{
			return false;
		}

		// 尚未收到任何有效資料時，忽略初始化的空 Packet。
		if (!hasReceivedGT7Packet && packetId == 0 &&
			gt7Packet.packetContent.magic == 0)
		{
			return false;
		}

		hasReceivedGT7Packet = true;
		lastGT7PacketId = packetId;
		lastGT7PacketTime = millis();

		const auto &data = gt7Packet.packetContent;

		// 速度與檔位
		speed = String(static_cast<int>(data.speed * 3.6f));

		const int currentGear =
			gt7Telem.getCurrentGearFromByte();

		gear = currentGear == 0
				   ? "N"
				   : String(currentGear);

		// RPM Bar
		const float rpm = data.EngineRPM;
		const float maxRpm = data.maxAlertRPM;
		const float redLineRpm = data.minAlertRPM;
		engineRpm = max(0, static_cast<int>(lroundf(rpm)));
		static constexpr float FALLBACK_MAX_RPM = 10000.0f;
		static constexpr float MIN_REASONABLE_MAX_RPM = 1000.0f;
		static constexpr float MAX_REASONABLE_MAX_RPM = 25000.0f;
		const bool maxRpmValid = maxRpm >= MIN_REASONABLE_MAX_RPM &&
			maxRpm <= MAX_REASONABLE_MAX_RPM;
		const float effectiveMaxRpm = maxRpmValid ? maxRpm : FALLBACK_MAX_RPM;

		rpmAlertRangeValid = maxRpmValid && redLineRpm > 0.0f &&
			redLineRpm < maxRpm;

		rpmPercent = constrain(
			static_cast<int>((rpm / effectiveMaxRpm) * 100.0f),
			0,
			100);

		rpmRedLineSetting = rpmAlertRangeValid
			? constrain(
				static_cast<int>((redLineRpm / maxRpm) * 100.0f),
				1,
				99)
			: 90;

		// 圈速：currentLap 只有使用 C Packet 時才有值
		currentLapTime = formatLapTimeMs(data.currentLap);
		lastLapTime = formatLapTimeMs(data.lastLaptime);
		bestLapTime = formatLapTimeMs(data.bestLaptime);

		// 圈數
		const int currentLap = max(0, static_cast<int>(data.lapCount));
		const int totalLapCount = max(0, static_cast<int>(data.totalLaps));

		// 上一圈與最佳圈的圈速差。新最佳圈會顯示 +0.000。
		sessionBestLiveDeltaSeconds =
			updateLastBestDifference(
				data.lastLaptime,
				data.bestLaptime);

		// 此欄位不再顯示 Delta P，畫面改為油門／煞車圖條。
		sessionBestLiveDeltaProgressSeconds = "";

		if (totalLapCount > 0)
		{
			tyrePressureRearLeft = String(currentLap) + "/" + String(totalLapCount);
		}
		else
		{
			tyrePressureRearLeft = String(currentLap);
		}

		// 目前封包只有賽前起跑位置，沒有比賽中的即時排名
		if (data.RaceStartPosition > 0)
		{
			tyrePressureFrontRight = String(data.RaceStartPosition);
		}
		else
		{
			tyrePressureFrontRight = "--";
		}

		// 油量百分比
		float fuelPercent = 0.0f;
		if (data.fuelCapacity > 0.0f)
		{
			fuelPercent = constrain(
				(data.fuelLevel / data.fuelCapacity) * 100.0f,
				0.0f,
				100.0f);
		}

		brakeBias = data.fuelCapacity > 0.0f
			? String(fuelPercent, 0)
			: "--";
		fuelAlertActive = String(fuelPercent, 1);

		for (int tyreIndex = 0; tyreIndex < 4; tyreIndex++)
		{
			tyreTemperatures[tyreIndex] = data.tyreTemp[tyreIndex];
		}

		// 由獨立 library 估算剩餘油量圈數。
		// 注意：GT7FuelEstimator 接收的是公升，不是百分比。
		derivedMetrics.fuel.update(
			data.fuelLevel,
			currentLap);

		const float estimatedFuelLaps =
			derivedMetrics.fuel.remainingLaps();

		if (estimatedFuelLaps >= 0.0f)
		{
			tyrePressureFrontLeft = String(estimatedFuelLaps, 1);
		}
		else
		{
			tyrePressureFrontLeft = "--";
		}

		// 油門、煞車：UDP 範圍 0～255，轉為百分比
		const int throttlePercent =
			constrain(static_cast<int>(data.throttle * 100.0f / 255.0f), 0, 100);

		const int brakePercent =
			constrain(static_cast<int>(data.brake * 100.0f / 255.0f), 0, 100);

		tcLevel = String(throttlePercent);
		absLevel = String(brakePercent);
		// GT7 exposes both driver pedal position and the filtered output actually
		// applied by the simulation (after traction/ABS intervention).
		tcFilteredLevel = String(constrain(
			static_cast<int>(data.throttleFiltered * 100.0f / 255.0f), 0, 100));
		absFilteredLevel = String(constrain(
			static_cast<int>(data.brakeFiltered * 100.0f / 255.0f), 0, 100));

		const uint16_t flags = static_cast<uint16_t>(data.flags);
		revLimitAlertActive =
			(flags & static_cast<uint16_t>(SimulatorFlags::RevLimiterBlinkAlertActive)) != 0;

		// GT7 flags bit 0：車輛目前位於賽道／駕駛畫面中。
		// 回到選單、離開賽道或載入畫面時會變成 false。
		gt7CarOnTrack = (flags & (1U << 0)) != 0;

		const bool tcsIsActive = (flags & (1U << 11)) != 0;

		tcActive = tcsIsActive ? "True" : "False";

		// GT7 沒有直接提供 ABS Active，使用四輪角速度與實際輪胎半徑估算。
		derivedMetrics.abs.update(
			data.speed * 3.6f,
			static_cast<float>(brakePercent),
			data.wheelRPS,
			data.tyreRadius,
			millis());

		absActive = derivedMetrics.abs.isActive()
						? "True"
						: "False";

		isTCCutNull = "True";
		tcTcCut = "0";
		brake = "0";
		lapInvalidated = "False";

		return true;
	}

#endif

	void read()
	{
#if !INCLUDE_GT7_WIFI

		// 1～9：主要行車及圈速資料
		speed = FlowSerialReadStringUntil(';').toInt();
		gear = FlowSerialReadStringUntil(';');
		rpmPercent = FlowSerialReadStringUntil(';').toInt();
		rpmRedLineSetting = FlowSerialReadStringUntil(';').toInt();

		currentLapTime = FlowSerialReadStringUntil(';');
		lastLapTime = FlowSerialReadStringUntil(';');
		bestLapTime = FlowSerialReadStringUntil(';');

		sessionBestLiveDeltaSeconds =
			FlowSerialReadStringUntil(';');

		sessionBestLiveDeltaProgressSeconds =
			FlowSerialReadStringUntil(';');

		// Delta 欄改為上一圈－最佳圈；仍讀取舊欄位以維持 Protocol 相容。
		sessionBestLiveDeltaSeconds = formatLastBestDifference(
			parseLapTimeStringMs(lastLapTime),
			parseLapTimeStringMs(bestLapTime));

		// 10：預估剩餘油量圈數
		tyrePressureFrontLeft =
			FlowSerialReadStringUntil(';');

		// 11：起跑位置／排位位置
		tyrePressureFrontRight =
			FlowSerialReadStringUntil(';');

		// 12：目前圈數／總圈數，例如 3/10
		tyrePressureRearLeft =
			FlowSerialReadStringUntil(';');

		// 13：低油量警示
		fuelAlertActive =
			FlowSerialReadStringUntil(';');

		// 14：油門百分比
		tcLevel =
			FlowSerialReadStringUntil(';');

		// 15：TC 是否介入
		tcActive =
			FlowSerialReadStringUntil(';');

		// 16：煞車百分比
		absLevel =
			FlowSerialReadStringUntil(';');
		// SimHub's existing protocol has no separate filtered pedal channels.
		// Mirror the input so the dual-layer renderer remains backward compatible.
		tcFilteredLevel = tcLevel;
		absFilteredLevel = absLevel;

		// 17：ABS 是否介入
		absActive =
			FlowSerialReadStringUntil(';');

		// 18：固定為 True
		isTCCutNull =
			FlowSerialReadStringUntil(';');

		// 19：保留欄位，固定為 0
		tcTcCut =
			FlowSerialReadStringUntil(';');

		// 20：剩餘油量百分比
		brakeBias =
			FlowSerialReadStringUntil(';');

		// 21：保留欄位，固定為 0
		brake =
			FlowSerialReadStringUntil(';');

		// 22：本圈是否無效
		lapInvalidated =
			FlowSerialReadStringUntil(';');

		// 23：SimHub 是否正在接收遊戲資料
		gameRunning =
			FlowSerialReadStringUntil(';');

		// 清除可能存在的空白、\r
		gameRunning.trim();

		// Protocol 最後一欄也有分號，因此再讀掉封包結尾的換行
		FlowSerialReadStringUntil('\n');
#endif
		const bool isGameRunning =
			gameRunning == "True" ||
			gameRunning == "true" ||
			gameRunning == "TRUE" ||
			gameRunning == "1";

		/*
		 * 只有狀態真的從 False -> True，
		 * 才視為 GT7 新的一次啟動。
		 */
		bool gameJustStarted = false;

		if (gameRunningInitialized)
		{
			gameJustStarted =
				!previousGameRunning &&
				isGameRunning;
		}
		else
		{
			// 第一筆資料只用來建立初始狀態
			gameRunningInitialized = true;
		}

		/*
		 * GT7 從未執行變成執行：
		 * 無論之前是自動或手動關屏，都自動亮起。
		 */
		if (gameJustStarted)
		{
            resetLapDifference();

			screenSleeping = false;
			screenOffByUser = false;

			fadeScreenOn();

			forceUpdate = true;
			gameStoppedTimerStarted = false;
		}

		if (isGameRunning)
		{
			/*
			 * 遊戲持續執行時，只取消自動休眠倒數。
			 * 不直接開背光，避免手動關屏後馬上又亮。
			 */
			gameStoppedTimerStarted = false;
		}
		else
		{
			/*
			 * GT7 未執行時開始五分鐘倒數。
			 */
			if (!gameStoppedTimerStarted)
			{
				gameStoppedTimerStarted = true;
				gameStoppedTime = millis();
			}
		}

		// 最後再保存本次狀態，供下一筆資料比較
		previousGameRunning = isGameRunning;
	}

#if INCLUDE_GT7_WIFI
	void updateGT7GameState()
	{
		const bool telemetryIsAlive =
			lastGT7PacketTime != 0 &&
			millis() - lastGT7PacketTime < RESET_WAITING_TIME * 1000;

		// 不只要求 UDP 還活著，也要求車輛真的在賽道上。
		// 因此離開賽道但 GT7 仍持續送封包時，也會切回等待畫面。
		const bool isGameRunning =
			telemetryIsAlive && gt7CarOnTrack;

		bool gameJustStarted = false;

		if (gameRunningInitialized)
		{
			gameJustStarted =
				!previousGameRunning &&
				isGameRunning;
		}
		else
		{
			gameRunningInitialized = true;

			// 開機時 GT7 已經在執行，也直接亮屏
			gameJustStarted = isGameRunning;
		}

		if (gameJustStarted)
		{
            resetLapDifference();
			screenSleeping = false;
			screenOffByUser = false;

			fadeScreenOn();

			forceUpdate = true;
			gameStoppedTimerStarted = false;
		}

		if (isGameRunning)
		{
			gameStoppedTimerStarted = false;
		}
		else if (!gameStoppedTimerStarted)
		{
			gameStoppedTimerStarted = true;
			gameStoppedTime = millis();
		}

		previousGameRunning = isGameRunning;
	}

#endif

	void loop()
	{
#if INCLUDE_GT7_WIFI
		static uint32_t lastHeartbeatTime = 0;
		const uint32_t now = millis();
		// GT7 需要持續收到 heartbeat 才會繼續傳送遙測資料。
		if (now - lastHeartbeatTime >= 500)
		{
			lastHeartbeatTime = now;
			gt7Telem.sendHeartbeat();
		}

		readGT7Wifi();
		updateGT7GameState();
#endif

		/*
		 * GT7 停止五分鐘，自動關閉背光。
		 */
		if (!screenSleeping &&
			gameStoppedTimerStarted &&
			millis() - gameStoppedTime >= SCREEN_SLEEP_TIMEOUT)
		{
			screenSleeping = true;
			screenOffByUser = false;
			fadeScreenOff();
		}

		// Keep reading touch while asleep so a tap can wake the display and a
		// long press can open Settings.
		readTouch();

		// 確認畫面開啟時，不讓 Dashboard 或 Connecting 畫面蓋回來。
		if (settingsScreen != SettingsScreen::Closed)
		{
			return;
		}

		if (screenSleeping)
		{
			return;
		}

		// 還沒收到 GT7 Telemetry 時顯示等待畫面。
		if (!previousGameRunning)
		{
			updateConnectingScreen();
			return;
		}

		// 從 Connecting 切回主儀表時，完整重畫一次。
		if (connectingScreenActive)
		{
			connectingScreenActive = false;
			connectingAnimationStep = 0;

			tft.fillScreen(TFT_BLACK);
			prevData.clear();
			prevColor.clear();
			prev_gear = "";
			prev_rpmPercent = 50;
			forceUpdate = true;
		}

		static int lastPage = currentPage;

		if (currentPage != lastPage)
		{
			tft.fillScreen(TFT_BLACK);
			prevData.clear();
			prevColor.clear();
			forceUpdate = true;
			lastPage = currentPage;
		}

		if (activeDashboardTheme != renderedDashboardTheme)
		{
			renderedDashboardTheme = activeDashboardTheme;
			invalidateDashboardRenderer();
		}

		if (currentPage == 1)
		{
			renderDashboard(static_cast<const DashboardState &>(*this), forceUpdate);
		}
		else if (forceUpdate)
		{
			drawPage2();
		}

		forceUpdate = false;
	}

	void idle() {}

	void renderDashboard(const DashboardState &state, bool forceUpdate)
	{
		switch (activeDashboardTheme)
		{
		case DashboardTheme::Classic:
			drawPage1Legacy(forceUpdate);
			break;
		case DashboardTheme::Retro:
			drawRetroDashboard(state, forceUpdate);
			break;
		case DashboardTheme::Radar:
			drawRadarDashboard(state, forceUpdate);
			break;
		case DashboardTheme::GT3:
		default:
#if GT7_DASHBOARD_LEGACY_UI
			drawPage1Legacy(forceUpdate);
#else
			drawPage1(forceUpdate);
#endif
			break;
		}
	}

	static uint16_t blendRgb565(uint16_t from, uint16_t to, uint8_t amount)
	{
		const uint8_t fromR = ((from >> 11) & 0x1F) * 255 / 31;
		const uint8_t fromG = ((from >> 5) & 0x3F) * 255 / 63;
		const uint8_t fromB = (from & 0x1F) * 255 / 31;
		const uint8_t toR = ((to >> 11) & 0x1F) * 255 / 31;
		const uint8_t toG = ((to >> 5) & 0x3F) * 255 / 63;
		const uint8_t toB = (to & 0x1F) * 255 / 31;
		return tft.color565(
			fromR + (toR - fromR) * amount / 255,
			fromG + (toG - fromG) * amount / 255,
			fromB + (toB - fromB) * amount / 255);
	}

	void drawRetroTextValue(int32_t x, int32_t y, int32_t width, int32_t height,
		const String &value, const String &id, uint16_t color, uint8_t font,
		textdatum_t datum, bool forceUpdate, float scaleX = 1.0f,
		float scaleY = 1.0f)
	{
		const String state = value + ":" + String(color);
		if (!forceUpdate && prevData[id] == state) return;
		const uint16_t paper = tft.color565(198, 186, 152);
		tft.fillRect(x, y, width, height, paper);
		tft.setTextColor(color, paper);
		tft.setTextDatum(datum);
		tft.setTextSize(scaleX, scaleY);
		const int32_t textX = datum == MR_DATUM ? x + width - 2
			: (datum == MC_DATUM ? x + width / 2 : x + 2);
		tft.drawString(value, textX, y + height / 2, font);
		tft.setTextSize(1.0f);
		tft.setTextDatum(TL_DATUM);
		prevData[id] = state;
	}

	void drawRetroTextValueBuffered(int32_t x, int32_t y, int32_t width,
		int32_t height, const String &value, const String &id, uint16_t color,
		uint8_t font, textdatum_t datum, bool forceUpdate, LGFX_Sprite &sprite,
		bool &spriteCreated, float scaleX = 1.0f, float scaleY = 1.0f)
	{
		const String cacheState = value + ":" + String(color);
		if (!forceUpdate && prevData[id] == cacheState) return;
		const uint16_t paper = tft.color565(198, 186, 152);
		if (!spriteCreated)
		{
			sprite.setColorDepth(16);
			spriteCreated = sprite.createSprite(width, height) != nullptr;
		}
		if (!spriteCreated)
		{
			drawRetroTextValue(x, y, width, height, value, id, color, font,
				datum, true, scaleX, scaleY);
			return;
		}
		sprite.fillSprite(paper);
		sprite.setTextColor(color, paper);
		sprite.setTextDatum(datum);
		sprite.setTextSize(scaleX, scaleY);
		const int32_t textX = datum == MR_DATUM ? width - 2
			: (datum == MC_DATUM ? width / 2 : 2);
		sprite.drawString(value, textX, height / 2, font);
		sprite.setTextSize(1.0f);
		sprite.pushSprite(x, y);
		prevData[id] = cacheState;
	}

	void drawRetroRpm(const DashboardState &state, bool forceUpdate)
	{
		static constexpr int SEGMENT_COUNT = 28;
		static constexpr int SEGMENT_X = 58;
		static constexpr int SEGMENT_PITCH = 9;
		static constexpr int SEGMENT_WIDTH = 7;
		static constexpr int SEGMENT_Y = 24;
		static constexpr int SEGMENT_HEIGHT = 17;
		static constexpr float PULSE_HZ = 3.0f;
		static constexpr uint32_t FRAME_MS = 24;
		static int previousActive = -1;
		static bool previousPulse = false;
		static uint32_t pulseStart = 0;
		static uint32_t lastFrame = 0;
		static uint8_t previousMix = 0;

		const uint16_t paper = tft.color565(198, 186, 152);
		const uint16_t ink = tft.color565(25, 31, 29);
		const uint16_t faint = tft.color565(118, 116, 103);
		const uint16_t alert = tft.color565(178, 55, 39);
		const int active = constrain(
			(state.rpmPercent * SEGMENT_COUNT + 99) / 100, 0, SEGMENT_COUNT);
		const bool warning = state.rpmAlertRangeValid &&
			state.rpmPercent >= state.rpmRedLineSetting;
		const bool pulse = warning || state.revLimitAlertActive;
		const uint32_t now = millis();
		const bool pulseStarted = pulse && !previousPulse;
		if (pulseStarted) pulseStart = now;

		uint8_t whiteMix = 0;
		bool pulseFrame = false;
		if (pulse && (pulseStarted || now - lastFrame >= FRAME_MS))
		{
			float peak = 0.30f;
			if (warning)
			{
				const float span = max(1, 100 - state.rpmRedLineSetting);
				const float progress = constrain(
					(state.rpmPercent - state.rpmRedLineSetting) / span, 0.0f, 1.0f);
				peak += 0.20f * progress;
			}
			if (state.revLimitAlertActive) peak = 0.62f;
			const float elapsed = (now - pulseStart) / 1000.0f;
			const float wave = 0.5f + 0.5f * cosf(TWO_PI * PULSE_HZ * elapsed);
			whiteMix = static_cast<uint8_t>(lroundf(255.0f * peak * wave));
			pulseFrame = pulseStarted || whiteMix != previousMix;
			lastFrame = now;
		}
		else if (pulse)
		{
			whiteMix = previousMix;
		}

		const String alertState = String(state.rpmRedLineSetting) + ":" +
			String(state.rpmAlertRangeValid ? 1 : 0);
		const bool alertChanged = prevData["retroAlertRange"] != alertState;
		const bool pulseEnded = !pulse && previousPulse;
		if (forceUpdate || active != previousActive || alertChanged || pulseFrame || pulseEnded)
		{
			for (int i = 0; i < SEGMENT_COUNT; ++i)
			{
				const bool fillChanged = previousActive < 0 ||
					(i >= min(active, previousActive) && i < max(active, previousActive));
				const bool pulseChanged = i < active && (pulseFrame || pulseEnded);
				if (!forceUpdate && !alertChanged && !fillChanged && !pulseChanged) continue;
				const int x = SEGMENT_X + i * SEGMENT_PITCH;
				tft.fillRect(x, SEGMENT_Y, SEGMENT_WIDTH, SEGMENT_HEIGHT, paper);
				if (i < active)
				{
					const uint16_t color = pulse
						? blendRgb565(ink, TFT_WHITE, whiteMix)
						: ink;
					tft.fillRect(x, SEGMENT_Y, SEGMENT_WIDTH, SEGMENT_HEIGHT, color);
				}
				else
				{
					tft.drawRect(x, SEGMENT_Y, SEGMENT_WIDTH, SEGMENT_HEIGHT, faint);
				}
			}

			// The amber index marks the car-specific min-alert position.
			tft.fillRect(SEGMENT_X - 4, 17,
				SEGMENT_PITCH * (SEGMENT_COUNT - 1) + SEGMENT_WIDTH + 8, 6, paper);
			if (state.rpmAlertRangeValid)
			{
				const float position = constrain(
					state.rpmRedLineSetting / 100.0f, 0.0f, 1.0f);
				const int markerX = SEGMENT_X + lroundf(
					position * SEGMENT_PITCH * (SEGMENT_COUNT - 1)) + SEGMENT_WIDTH / 2;
				tft.fillTriangle(markerX - 3, 17, markerX + 3, 17,
					markerX, 22, alert);
			}
		}

		const int displayedRpm = ((state.engineRpm + 25) / 50) * 50;
		drawRetroTextValue(5, 25, 49, 20, String(displayedRpm), "retroRpm",
			pulse ? alert : ink, 2, MC_DATUM, forceUpdate);
		previousActive = active;
		previousPulse = pulse;
		previousMix = pulse ? whiteMix : 0;
		prevData["retroAlertRange"] = alertState;
	}

	void drawRetroPedals(const DashboardState &state, bool forceUpdate)
	{
		const int brakeInput = constrain(state.absLevel.toInt(), 0, 100);
		const int brakeApplied = constrain(state.absFilteredLevel.toInt(), 0, 100);
		const int throttleInput = constrain(state.tcLevel.toInt(), 0, 100);
		const int throttleApplied = constrain(state.tcFilteredLevel.toInt(), 0, 100);
		const String value = String(brakeInput) + ":" + String(brakeApplied) + ":" +
			String(throttleInput) + ":" + String(throttleApplied);
		if (!forceUpdate && prevData["retroPedals"] == value) return;
		const uint16_t paper = tft.color565(198, 186, 152);
		const uint16_t ink = tft.color565(25, 31, 29);
		const uint16_t faint = tft.color565(118, 116, 103);
		const uint16_t applied = tft.color565(74, 93, 69);
		static LGFX_Sprite pedalSprite(&tft);
		static bool spriteCreated = false;
		if (!spriteCreated)
		{
			pedalSprite.setColorDepth(16);
			spriteCreated = pedalSprite.createSprite(93, 33) != nullptr;
		}
		if (!spriteCreated) return;
		pedalSprite.fillSprite(paper);
		pedalSprite.setTextColor(ink, paper);
		pedalSprite.setTextDatum(TL_DATUM);
		pedalSprite.drawString("B", 8, 4, 1);
		pedalSprite.drawString("T", 8, 17, 1);
		const int barX = 21;
		const int barW = 68;
		const int barH = 7;
		const int values[2][2] = {
			{brakeInput, brakeApplied}, {throttleInput, throttleApplied}};
		for (int row = 0; row < 2; ++row)
		{
			const int y = 7 + row * 13;
			pedalSprite.drawRect(barX, y, barW, barH, faint);
			const int inputW = (barW - 2) * values[row][0] / 100;
			const int appliedW = (barW - 2) * values[row][1] / 100;
			if (inputW > 0) pedalSprite.fillRect(barX + 1, y + 1, inputW, barH - 2, ink);
			if (appliedW > 0) pedalSprite.fillRect(barX + 1, y + 1, appliedW, barH - 2, applied);
		}
		pedalSprite.pushSprite(143, 204);
		prevData["retroPedals"] = value;
	}

	void drawRetroDashboard(const DashboardState &state, bool forceUpdate)
	{
		static LGFX_Sprite speedSprite(&tft);
		static LGFX_Sprite currentSprite(&tft);
		static LGFX_Sprite tyreSprite(&tft);
		static bool speedSpriteCreated = false;
		static bool currentSpriteCreated = false;
		static bool tyreSpriteCreated = false;
		const uint16_t paper = tft.color565(198, 186, 152);
		const uint16_t ink = tft.color565(25, 31, 29);
		const uint16_t faint = tft.color565(118, 116, 103);
		const uint16_t green = tft.color565(55, 102, 64);
		const uint16_t red = tft.color565(178, 55, 39);

		if (forceUpdate)
		{
			tft.fillScreen(paper);
			tft.setTextPadding(0);
			tft.setTextSize(1.0f);
			tft.setTextDatum(TL_DATUM);
			tft.setTextColor(ink, paper);

			// Compact industrial panel grid, sized for the 320 x 240 display.
			tft.drawRoundRect(2, 2, 316, 47, 4, ink);
			tft.drawRoundRect(2, 52, 94, 106, 4, ink);
			tft.drawRoundRect(99, 52, 84, 106, 4, ink);
			tft.drawRoundRect(186, 52, 132, 106, 4, ink);
			tft.drawFastHLine(186, 87, 132, faint);
			tft.drawFastHLine(186, 122, 132, faint);
			tft.drawRoundRect(2, 161, 316, 39, 4, ink);
			tft.drawFastVLine(80, 161, 39, faint);
			tft.drawFastVLine(157, 161, 39, faint);
			tft.drawFastVLine(237, 161, 39, faint);
			tft.drawRoundRect(2, 203, 316, 35, 4, ink);
			tft.drawFastVLine(142, 203, 35, faint);
			tft.drawFastVLine(237, 203, 35, faint);

			tft.drawString("RPM", 8, 8, 2);
			tft.drawString("SPEED", 8, 59, 2);
			tft.drawString("km/h", 61, 143, 1);
			tft.drawCentreString("GEAR", 141, 59, 2);
			tft.drawString("BEST", 192, 57, 1);
			tft.drawString("LAST", 192, 92, 1);
			tft.drawString("CURRENT", 192, 127, 1);
			tft.drawString("FUEL", 8, 166, 1);
			tft.drawString("REM", 86, 166, 1);
			tft.drawString("LAP", 163, 166, 1);
			tft.drawString("POS", 243, 166, 1);
			tft.drawString("TYRE TEMP C", 8, 207, 1);
			tft.drawString("ABS", 243, 208, 1);
			tft.drawString("TCS", 243, 223, 1);
		}

		drawRetroRpm(state, forceUpdate);
		drawRetroTextValueBuffered(7, 77, 84, 62, state.speed, "retroSpeed", ink,
			7, MC_DATUM, forceUpdate, speedSprite, speedSpriteCreated, 0.72f, 0.82f);
		drawRetroTextValue(104, 76, 74, 72, state.gear, "retroGear", ink,
			7, MC_DATUM, forceUpdate, 1.20f, 1.22f);

		// Keep labels and values on separate rows. Fractionally scaled bitmap fonts
		// produced malformed glyphs on the 2.8-inch panel, so Retro uses native sizes.
		drawRetroTextValue(190, 68, 124, 17, state.bestLapTime, "retroBest", ink,
			2, MR_DATUM, forceUpdate);
		drawRetroTextValue(190, 103, 124, 17, state.lastLapTime, "retroLast", ink,
			2, MR_DATUM, forceUpdate);
		const uint16_t currentColor = state.lapInvalidated == "True" ? red : ink;
		drawRetroTextValueBuffered(190, 133, 124, 14, state.currentLapTime,
			"retroCurrent", currentColor, 2, MR_DATUM, forceUpdate,
			currentSprite, currentSpriteCreated, 0.9f, 0.82f);

		uint16_t deltaColor = ink;
		if (state.sessionBestLiveDeltaSeconds.startsWith("-")) deltaColor = green;
		else if (state.sessionBestLiveDeltaSeconds.startsWith("+") &&
			state.sessionBestLiveDeltaSeconds != "+0.000") deltaColor = red;
		drawRetroTextValue(190, 148, 124, 8, state.sessionBestLiveDeltaSeconds,
			"retroDelta", deltaColor, 1, MR_DATUM, forceUpdate);

		drawRetroTextValue(7, 177, 68, 18, state.brakeBias, "retroFuel", ink,
			2, MC_DATUM, forceUpdate);
		drawRetroTextValue(85, 177, 67, 18, state.tyrePressureFrontLeft, "retroRem", ink,
			2, MC_DATUM, forceUpdate);
		String lap = state.tyrePressureRearLeft;
		lap.replace("/", "/");
		drawRetroTextValue(162, 177, 70, 18, lap, "retroLap", ink,
			2, MC_DATUM, forceUpdate);
		drawRetroTextValue(242, 177, 70, 18, state.tyrePressureFrontRight, "retroPos", ink,
			2, MC_DATUM, forceUpdate);

		String tyres;
		const char *names[] = {"FL", "FR", "RL", "RR"};
		for (int i = 0; i < 4; ++i)
		{
			if (i > 0) tyres += " ";
			tyres += names[i];
			tyres += isnan(state.tyreTemperatures[i])
				? "--" : String(lroundf(state.tyreTemperatures[i]));
		}
		drawRetroTextValueBuffered(6, 218, 132, 16, tyres, "retroTyres", ink,
			1, MC_DATUM, forceUpdate, tyreSprite, tyreSpriteCreated);
		drawRetroPedals(state, forceUpdate);
		drawRetroTextValue(273, 205, 40, 15,
			isActiveValue(state.absActive) ? "ON" : "--", "retroAbs",
			isActiveValue(state.absActive) ? red : faint, 1, MC_DATUM, forceUpdate);
		drawRetroTextValue(273, 220, 40, 15,
			isActiveValue(state.tcActive) ? "ON" : "--", "retroTcs",
			isActiveValue(state.tcActive) ? red : faint, 1, MC_DATUM, forceUpdate);
	}

	void drawRadarValue(int32_t x, int32_t y, int32_t width, int32_t height,
		const String &value, const String &id, uint16_t color, uint8_t font,
		textdatum_t datum, bool forceUpdate, float scaleX = 1.0f,
		float scaleY = 1.0f)
	{
		const String cacheState = value + ":" + String(color);
		if (!forceUpdate && prevData[id] == cacheState) return;
		tft.fillRect(x, y, width, height, TFT_BLACK);
		tft.setTextColor(color, TFT_BLACK);
		tft.setTextDatum(datum);
		tft.setTextSize(scaleX, scaleY);
		const int32_t textX = datum == MR_DATUM ? x + width - 1
			: (datum == MC_DATUM ? x + width / 2 : x + 1);
		tft.drawString(value, textX, y + height / 2, font);
		tft.setTextSize(1.0f);
		tft.setTextDatum(TL_DATUM);
		prevData[id] = cacheState;
	}

	void drawRadarLapValue(int32_t x, int32_t y, int32_t width,
		int32_t height, const String &value, bool forceUpdate)
	{
		if (!forceUpdate && prevData["radarLap"] == value) return;
		tft.fillRect(x, y, width, height, TFT_BLACK);
		tft.setTextColor(TFT_WHITE, TFT_BLACK);
		tft.setTextDatum(ML_DATUM);
		tft.setTextSize(0.98f, 1.0f);
		tft.setTextFont(2);

		const int separator = value.indexOf('/');
		if (separator > 0 && separator < value.length() - 1)
		{
			const String currentLap = value.substring(0, separator);
			const String totalLaps = value.substring(separator + 1);
			int32_t textX = x + 1;
			tft.drawString(currentLap, textX, y + height / 2);
			textX += tft.textWidth(currentLap) + 2;
			tft.drawString("/", textX, y + height / 2);
			textX += tft.textWidth("/") + 2;
			tft.drawString(totalLaps, textX, y + height / 2);
		}
		else
		{
			tft.drawString(value, x + 1, y + height / 2);
		}

		tft.setTextFont(1);
		tft.setTextSize(1.0f);
		tft.setTextDatum(TL_DATUM);
		prevData["radarLap"] = value;
	}

	void drawRadarValueBuffered(int32_t x, int32_t y, int32_t width,
		int32_t height, const String &value, const String &id, uint16_t color,
		uint8_t font, textdatum_t datum, bool forceUpdate, LGFX_Sprite &sprite,
		bool &spriteCreated, float scaleX = 1.0f, float scaleY = 1.0f)
	{
		const String cacheState = value + ":" + String(color);
		if (!forceUpdate && prevData[id] == cacheState) return;
		if (!spriteCreated)
		{
			sprite.setColorDepth(16);
			spriteCreated = sprite.createSprite(width, height) != nullptr;
		}
		if (!spriteCreated)
		{
			drawRadarValue(x, y, width, height, value, id, color, font,
				datum, true, scaleX, scaleY);
			return;
		}
		sprite.fillSprite(TFT_BLACK);
		sprite.setTextColor(color, TFT_BLACK);
		sprite.setTextDatum(datum);
		sprite.setTextSize(scaleX, scaleY);
		const int32_t textX = datum == MR_DATUM ? width - 1
			: (datum == MC_DATUM ? width / 2 : 1);
		sprite.drawString(value, textX, height / 2, font);
		sprite.setTextSize(1.0f);
		sprite.pushSprite(x, y);
		prevData[id] = cacheState;
	}

	void drawRadarPedals(const DashboardState &state, bool forceUpdate)
	{
		const int brakeInput = constrain(state.absLevel.toInt(), 0, 100);
		const int brakeApplied = constrain(state.absFilteredLevel.toInt(), 0, 100);
		const int throttleInput = constrain(state.tcLevel.toInt(), 0, 100);
		const int throttleApplied = constrain(state.tcFilteredLevel.toInt(), 0, 100);
		const String cacheState = String(brakeInput) + ":" + String(brakeApplied) +
			":" + String(throttleInput) + ":" + String(throttleApplied);
		if (!forceUpdate && prevData["radarPedals"] == cacheState) return;

		static LGFX_Sprite pedalSprite(&tft);
		static bool spriteCreated = false;
		if (!spriteCreated)
		{
			pedalSprite.setColorDepth(16);
			spriteCreated = pedalSprite.createSprite(56, 24) != nullptr;
		}
		if (!spriteCreated) return;
		const uint16_t frame = tft.color565(70, 74, 78);
		const uint16_t input = tft.color565(112, 116, 120);
		const uint16_t applied = tft.color565(190, 195, 200);
		pedalSprite.fillSprite(TFT_BLACK);
		pedalSprite.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
		pedalSprite.setTextDatum(TL_DATUM);
		pedalSprite.drawString("B", 0, 1, 1);
		pedalSprite.drawString("T", 0, 13, 1);
		const int values[2][2] = {
			{brakeInput, brakeApplied}, {throttleInput, throttleApplied}};
		for (int row = 0; row < 2; ++row)
		{
			const int y = 2 + row * 12;
			pedalSprite.drawRect(10, y, 45, 7, frame);
			const int inputWidth = 43 * values[row][0] / 100;
			const int appliedWidth = 43 * values[row][1] / 100;
			if (inputWidth > 0)
				pedalSprite.fillRect(11, y + 1, inputWidth, 5, input);
			if (appliedWidth > 0)
				pedalSprite.fillRect(11, y + 1, appliedWidth, 5, applied);
		}
		pedalSprite.pushSprite(258, 171);
		prevData["radarPedals"] = cacheState;
	}

	void drawRadarTyres(const DashboardState &state, bool forceUpdate,
		LGFX_Sprite &sprite, bool &spriteCreated)
	{
		String cacheState;
		for (int i = 0; i < 4; ++i)
		{
			if (i > 0) cacheState += ":";
			cacheState += isnan(state.tyreTemperatures[i])
				? "--" : String(lroundf(state.tyreTemperatures[i]));
		}
		if (!forceUpdate && prevData["radarTyres"] == cacheState) return;

		if (!spriteCreated)
		{
			sprite.setColorDepth(16);
			spriteCreated = sprite.createSprite(88, 17) != nullptr;
		}
		if (!spriteCreated) return;

		sprite.fillSprite(TFT_BLACK);
		sprite.setTextDatum(MC_DATUM);
		sprite.setTextSize(1.0f);
		for (int i = 0; i < 4; ++i)
		{
			const bool valid = isfinite(state.tyreTemperatures[i]) &&
				state.tyreTemperatures[i] > 0.0f;
			const uint16_t color = valid
				? tyreTemperatureColor(state.tyreTemperatures[i])
				: tft.color565(55, 59, 62);
			const int x = i * 22;
			sprite.setTextColor(valid ? TFT_LIGHTGREY : tft.color565(90, 94, 98),
				TFT_BLACK);
			sprite.drawString(valid
				? String(lroundf(state.tyreTemperatures[i])) : "--",
				x + 10, 6, 1);
			sprite.fillRoundRect(x + 5, 12, 10, 2, 1, color);
		}
		sprite.setTextSize(1.0f);
		sprite.pushSprite(226, 218);
		prevData["radarTyres"] = cacheState;
	}

	void drawRadarStatusDot(int32_t x, int32_t y, bool active,
		const String &id, uint16_t activeColor, uint16_t inactiveColor,
		bool forceUpdate)
	{
		const String cacheState = active ? "1" : "0";
		if (!forceUpdate && prevData[id] == cacheState) return;
		tft.fillRect(x - 9, y - 7, 18, 14, TFT_BLACK);
		if (active)
		{
			tft.fillCircle(x, y, 5, activeColor);
			tft.drawCircle(x, y, 6, activeColor);
		}
		else
		{
			tft.drawCircle(x, y, 5, inactiveColor);
		}
		prevData[id] = cacheState;
	}

	void drawRadarRpm(const DashboardState &state, bool forceUpdate)
	{
		static constexpr int CENTER_X = 160;
		static constexpr int CENTER_Y = 108;
		static constexpr int INNER_RADIUS = 70;
		static constexpr int OUTER_RADIUS = 84;
		static constexpr int SEGMENT_COUNT = 36;
		static constexpr float START_DEGREES = 140.0f;
		static constexpr float SWEEP_DEGREES = 260.0f;
		static constexpr float PULSE_HZ = 3.0f;
		static constexpr uint32_t FRAME_MS = 24;
		static int previousActive = -1;
		static bool previousPulse = false;
		static uint32_t pulseStart = 0;
		static uint32_t lastFrame = 0;
		static uint8_t previousMix = 0;

		const int active = constrain(
			(state.rpmPercent * SEGMENT_COUNT + 99) / 100, 0, SEGMENT_COUNT);
		const bool warning = state.rpmAlertRangeValid &&
			state.rpmPercent >= state.rpmRedLineSetting;
		const bool pulse = warning || state.revLimitAlertActive;
		const uint32_t now = millis();
		const bool pulseStarted = pulse && !previousPulse;
		if (pulseStarted) pulseStart = now;

		uint8_t whiteMix = 0;
		bool pulseFrame = false;
		if (pulse && (pulseStarted || now - lastFrame >= FRAME_MS))
		{
			float peak = 0.30f;
			if (warning)
			{
				const float span = max(1, 100 - state.rpmRedLineSetting);
				const float progress = constrain(
					(state.rpmPercent - state.rpmRedLineSetting) / span, 0.0f, 1.0f);
				peak += 0.20f * progress;
			}
			if (state.revLimitAlertActive) peak = 0.62f;
			const float elapsed = (now - pulseStart) / 1000.0f;
			const float wave = 0.5f + 0.5f * cosf(TWO_PI * PULSE_HZ * elapsed);
			whiteMix = static_cast<uint8_t>(lroundf(255.0f * peak * wave));
			pulseFrame = pulseStarted || whiteMix != previousMix;
			lastFrame = now;
		}
		else if (pulse)
		{
			whiteMix = previousMix;
		}

		const bool pulseEnded = !pulse && previousPulse;
		if (!forceUpdate && active == previousActive && !pulseFrame && !pulseEnded)
			return;

		const float minAlertPosition = state.rpmAlertRangeValid
			? constrain(state.rpmRedLineSetting / 100.0f, 0.45f, 0.95f)
			: 0.80f;
		const float stops[] = {0.0f, minAlertPosition * 0.45f,
			minAlertPosition * 0.72f, minAlertPosition, 1.0f};
		const uint8_t colors[][3] = {
			{72, 205, 55}, {130, 225, 45}, {250, 224, 35},
			{255, 126, 22}, {255, 24, 28}};
		const uint16_t inactive = tft.color565(48, 54, 58);

		auto pointAt = [&](float degrees, int radius, int &px, int &py)
		{
			const float radians = degrees * DEG_TO_RAD;
			px = CENTER_X + lroundf(cosf(radians) * radius);
			py = CENTER_Y + lroundf(sinf(radians) * radius);
		};

		for (int i = 0; i < SEGMENT_COUNT; ++i)
		{
			const bool fillChanged = previousActive < 0 ||
				(i >= min(active, previousActive) && i < max(active, previousActive));
			const bool pulseChanged = i < active && (pulseFrame || pulseEnded);
			if (!forceUpdate && !fillChanged && !pulseChanged) continue;

			uint16_t color = inactive;
			if (i < active)
			{
				const float position = i / static_cast<float>(SEGMENT_COUNT - 1);
				int stop = 0;
				while (stop < 3 && position > stops[stop + 1]) ++stop;
				const float span = max(0.001f, stops[stop + 1] - stops[stop]);
				const float blend = constrain(
					(position - stops[stop]) / span, 0.0f, 1.0f);
				int red = lroundf(colors[stop][0] +
					(colors[stop + 1][0] - colors[stop][0]) * blend);
				int green = lroundf(colors[stop][1] +
					(colors[stop + 1][1] - colors[stop][1]) * blend);
				int blue = lroundf(colors[stop][2] +
					(colors[stop + 1][2] - colors[stop][2]) * blend);
				if (pulse)
				{
					red += (255 - red) * whiteMix / 255;
					green += (255 - green) * whiteMix / 255;
					blue += (255 - blue) * whiteMix / 255;
				}
				color = tft.color565(red, green, blue);
			}

			const float centerAngle = START_DEGREES +
				SWEEP_DEGREES * i / (SEGMENT_COUNT - 1);
			const float halfAngle = SWEEP_DEGREES /
				(SEGMENT_COUNT - 1) * 0.30f;
			int x1, y1, x2, y2, x3, y3, x4, y4;
			pointAt(centerAngle - halfAngle, 70, x1, y1);
			pointAt(centerAngle - halfAngle, 84, x2, y2);
			pointAt(centerAngle + halfAngle, 70, x3, y3);
			pointAt(centerAngle + halfAngle, 84, x4, y4);
			tft.fillTriangle(x1, y1, x2, y2, x3, y3, color);
			tft.fillTriangle(x2, y2, x4, y4, x3, y3, color);
		}

		if (state.rpmAlertRangeValid)
		{
			const float markerAngle = START_DEGREES +
				SWEEP_DEGREES * constrain(
					state.rpmRedLineSetting / 100.0f, 0.0f, 1.0f);
			int x1, y1, x2, y2;
			pointAt(markerAngle, INNER_RADIUS - 5, x1, y1);
			pointAt(markerAngle, OUTER_RADIUS + 3, x2, y2);
			tft.drawLine(x1, y1, x2, y2, tft.color565(255, 80, 42));
		}
		previousActive = active;
		previousPulse = pulse;
		previousMix = pulse ? whiteMix : 0;
	}

	void drawRadarDashboard(const DashboardState &state, bool forceUpdate)
	{
		static LGFX_Sprite speedSprite(&tft);
		static LGFX_Sprite currentSprite(&tft);
		static LGFX_Sprite tyreSprite(&tft);
		static bool speedSpriteCreated = false;
		static bool currentSpriteCreated = false;
		static bool tyreSpriteCreated = false;
		const uint16_t frame = tft.color565(55, 59, 62);
		const uint16_t muted = tft.color565(150, 154, 158);
		const uint16_t green = tft.color565(92, 220, 56);
		const uint16_t yellow = tft.color565(255, 218, 24);
		const uint16_t blue = tft.color565(30, 155, 255);
		const uint16_t red = tft.color565(255, 70, 52);
		const String layoutState = String(state.rpmRedLineSetting) + ":" +
			String(state.rpmAlertRangeValid ? 1 : 0);
		const bool redraw = forceUpdate || prevData["radarLayoutState"] != layoutState;

		if (redraw)
		{
			tft.fillScreen(TFT_BLACK);
			tft.setTextPadding(0);
			tft.setTextDatum(TL_DATUM);
			tft.setTextSize(1.0f);

			tft.fillCircle(160, 108, 88, TFT_BLACK);
			tft.drawCircle(160, 108, 88, tft.color565(62, 65, 68));
			tft.drawCircle(160, 108, 86, tft.color565(35, 38, 40));

			tft.drawRoundRect(2, 20, 65, 179, 4, frame);
			tft.drawRoundRect(253, 20, 65, 179, 4, frame);
			for (int y : {64, 108, 152})
			{
				tft.drawFastHLine(3, y, 63, frame);
				tft.drawFastHLine(254, y, 63, frame);
			}
			tft.drawRoundRect(2, 203, 316, 35, 4, frame);
			tft.drawFastVLine(102, 204, 33, frame);
			tft.drawFastVLine(222, 204, 33, frame);

			for (int i = 0; i <= 12; ++i)
			{
				const float angle = (140.0f + 260.0f * i / 12.0f) * DEG_TO_RAD;
				const int inner = i % 3 == 0 ? 60 : 63;
				const int x1 = 160 + lroundf(cosf(angle) * inner);
				const int y1 = 108 + lroundf(sinf(angle) * inner);
				const int tickOuter = 68;
				const int x2 = 160 + lroundf(cosf(angle) * tickOuter);
				const int y2 = 108 + lroundf(sinf(angle) * tickOuter);
				tft.drawLine(x1, y1, x2, y2,
					i % 3 == 0 ? TFT_WHITE : tft.color565(85, 88, 92));
			}

			tft.setTextColor(muted, TFT_BLACK);
			tft.drawString("LAP", 8, 25, 1);
			tft.drawString("POS", 8, 69, 1);
			tft.drawString("BEST", 8, 113, 1);
			tft.drawString("LAST", 8, 157, 1);
			tft.drawString("FUEL", 260, 25, 1);
			tft.drawString("REM", 260, 69, 1);
			tft.drawString("CURRENT", 258, 113, 1);
			tft.drawString("PEDALS", 258, 155, 1);
			tft.drawCentreString("GEAR", 160, 53, 1);
			tft.drawCentreString("km/h", 160, 163, 1);
			tft.drawCentreString("RPM", 160, 178, 1);
			tft.drawString("ABS", 8, 208, 1);
			tft.drawString("TCS", 54, 208, 1);
			tft.drawString("DELTA", 110, 208, 1);
			tft.drawString("TYRE TEMP", 230, 208, 1);
		}

		drawRadarRpm(state, redraw);
		drawRadarLapValue(7, 36, 56, 24, state.tyrePressureRearLeft, redraw);
		drawRadarValue(7, 80, 56, 24, state.tyrePressureFrontRight,
			"radarPos", TFT_WHITE, 2, ML_DATUM, redraw, 1.05f, 1.0f);
		drawRadarValue(7, 124, 56, 24, state.bestLapTime,
			"radarBest", TFT_WHITE, 1, ML_DATUM, redraw);
		drawRadarValue(7, 168, 56, 24, state.lastLapTime,
			"radarLast", TFT_WHITE, 1, ML_DATUM, redraw);
		drawRadarValue(257, 36, 57, 24, state.brakeBias,
			"radarFuel", TFT_WHITE, 2, MR_DATUM, redraw, 1.0f, 1.0f);
		drawRadarValue(257, 80, 57, 24, state.tyrePressureFrontLeft,
			"radarRem", TFT_WHITE, 2, MR_DATUM, redraw, 1.0f, 1.0f);
		drawRadarValueBuffered(257, 124, 57, 22, state.currentLapTime,
			"radarCurrent", state.lapInvalidated == "True" ? red : TFT_WHITE,
			1, MR_DATUM, redraw, currentSprite, currentSpriteCreated);

		drawRadarTyres(state, redraw, tyreSprite, tyreSpriteCreated);
		drawRadarPedals(state, redraw);

		uint16_t deltaColor = TFT_WHITE;
		if (state.sessionBestLiveDeltaSeconds.startsWith("-")) deltaColor = green;
		else if (state.sessionBestLiveDeltaSeconds.startsWith("+") &&
			state.sessionBestLiveDeltaSeconds != "+0.000") deltaColor = red;
		String deltaDisplay = state.sessionBestLiveDeltaSeconds;
		if (deltaDisplay.startsWith("+") || deltaDisplay.startsWith("-"))
			deltaDisplay = deltaDisplay.substring(0, 1) + " " + deltaDisplay.substring(1);
		drawRadarValue(106, 218, 112, 17, deltaDisplay,
			"radarDelta", deltaColor, 2, MC_DATUM, redraw, 1.14f, 1.0f);
		drawRadarStatusDot(29, 226, isActiveValue(state.absActive), "radarAbs",
			yellow, muted, redraw);
		drawRadarStatusDot(75, 226, isActiveValue(state.tcActive), "radarTcs",
			blue, muted, redraw);

		drawRadarValue(116, 63, 88, 56, state.gear, "radarGear",
			TFT_WHITE, 7, MC_DATUM, redraw, 0.98f, 1.0f);
		drawRadarValueBuffered(115, 121, 90, 39, state.speed, "radarSpeed",
			TFT_WHITE, 7, MC_DATUM, redraw, speedSprite, speedSpriteCreated,
			0.66f, 0.72f);
		const int displayedRpm = ((state.engineRpm + 25) / 50) * 50;
		drawRadarValue(127, 187, 66, 16, String(displayedRpm), "radarRpm",
			state.revLimitAlertActive ? red : tft.color565(255, 118, 25),
			2, MC_DATUM, redraw);
		prevData["radarLayoutState"] = layoutState;
	}

	void drawThemePlaceholder(
		const DashboardState &state,
		DashboardTheme theme,
		bool forceUpdate)
	{
		const String cacheKey = theme == DashboardTheme::Classic
			? "classicPlaceholder"
			: "retroPlaceholder";
		const String liveState = state.speed + ":" + state.gear + ":" +
			String(state.engineRpm);
		if (!forceUpdate && prevData[cacheKey] == liveState)
		{
			return;
		}

		if (forceUpdate)
		{
			tft.fillScreen(TFT_BLACK);
			tft.setTextPadding(0);
			tft.setTextDatum(MC_DATUM);
			tft.setTextColor(TFT_WHITE, TFT_BLACK);
			tft.drawCentreString(dashboardThemeName(theme), X_CENTER, 70, 4);
			tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
			tft.drawCentreString("PHASE 1 RENDERER", X_CENTER, 105, 2);
			tft.drawCentreString("UI COMING IN A LATER PHASE", X_CENTER, 130, 1);
		}

		tft.fillRect(30, 160, SCREEN_WIDTH - 60, 38, TFT_BLACK);
		tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
		tft.setTextDatum(MC_DATUM);
		const String telemetryLine = state.speed + " km/h   GEAR " + state.gear +
			"   " + String(state.engineRpm) + " RPM";
		tft.drawCentreString(telemetryLine, X_CENTER, 176, 2);
		prevData[cacheKey] = liveState;
	}

	void drawConnectingScreenBase()
	{
		tft.fillScreen(TFT_BLACK);

		tft.setTextPadding(0);
		tft.setTextDatum(MC_DATUM);

		tft.setTextColor(TFT_WHITE, TFT_BLACK);
		tft.drawCentreString(
			"GT7 DASH",
			SCREEN_WIDTH / 2,
			75,
			4);

		tft.setTextColor(
			tft.color565(120, 120, 120),
			TFT_BLACK);

		tft.drawCentreString(
			"Waiting for Telemetry",
			SCREEN_WIDTH / 2,
			120,
			2);

		const int barWidth = 110;
		const int barHeight = 3;
		const int barX = (SCREEN_WIDTH - barWidth) / 2;
		const int barY = 162;

		tft.fillRect(
			barX,
			barY,
			barWidth,
			barHeight,
			tft.color565(28, 28, 28));

		tft.drawCentreString(
			"Tap for settings",
			SCREEN_WIDTH / 2,
			SCREEN_HEIGHT - 26,
			1);

		tft.drawCentreString(
			String("v") + GT7_DASH_VERSION + "  |  by caa1211",
			SCREEN_WIDTH / 2,
			SCREEN_HEIGHT - 12,
			1);
	}

	void drawConnectingBar()
	{
		const int barWidth = 110;
		const int barHeight = 3;

		const int barX = (SCREEN_WIDTH - barWidth) / 2;
		const int barY = 162;

		// 每次先重畫整條深灰底
		tft.fillRect(
			barX,
			barY,
			barWidth,
			barHeight,
			tft.color565(28, 28, 28));

		const int highlightWidth = 32;

		// 讓流光可以從左側外面進入，再從右側離開
		const int travelWidth = barWidth + highlightWidth * 2;
		const int highlightX =
			(connectingAnimationStep % travelWidth) - highlightWidth;

		/*
		 * 將亮區切成多段灰階：
		 *
		 * 暗 → 灰 → 亮灰 → 灰 → 暗
		 *
		 * 因為高度只有 3px，看起來會像柔和的流光。
		 */
		const int segmentCount = 16;
		const int segmentWidth =
			(highlightWidth + segmentCount - 1) / segmentCount;

		for (int i = 0; i < segmentCount; i++)
		{
			const float position =
				(float)i / (segmentCount - 1);

			// 三角形亮度，中央最亮、兩側漸暗
			float brightness =
				1.0f - fabsf(position * 2.0f - 1.0f);

			// 亮度範圍約 45～190，避免白得太搶眼
			uint8_t gray =
				45 + (uint8_t)(brightness * 145);

			int segmentX =
				barX + highlightX + i * segmentWidth;

			int drawWidth = segmentWidth + 1;

			// 裁切左邊界
			if (segmentX < barX)
			{
				drawWidth -= barX - segmentX;
				segmentX = barX;
			}

			// 裁切右邊界
			if (segmentX + drawWidth > barX + barWidth)
			{
				drawWidth =
					barX + barWidth - segmentX;
			}

			if (drawWidth > 0)
			{
				tft.fillRect(
					segmentX,
					barY,
					drawWidth,
					barHeight,
					tft.color565(gray, gray, gray));
			}
		}
	}
	void updateConnectingScreen()
	{
		if (!connectingScreenActive)
		{
			tft.fillScreen(TFT_BLACK);
			drawConnectingScreenBase();

			connectingAnimationStep = 0;
			lastConnectingAnimationTime = 0;
			connectingScreenActive = true;
		}

		const unsigned long now = millis();

		// 數值越大，動畫移動越慢
		if (now - lastConnectingAnimationTime >= 65)
		{
			lastConnectingAnimationTime = now;

			connectingAnimationStep += 2;

			const int highlightWidth = 32;
			const int barWidth = 110;
			const int travelWidth =
				barWidth + highlightWidth * 2;

			if (connectingAnimationStep >= travelWidth)
			{
				connectingAnimationStep = 0;
			}

			drawConnectingBar();
		}
	}

	bool isActiveValue(String value)
	{
		value.trim();
		return value == "True" ||
			   value == "true" ||
			   value == "TRUE" ||
			   value == "1";
	}

	int32_t previousBestLapMs = -1;
	int32_t lastProcessedLapMs = -1;

	String updateLastBestDifference(
		int32_t lastLapMs,
		int32_t bestLapMs)
	{
		if (lastLapMs <= 0 || bestLapMs <= 0)
		{
			return "+0.000";
		}

		// 同一個上一圈可能每個封包重複傳送，只處理一次。
		if (lastLapMs == lastProcessedLapMs)
		{
			return sessionBestLiveDeltaSeconds;
		}

		lastProcessedLapMs = lastLapMs;

		String result = "+0.000";

		if (previousBestLapMs > 0)
		{
			result = formatDeltaSeconds(
				static_cast<float>(lastLapMs - previousBestLapMs) / 1000.0f);
		}

		// 本圈處理完後，才更新保存的最佳圈。
		previousBestLapMs = bestLapMs;

		return result;
	}

	void drawPage1(bool forceUpdate = false)
	{
		static LGFX_Sprite speedSprite(&tft);
		static LGFX_Sprite currentLapSprite(&tft);
		static LGFX_Sprite gearSprite(&tft);
		static bool speedSpriteCreated = false;
		static bool currentLapSpriteCreated = false;
		static bool gearSpriteCreated = false;
		const uint16_t panelLine = tft.color565(82, 86, 92);
		const uint16_t labelColor = tft.color565(155, 158, 162);

		if (forceUpdate)
		{
			tft.fillScreen(TFT_BLACK);
			tft.setTextPadding(0);
			tft.setTextDatum(TL_DATUM);

			// Three restrained panel outlines match the target dashboard grouping.
			tft.drawRoundRect(DashboardLayout::LEFT_PANEL_X, DashboardLayout::SIDE_PANEL_Y,
				DashboardLayout::LEFT_PANEL_W, DashboardLayout::SIDE_PANEL_H, 5, panelLine);
			tft.drawRoundRect(DashboardLayout::RIGHT_PANEL_X, DashboardLayout::SIDE_PANEL_Y,
				DashboardLayout::RIGHT_PANEL_W, DashboardLayout::SIDE_PANEL_H, 5, panelLine);
			tft.drawRoundRect(DashboardLayout::BOTTOM_PANEL_X, DashboardLayout::BOTTOM_PANEL_Y,
				DashboardLayout::BOTTOM_PANEL_W, DashboardLayout::BOTTOM_PANEL_H, 5, panelLine);
			tft.drawFastHLine(DashboardLayout::LEFT_TEXT_X, DashboardLayout::LEFT_SEPARATOR_Y,
				DashboardLayout::SIDE_WIDTH - 3, panelLine);
			for (int i = 0; i < 2; ++i)
				tft.drawFastHLine(DashboardLayout::RIGHT_TEXT_X, DashboardLayout::LAP_SEPARATOR_Y[i],
					DashboardLayout::SIDE_WIDTH - 2, panelLine);

			drawMeasuredText("SPEED", DashboardLayout::LEFT_TEXT_X,
				DashboardLayout::SPEED_LABEL_Y, DashboardLayout::SIDE_WIDTH, 18,
				&fonts::FreeSans9pt7b, labelColor, ML_DATUM, 0.72f, 0.74f);
			drawMeasuredText("km/h", DashboardLayout::LEFT_X,
				DashboardLayout::SPEED_UNIT_Y, DashboardLayout::SIDE_WIDTH - 5, 13,
				&fonts::FreeSans9pt7b, labelColor, MR_DATUM, 0.62f, 0.64f);
			drawMeasuredText("DELTA", DashboardLayout::LEFT_TEXT_X,
				DashboardLayout::DELTA_LABEL_Y, DashboardLayout::SIDE_WIDTH, 18,
				&fonts::FreeSans9pt7b, labelColor, ML_DATUM, 0.72f, 0.74f);
			const char *lapLabels[] = {"BEST", "LAST", "CURRENT"};
			for (int i = 0; i < 3; ++i)
				drawMeasuredText(lapLabels[i], DashboardLayout::RIGHT_TEXT_X,
					DashboardLayout::LAP_LABEL_Y[i], DashboardLayout::SIDE_WIDTH, 18,
					&fonts::FreeSans9pt7b, labelColor, ML_DATUM, 0.72f, 0.74f);

			const char *bottomLabels[] = {"FUEL", "LEFT", "LAP", "POS", "ABS", "TCS"};
			for (int i = 0; i < 6; ++i)
			{
				if (i > 0) tft.drawFastVLine(DashboardLayout::BOTTOM_X[i],
					DashboardLayout::BOTTOM_LINE_TOP, DashboardLayout::BOTTOM_LINE_H, panelLine);
				drawMeasuredText(bottomLabels[i], DashboardLayout::BOTTOM_X[i],
					DashboardLayout::BOTTOM_LABEL_Y, DashboardLayout::BOTTOM_W[i], 17,
					&fonts::FreeSans9pt7b, labelColor, MC_DATUM, 0.70f, 0.72f);
			}
			tft.setTextDatum(TL_DATUM);
		}

		drawRpmMeterGT3(forceUpdate);
		drawDashboardValueBuffered(DashboardLayout::LEFT_X, DashboardLayout::SPEED_VALUE_Y,
			DashboardLayout::SIDE_WIDTH, DashboardLayout::SPEED_VALUE_H,
			speed, "gt3Speed", TFT_WHITE, &fonts::DejaVu56,
			MC_DATUM, forceUpdate, speedSprite, speedSpriteCreated, 0.72f, 0.72f);

		uint16_t deltaColor = TFT_WHITE;
		if (sessionBestLiveDeltaSeconds.startsWith("-")) deltaColor = TFT_GREEN;
		else if (sessionBestLiveDeltaSeconds.startsWith("+") && sessionBestLiveDeltaSeconds != "+0.000") deltaColor = TFT_RED;
		drawDashboardValue(DashboardLayout::LEFT_X, DashboardLayout::DELTA_VALUE_Y,
			DashboardLayout::SIDE_WIDTH, DashboardLayout::DELTA_VALUE_H,
			sessionBestLiveDeltaSeconds, "gt3Delta", deltaColor, &fonts::FreeSans18pt7b,
			MC_DATUM, forceUpdate, 0.72f, 0.92f);

		// The gear datum is tied to the physical screen center, independent of side columns.
		drawCenteredGear(forceUpdate, gearSprite, gearSpriteCreated);
		drawGearShiftArcs(forceUpdate);
		drawTyreTemperatureStrip(DashboardLayout::TYRE_X, DashboardLayout::TYRE_Y,
			DashboardLayout::TYRE_W, DashboardLayout::TYRE_H, forceUpdate, 1);
		drawPedalProgress(DashboardLayout::BRAKE_BAR_X, DashboardLayout::PEDAL_BAR_Y,
			DashboardLayout::PEDAL_BAR_W, DashboardLayout::PEDAL_BAR_H,
			absLevel.toInt(), absFilteredLevel.toInt(), "gt3BrakeBar",
			tft.color565(218, 220, 224), forceUpdate);
		drawPedalProgress(DashboardLayout::THROTTLE_BAR_X, DashboardLayout::PEDAL_BAR_Y,
			DashboardLayout::PEDAL_BAR_W, DashboardLayout::PEDAL_BAR_H,
			tcLevel.toInt(), tcFilteredLevel.toInt(), "gt3ThrottleBar",
			tft.color565(218, 220, 224), forceUpdate);

		drawDashboardValue(DashboardLayout::RIGHT_TEXT_X, DashboardLayout::LAP_VALUE_Y[0],
			DashboardLayout::SIDE_WIDTH - DashboardLayout::INNER_PADDING, DashboardLayout::LAP_VALUE_H,
			bestLapTime, "gt3Best", TFT_WHITE, &fonts::FreeSans12pt7b,
			MC_DATUM, forceUpdate, 0.72f, 0.90f);
		drawDashboardValue(DashboardLayout::RIGHT_TEXT_X, DashboardLayout::LAP_VALUE_Y[1],
			DashboardLayout::SIDE_WIDTH - DashboardLayout::INNER_PADDING, DashboardLayout::LAP_VALUE_H,
			lastLapTime, "gt3Last", TFT_WHITE, &fonts::FreeSans12pt7b,
			MC_DATUM, forceUpdate, 0.72f, 0.90f);
		drawDashboardValueBuffered(DashboardLayout::RIGHT_TEXT_X, DashboardLayout::LAP_VALUE_Y[2],
			DashboardLayout::SIDE_WIDTH - DashboardLayout::INNER_PADDING, DashboardLayout::LAP_VALUE_H,
			currentLapTime, "gt3Current",
			lapInvalidated == "True" ? TFT_RED : TFT_WHITE, &fonts::FreeSans12pt7b,
			MC_DATUM, forceUpdate, currentLapSprite, currentLapSpriteCreated, 0.72f, 0.90f);

		const bool absOn = isActiveValue(absActive);
		const bool tcsOn = isActiveValue(tcActive);
		uint16_t leftColor = TFT_WHITE;
		if (tyrePressureFrontLeft != "--" && tyrePressureFrontLeft.toFloat() <= 1.0f)
			leftColor = TFT_RED;
		String lapCounterDisplay = tyrePressureRearLeft;
		lapCounterDisplay.replace("/", " / ");
		drawFuelIndicator(DashboardLayout::BOTTOM_X[0] + 4, DashboardLayout::BOTTOM_VALUE_Y,
			DashboardLayout::BOTTOM_W[0] - 8, DashboardLayout::BOTTOM_VALUE_H, brakeBias, forceUpdate);
		drawDashboardValue(DashboardLayout::BOTTOM_X[1] + 2, DashboardLayout::BOTTOM_VALUE_Y,
			DashboardLayout::BOTTOM_W[1] - 4, DashboardLayout::BOTTOM_VALUE_H,
			tyrePressureFrontLeft, "gt3Left", leftColor, &fonts::FreeSans12pt7b,
			MC_DATUM, forceUpdate, 0.82f, 0.80f);
		drawDashboardValue(DashboardLayout::BOTTOM_X[2] + 2, DashboardLayout::BOTTOM_VALUE_Y,
			DashboardLayout::BOTTOM_W[2] - 4, DashboardLayout::BOTTOM_VALUE_H,
			lapCounterDisplay, "gt3Lap", TFT_WHITE, &fonts::FreeSans12pt7b,
			MC_DATUM, forceUpdate, 0.56f, 0.74f);
		drawDashboardValue(DashboardLayout::BOTTOM_X[3] + 2, DashboardLayout::BOTTOM_VALUE_Y,
			DashboardLayout::BOTTOM_W[3] - 4, DashboardLayout::BOTTOM_VALUE_H,
			tyrePressureFrontRight, "gt3Pos", tft.color565(160, 60, 255), &fonts::FreeSans12pt7b,
			MC_DATUM, forceUpdate, 0.82f, 0.80f);
		drawCompactStatus(DashboardLayout::BOTTOM_X[4] + 2, DashboardLayout::BOTTOM_VALUE_Y,
			DashboardLayout::BOTTOM_W[4] - 4, DashboardLayout::BOTTOM_VALUE_H,
			"gt3Abs", absOn, tft.color565(57, 255, 20), forceUpdate);
		drawCompactStatus(DashboardLayout::BOTTOM_X[5] + 2, DashboardLayout::BOTTOM_VALUE_Y,
			DashboardLayout::BOTTOM_W[5] - 6, DashboardLayout::BOTTOM_VALUE_H,
			"gt3Tcs", tcsOn, tft.color565(0, 200, 255), forceUpdate);
	}

	DashboardTheme getDashboardTheme() const
	{
		return activeDashboardTheme;
	}

	const DashboardState &getDashboardState() const
	{
		return static_cast<const DashboardState &>(*this);
	}

	bool selectDashboardTheme(DashboardTheme theme, bool persist = true)
	{
		const uint8_t value = static_cast<uint8_t>(theme);
		if (!isValidDashboardTheme(value))
		{
			return false;
		}

		if (persist && dashboardPreferencesReady)
		{
			dashboardPreferences.putUChar("theme", value);
		}

		if (activeDashboardTheme != theme)
		{
			activeDashboardTheme = theme;
			renderedDashboardTheme = theme;
			invalidateDashboardRenderer();
		}

		return true;
	}

	void drawMeasuredText(const String &value, int32_t x, int32_t y,
		int32_t width, int32_t height, const lgfx::IFont *font, uint16_t color,
		textdatum_t datum, float scaleX = 1.0f, float scaleY = 1.0f)
	{
		tft.setTextPadding(0);
		tft.setTextColor(color, TFT_BLACK);
		tft.setTextSize(scaleX, scaleY);
		tft.setFont(font);

		const int32_t measuredWidth = tft.textWidth(value);
		const int32_t measuredHeight = tft.fontHeight();
		int32_t drawX = x;
		if (datum == MC_DATUM || datum == TC_DATUM || datum == BC_DATUM)
			drawX = x + (width - measuredWidth) / 2;
		else if (datum == MR_DATUM || datum == TR_DATUM || datum == BR_DATUM)
			drawX = x + width - measuredWidth;

		const int32_t drawY = y + (height - measuredHeight) / 2;
		tft.setTextDatum(TL_DATUM);
		tft.drawString(value, drawX, drawY);
		tft.setFont(&fonts::Font0);
		tft.setTextSize(1.0f);
		tft.setTextDatum(TL_DATUM);
	}

	void drawGearGaugeArcs(uint16_t color)
	{
		const int radiusOuter = DashboardLayout::GEAR_RADIUS;
		const int angleStep = 2;

		auto drawOneArc = [&](int startAngle, int endAngle, int radius)
		{
			float previousRadians = startAngle * DEG_TO_RAD;
			int previousX = DashboardLayout::GEAR_CENTER_X + lroundf(cosf(previousRadians) * radius);
			int previousY = DashboardLayout::GEAR_CENTER_Y + lroundf(sinf(previousRadians) * radius);
			for (int angle = startAngle + angleStep; angle <= endAngle; angle += angleStep)
			{
				const float radians = angle * DEG_TO_RAD;
				const int currentX = DashboardLayout::GEAR_CENTER_X + lroundf(cosf(radians) * radius);
				const int currentY = DashboardLayout::GEAR_CENTER_Y + lroundf(sinf(radians) * radius);
				tft.drawLine(previousX, previousY, currentX, currentY, color);
				previousX = currentX;
				previousY = currentY;
			}
		};

		// Equal 44-degree openings at twelve and six o'clock.
		// Two closely spaced traces create a smooth, readable gauge outline
		// without turning it into a heavy four-pixel bracket.
		for (int radius = radiusOuter; radius >= radiusOuter - 1; --radius)
		{
			drawOneArc(-68, 68, radius);
			drawOneArc(112, 248, radius);
		}
	}

	void drawGearShiftArcs(bool forceUpdate)
	{
		const String id = "gt3GearArcs";
		if (!forceUpdate && prevData[id] == gear) return;

		const int gearNumber = gear.toInt();
		const int radius = 54;
		const int segmentSweep = 18;
		const int segmentStep = 25;
		const int firstSegmentStart = 28;
		const uint16_t currentGearColor = tft.color565(72, 76, 82);
		const uint16_t otherGearColor = tft.color565(218, 220, 224);

		auto drawArcSegment = [&](int startAngle, int endAngle, uint16_t color)
		{
			for (int thickness = 0; thickness < 3; ++thickness)
			{
				const int r = radius - thickness;
				float previousRadians = startAngle * DEG_TO_RAD;
				int previousX = DashboardLayout::GEAR_CENTER_X + lroundf(cosf(previousRadians) * r);
				int previousY = DashboardLayout::GEAR_CENTER_Y + lroundf(sinf(previousRadians) * r);
				for (int angle = startAngle + 2; angle <= endAngle; angle += 2)
				{
					const float radians = angle * DEG_TO_RAD;
					const int currentX = DashboardLayout::GEAR_CENTER_X + lroundf(cosf(radians) * r);
					const int currentY = DashboardLayout::GEAR_CENTER_Y + lroundf(sinf(radians) * r);
					tft.drawLine(previousX, previousY, currentX, currentY, color);
					previousX = currentX;
					previousY = currentY;
				}
			}
		};

		// Segment zero starts at the bottom; higher gears illuminate upward.
		for (int segment = 0; segment < 4; ++segment)
		{
			const int rightStart = firstSegmentStart - segment * segmentStep;
			const int rightEnd = rightStart + segmentSweep;
			const bool marksCurrentGear = gearNumber >= 1 && gearNumber <= 4 &&
				segment == gearNumber - 1;
			const uint16_t color = marksCurrentGear ? currentGearColor : otherGearColor;
			drawArcSegment(rightStart, rightEnd, color);
			drawArcSegment(180 - rightEnd, 180 - rightStart, color);
		}

		prevData[id] = gear;
	}

	void drawDashboardValue(int32_t x, int32_t y, int32_t width, int32_t height,
		const String &value, const String &id, uint16_t color, const lgfx::IFont *font,
		textdatum_t datum, bool forceUpdate, float scaleX = 1.0f, float scaleY = 1.0f)
	{
		if (!forceUpdate && prevData[id] == value && prevColor[id] == color) return;
		tft.fillRect(x, y, width, height, TFT_BLACK);
		drawMeasuredText(value, x, y, width, height, font, color, datum, scaleX, scaleY);
		prevData[id] = value;
		prevColor[id] = color;
	}

	void drawDashboardValueBuffered(int32_t x, int32_t y, int32_t width, int32_t height,
		const String &value, const String &id, uint16_t color, const lgfx::IFont *font,
		textdatum_t datum, bool forceUpdate, LGFX_Sprite &sprite, bool &spriteCreated,
		float scaleX = 1.0f, float scaleY = 1.0f)
	{
		if (!forceUpdate && prevData[id] == value && prevColor[id] == color) return;

		if (!spriteCreated)
		{
			sprite.setColorDepth(16);
			spriteCreated = sprite.createSprite(width, height) != nullptr;
		}

		// Fall back safely if heap fragmentation prevents this small allocation.
		if (!spriteCreated)
		{
			drawDashboardValue(x, y, width, height, value, id, color, font,
				datum, forceUpdate, scaleX, scaleY);
			return;
		}

		sprite.fillSprite(TFT_BLACK);
		sprite.setTextPadding(0);
		sprite.setTextColor(color, TFT_BLACK);
		sprite.setTextSize(scaleX, scaleY);
		sprite.setFont(font);

		const int32_t measuredWidth = sprite.textWidth(value);
		const int32_t measuredHeight = sprite.fontHeight();
		int32_t drawX = 0;
		if (datum == MC_DATUM || datum == TC_DATUM || datum == BC_DATUM)
			drawX = (width - measuredWidth) / 2;
		else if (datum == MR_DATUM || datum == TR_DATUM || datum == BR_DATUM)
			drawX = width - measuredWidth;
		const int32_t drawY = (height - measuredHeight) / 2;

		sprite.setTextDatum(TL_DATUM);
		sprite.drawString(value, drawX, drawY);
		sprite.pushSprite(x, y);
		sprite.setFont(&fonts::Font0);
		sprite.setTextSize(1.0f);

		prevData[id] = value;
		prevColor[id] = color;
	}

	void drawCenteredGear(bool forceUpdate, LGFX_Sprite &sprite, bool &spriteCreated)
	{
		const String id = "gt3Gear";
		if (!forceUpdate && prevData[id] == gear) return;

		const int width = DashboardLayout::GEAR_VALUE_W;
		const int height = DashboardLayout::GEAR_VALUE_H;
		if (!spriteCreated)
		{
			sprite.setColorDepth(16);
			spriteCreated = sprite.createSprite(width, height) != nullptr;
		}

		if (!spriteCreated)
		{
			drawDashboardValue(DashboardLayout::GEAR_VALUE_X, DashboardLayout::GEAR_VALUE_Y,
				width, height, gear, id, TFT_WHITE, &fonts::DejaVu72,
				MC_DATUM, forceUpdate, 1.25f, 1.25f);
			return;
		}

		sprite.setTextPadding(0);
		sprite.setTextColor(TFT_WHITE, TFT_BLACK);
		sprite.setTextSize(1.25f, 1.25f);
		sprite.setFont(&fonts::DejaVu72);
		sprite.setTextDatum(TL_DATUM);

		const int initialX = (width - sprite.textWidth(gear)) / 2;
		const int initialY = (height - sprite.fontHeight()) / 2;
		sprite.fillSprite(TFT_BLACK);
		sprite.drawString(gear, initialX, initialY);

		int minX = width;
		int minY = height;
		int maxX = -1;
		int maxY = -1;
		for (int py = 0; py < height; ++py)
		{
			for (int px = 0; px < width; ++px)
			{
				if (sprite.readPixel(px, py) == TFT_BLACK) continue;
				minX = min(minX, px);
				maxX = max(maxX, px);
				minY = min(minY, py);
				maxY = max(maxY, py);
			}
		}

		int correctedX = initialX;
		int correctedY = initialY;
		if (maxX >= minX && maxY >= minY)
		{
			correctedX += (width - 1 - minX - maxX) / 2;
			correctedY += (height - 1 - minY - maxY) / 2;
		}

		sprite.fillSprite(TFT_BLACK);
		sprite.drawString(gear, correctedX, correctedY);
		sprite.pushSprite(DashboardLayout::GEAR_VALUE_X, DashboardLayout::GEAR_VALUE_Y);
		sprite.setFont(&fonts::Font0);
		sprite.setTextSize(1.0f);
		prevData[id] = gear;
	}

	void drawFuelIndicator(int32_t x, int32_t y, int32_t width, int32_t height,
		const String &value, bool forceUpdate)
	{
		const String id = "gt3Fuel";
		uint16_t color = TFT_WHITE;
		if (value != "--")
		{
			const float fuelPercent = value.toFloat();
			if (fuelPercent <= 9.0f) color = TFT_RED;
			else if (fuelPercent <= 24.0f) color = TFT_YELLOW;
		}
		const String state = value + ":" + String(color);
		if (!forceUpdate && prevData[id] == state) return;

		tft.fillRect(x, y, width, height, TFT_BLACK);
		const int32_t pumpX = x + 2;
		const int32_t pumpHeight = 18;
		const int32_t pumpY = y + (height - pumpHeight) / 2 - 1;

		// Allocation-free monochrome fuel pump: body, window, base and hose.
		tft.drawRect(pumpX, pumpY, 8, 13, color);
		tft.drawRect(pumpX + 2, pumpY + 2, 4, 4, color);
		tft.drawFastHLine(pumpX - 1, pumpY + 17, 11, color);
		tft.drawFastVLine(pumpX + 10, pumpY + 4, 8, color);
		tft.drawLine(pumpX + 7, pumpY + 2, pumpX + 10, pumpY + 5, color);
		tft.drawLine(pumpX + 10, pumpY + 12, pumpX + 12, pumpY + 15, color);

		const int32_t valueX = x + 16;
		const int32_t valueWidth = width - 16;
		drawMeasuredText(value, valueX, y, valueWidth, height,
			&fonts::FreeSans12pt7b, color, MC_DATUM, 0.70f, 0.80f);
		prevData[id] = state;
	}

	void drawCompactStatus(int32_t x, int32_t y, int32_t width, int32_t height,
		const String &id, bool active, uint16_t activeColor, bool forceUpdate)
	{
		const String state = active ? "1" : "0";
		if (!forceUpdate && prevData[id] == state) return;
		tft.fillRect(x, y, width, height, TFT_BLACK);
		const uint16_t color = activeColor;
		const int32_t centerY = y + (height - 1) / 2 - 2;
		tft.drawCircle(x + width / 2, centerY, 7, color);
		if (active) tft.fillCircle(x + width / 2, centerY, 4, color);
		prevData[id] = state;
	}

	void drawPedalProgress(int32_t x, int32_t y, int32_t width, int32_t height,
		int inputPercent, int appliedPercent, const String &id,
		uint16_t appliedColor, bool forceUpdate)
	{
		inputPercent = constrain(inputPercent, 0, 100);
		appliedPercent = constrain(appliedPercent, 0, 100);
		const String state = String(inputPercent) + ":" + String(appliedPercent);
		if (!forceUpdate && prevData[id] == state) return;

		const int innerWidth = width - 2;
		const int innerHeight = height - 2;
		const int inputFill = (inputPercent * innerHeight + 99) / 100;
		const int appliedFill = (appliedPercent * innerHeight + 99) / 100;
		const uint16_t inputColor = tft.color565(180, 62, 66);

		// Compose the complete 17x25 bar off-screen and push it once. This prevents
		// the clear/background/foreground stages being visible on the SPI panel.
		static LGFX_Sprite pedalSprite(&tft);
		static bool pedalSpriteCreated = false;
		if (!pedalSpriteCreated)
		{
			pedalSprite.setColorDepth(16);
			pedalSprite.createSprite(DashboardLayout::PEDAL_BAR_W,
				DashboardLayout::PEDAL_BAR_H);
			pedalSpriteCreated = true;
		}

		pedalSprite.fillSprite(TFT_BLACK);
		if (inputPercent >= 100)
			pedalSprite.fillRoundRect(0, 0, width, height,
				DashboardLayout::TYRE_CORNER_RADIUS, inputColor);
		else if (inputFill > 0)
			pedalSprite.fillRect(1, 1 + innerHeight - inputFill,
				innerWidth, inputFill, inputColor);
		if (appliedPercent >= 100)
			pedalSprite.fillRoundRect(0, 0, width, height,
				DashboardLayout::TYRE_CORNER_RADIUS, appliedColor);
		else if (appliedFill > 0)
			pedalSprite.fillRect(1, 1 + innerHeight - appliedFill,
				innerWidth, appliedFill, appliedColor);
		pedalSprite.drawRoundRect(0, 0, width, height,
			DashboardLayout::TYRE_CORNER_RADIUS, tft.color565(82, 86, 92));
		pedalSprite.pushSprite(x, y);

		prevData[id] = state;
	}

	void drawRpmMeterGT3(bool forceUpdate)
	{
		// Shift-warning tuning: alert onset starts at peak brightness.
		static constexpr float RPM_SHIFT_PULSE_FREQUENCY_HZ = 3.0f;
		static constexpr float RPM_SHIFT_PULSE_MIN_WHITE_MIX = 0.25f;
		static constexpr float RPM_SHIFT_PULSE_NEAR_MAX_WHITE_MIX = 0.45f;
		static constexpr float RPM_SHIFT_PULSE_REV_ALERT_WHITE_MIX = 0.60f;
		static constexpr uint32_t RPM_SHIFT_PULSE_FRAME_INTERVAL_MS = 16;

		const int segmentCount = 36;
		const int segmentPitch = 8;
		const int active = constrain((rpmPercent * segmentCount + 99) / 100, 0, segmentCount);
		const int previous = forceUpdate ? -1 : constrain((prev_rpmPercent * segmentCount + 99) / 100, 0, segmentCount);
		const String gradientState = String(rpmRedLineSetting) + ":" +
			String(rpmAlertRangeValid ? 1 : 0);
		const bool redlineChanged = prevData["gt3Redline"] != gradientState;
		const int displayedRpm = ((engineRpm + 25) / 50) * 50;
		const String rpmValueText = String(displayedRpm);
		const uint32_t now = millis();
		const bool rpmWarningActive = rpmAlertRangeValid &&
			rpmPercent >= rpmRedLineSetting;
		const bool pulseActive = rpmWarningActive || revLimitAlertActive;
		const bool revAlertStarted = revLimitAlertActive && !revLimitAlertWasActive;
		const bool pulseStarted = (pulseActive && !rpmPulseWasActive) || revAlertStarted;
		const bool pulseEnded = !pulseActive && rpmPulseWasActive;
		const String rpmTextState = rpmValueText + ":" + String(pulseActive ? 1 : 0);
		const bool rpmTextChanged = forceUpdate ||
			prevData["gt3RpmValue"] != rpmTextState;
		if (pulseStarted) rpmPulseStartTime = now;

		float pulsePeakWhiteMix = RPM_SHIFT_PULSE_MIN_WHITE_MIX;
		if (rpmWarningActive)
		{
			const float warningSpan = max(1, 100 - rpmRedLineSetting);
			const float warningProgress = constrain(
				(rpmPercent - rpmRedLineSetting) / warningSpan, 0.0f, 1.0f);
			pulsePeakWhiteMix +=
				(RPM_SHIFT_PULSE_NEAR_MAX_WHITE_MIX - RPM_SHIFT_PULSE_MIN_WHITE_MIX) *
				warningProgress;
		}
		if (revLimitAlertActive)
			pulsePeakWhiteMix = RPM_SHIFT_PULSE_REV_ALERT_WHITE_MIX;

		uint8_t pulseWhiteMix = 0;
		bool pulseFrameChanged = false;
		if (pulseActive &&
			(pulseStarted || now - lastRpmPulseFrameTime >= RPM_SHIFT_PULSE_FRAME_INTERVAL_MS))
		{
			const float elapsedSeconds = (now - rpmPulseStartTime) / 1000.0f;
			const float pulse = 0.5f + 0.5f * cosf(
				TWO_PI * RPM_SHIFT_PULSE_FREQUENCY_HZ * elapsedSeconds);
			pulseWhiteMix = static_cast<uint8_t>(lroundf(
				255.0f * pulsePeakWhiteMix * pulse));
			pulseFrameChanged = pulseStarted || pulseWhiteMix != lastRpmPulseWhiteMix;
			lastRpmPulseFrameTime = now;
		}
		else if (pulseActive)
		{
			pulseWhiteMix = lastRpmPulseWhiteMix;
		}

		if (!forceUpdate && active == previous && !redlineChanged &&
			!pulseFrameChanged && !pulseEnded && !rpmTextChanged) return;
		const float minAlertPosition = rpmAlertRangeValid
			? constrain(rpmRedLineSetting / 100.0f, 0.30f, 0.95f)
			: 0.80f;
		const float gradientStops[] = {
			0.0f,
			minAlertPosition * 0.25f,
			minAlertPosition * 0.50f,
			minAlertPosition * 0.75f,
			minAlertPosition,
			1.0f};
		const uint8_t gradientRgb[][3] = {
			{0, 112, 255}, {0, 218, 255}, {35, 232, 118},
			{255, 220, 0}, {255, 112, 0}, {255, 20, 28}};
		const uint16_t inactiveColor = tft.color565(0x30, 0x38, 0x3C);
		const uint16_t minAlertMarkerColor = tft.color565(0xFF, 0xD4, 0x3B);
		const int firstSegmentCenterX = DashboardLayout::CENTER_X -
			(segmentCount - 1) * segmentPitch / 2;
		const int lastSegmentCenterX = DashboardLayout::CENTER_X +
			(segmentCount - 1) * segmentPitch / 2;
		const int minAlertMarkerX = rpmAlertRangeValid
			? firstSegmentCenterX + lroundf(
				(lastSegmentCenterX - firstSegmentCenterX) * minAlertPosition)
			: -1;
		const int previousMinAlertMarkerX = prevData["gt3MinAlertMarkerX"].toInt();
		if (redlineChanged && previousMinAlertMarkerX > 0)
		{
			const int oldDx = previousMinAlertMarkerX - DashboardLayout::CENTER_X;
			const int oldBarY = 15 + (oldDx * oldDx) / 1800;
			tft.fillRect(previousMinAlertMarkerX - 3, oldBarY - 10, 7, 31, TFT_BLACK);
		}

		for (int i = 0; i < segmentCount; ++i)
		{
			const bool rpmStateChanged = forceUpdate || redlineChanged ||
				(i >= min(active, previous) && i < max(active, previous));
			const bool pulseStateChanged = i < active && (pulseFrameChanged || pulseEnded);
			if (!rpmStateChanged && !pulseStateChanged) continue;
			const float position = i / static_cast<float>(segmentCount - 1);
			int stop = 0;
			while (stop < 4 && position > gradientStops[stop + 1]) ++stop;
			const float span = gradientStops[stop + 1] - gradientStops[stop];
			const float blend = constrain(
				(position - gradientStops[stop]) / span, 0.0f, 1.0f);
			int red = lroundf(gradientRgb[stop][0] +
				(gradientRgb[stop + 1][0] - gradientRgb[stop][0]) * blend);
			int green = lroundf(gradientRgb[stop][1] +
				(gradientRgb[stop + 1][1] - gradientRgb[stop][1]) * blend);
			int blue = lroundf(gradientRgb[stop][2] +
				(gradientRgb[stop + 1][2] - gradientRgb[stop][2]) * blend);
			uint16_t color = inactiveColor;
			if (i < active)
			{
				if (pulseActive)
				{
					red += (255 - red) * pulseWhiteMix / 255;
					green += (255 - green) * pulseWhiteMix / 255;
					blue += (255 - blue) * pulseWhiteMix / 255;
				}
				color = tft.color565(red, green, blue);
			}
			// Test geometry: every segment is the same unrotated rectangle. Only its
			// centre follows the existing gentle symmetric parabola.
			const int segmentCenterX = DashboardLayout::CENTER_X +
				(2 * i - (segmentCount - 1)) * segmentPitch / 2;
			const int dx = segmentCenterX - DashboardLayout::CENTER_X;
			const int segmentY = 15 + (dx * dx) / 1800;
			const int segmentWidth = 6;
			const int thickness = 18;
			tft.fillRect(segmentCenterX - segmentWidth / 2, segmentY,
				segmentWidth, thickness, color);
		}
		if (rpmAlertRangeValid)
		{
			const int markerDx = minAlertMarkerX - DashboardLayout::CENTER_X;
			const int markerBarY = 15 + (markerDx * markerDx) / 1800;
			const int markerTipY = markerBarY - 5;
			tft.fillTriangle(
				minAlertMarkerX - 3, markerTipY - 5,
				minAlertMarkerX + 3, markerTipY - 5,
				minAlertMarkerX, markerTipY,
				minAlertMarkerColor);
			tft.drawFastVLine(
				minAlertMarkerX,
				markerTipY + 1,
				markerBarY + 20 - markerTipY,
				minAlertMarkerColor);
		}
		if (rpmTextChanged)
		{
			const uint16_t rpmTextColor = pulseActive
				? tft.color565(255, 90, 69)
				: tft.color565(160, 163, 168);
			static LGFX_Sprite rpmTextSprite(&tft);
			static bool rpmTextSpriteCreated = false;
			if (!rpmTextSpriteCreated)
			{
				rpmTextSprite.setColorDepth(16);
				rpmTextSpriteCreated = rpmTextSprite.createSprite(110, 10) != nullptr;
			}
			if (rpmTextSpriteCreated)
			{
				rpmTextSprite.fillSprite(TFT_BLACK);
				rpmTextSprite.setTextColor(rpmTextColor, TFT_BLACK);
				rpmTextSprite.setFont(&fonts::FreeSans9pt7b);
				rpmTextSprite.setTextSize(0.52f, 0.50f);
				rpmTextSprite.setTextDatum(MR_DATUM);
				rpmTextSprite.drawString(rpmValueText, 53, 5);
				rpmTextSprite.setTextDatum(ML_DATUM);
				rpmTextSprite.drawString("RPM", 57, 5);
				rpmTextSprite.pushSprite(DashboardLayout::CENTER_X - 55, 40);
			}
			else
			{
				tft.fillRect(DashboardLayout::CENTER_X - 55, 40, 110, 10, TFT_BLACK);
				drawMeasuredText(rpmValueText, DashboardLayout::CENTER_X - 50, 40, 48, 10,
					&fonts::FreeSans9pt7b, rpmTextColor, MR_DATUM, 0.52f, 0.50f);
				drawMeasuredText("RPM", DashboardLayout::CENTER_X + 2, 40, 48, 10,
					&fonts::FreeSans9pt7b, rpmTextColor, ML_DATUM, 0.52f, 0.50f);
			}
			prevData["gt3RpmValue"] = rpmTextState;
		}
		prev_rpmPercent = rpmPercent;
		prevData["gt3Redline"] = gradientState;
		prevData["gt3MinAlertMarkerX"] = String(minAlertMarkerX);
		revLimitAlertWasActive = revLimitAlertActive;
		rpmPulseWasActive = pulseActive;
		lastRpmPulseWhiteMix = pulseActive ? pulseWhiteMix : 0;
	}

	void drawPage1Legacy(bool forceUpdate = false)
	{
		drawRpmMeter(0, 0, SCREEN_WIDTH, HALF_CELL_HEIGHT);
		drawGear(COL[2], COL[1]);
		drawTyreTemperatureStrip(
			COL[2] + 1,
			ROW[3] - 22,
			CELL_WIDTH - 2,
			18,
			forceUpdate);

		drawLapCellSprite(
			COL[0],
			ROW[1],
			bestLapTime,
			"bestLapTime",
			"Best Lap",
			TFT_WHITE,
			forceUpdate);

		drawLapCellSprite(
			COL[0],
			ROW[2],
			lastLapTime,
			"lastLapTime",
			"Last Lap",
			TFT_WHITE,
			forceUpdate);

		drawLapCellSprite(
			COL[0],
			ROW[3],
			currentLapTime,
			"currentLapTime",
			"Current Lap",
			lapInvalidated == "True" ? TFT_RED : TFT_WHITE,
			forceUpdate);

		// Third Column (speed)
		drawCell(COL[2], ROW[3], speed, "speed", "Speed", "center", TFT_WHITE, 4, forceUpdate);

		// 右上：上一圈相對最佳圈的差值。
		uint16_t lastBestColor = TFT_WHITE;
		if (sessionBestLiveDeltaSeconds.startsWith("-"))
		{
			lastBestColor = TFT_GREEN;
		}
		else if (
			sessionBestLiveDeltaSeconds.startsWith("+") &&
			sessionBestLiveDeltaSeconds != "+0.000")
		{
			lastBestColor = TFT_RED;
		}
		drawCell(
			SCREEN_WIDTH,
			ROW[1],
			sessionBestLiveDeltaSeconds,
			"lastBestDifference",
			"Delta",
			"right",
			lastBestColor,
			4,
			forceUpdate);

		// 右中：油門與煞車改成上下兩個獨立格，整個格子本身就是 progress。
		drawPedalProgressCells(
			SCREEN_WIDTH - CELL_WIDTH * 2 + 1,
			ROW[2],
			CELL_WIDTH * 2 - 2,
			CELL_HEIGHT - 1,
			constrain(tcLevel.toInt(), 0, 100),
			constrain(tcFilteredLevel.toInt(), 0, 100),
			constrain(absLevel.toInt(), 0, 100),
			constrain(absFilteredLevel.toInt(), 0, 100),
			forceUpdate);

		// ABS、TCS 狀態與油量
		const bool tcsOn = isActiveValue(tcActive);
		const bool absOn = isActiveValue(absActive);
		const uint16_t tcsColor = tcsOn ? TFT_YELLOW : TFT_DARKGREY;
		const uint16_t absColor = absOn ? TFT_RED : TFT_DARKGREY;
		float fuelPercentValue = fuelAlertActive.toFloat();

		// Fuel
		String fuelStatus;
		uint16_t fuelStatusColor;

		if (fuelPercentValue > 50.0f)
		{
			fuelStatus = "HI";
			fuelStatusColor = TFT_CYAN;
		}
		else if (fuelPercentValue > 25.0f)
		{
			fuelStatus = "OK";
			fuelStatusColor = TFT_CYAN;
		}
		else if (fuelPercentValue > 10.0f)
		{
			fuelStatus = "LOW";
			fuelStatusColor = TFT_ORANGE;
		}
		else if (fuelPercentValue > 0.0f)
		{
			fuelStatus = "PIT";
			fuelStatusColor = TFT_RED;
		}
		else
		{
			fuelStatus = "--";
			fuelStatusColor = TFT_DARKGREY;
		}

		// 左下兩格：保留 ABS/TCS 標題，使用狀態圓燈取代大型 ON/OFF 文字。
		drawStatusIndicatorCell(
			COL[0],
			ROW[4],
			CELL_WIDTH,
			CELL_HEIGHT,
			"ABS",
			"absStatus",
			absOn,
			TFT_RED,
			forceUpdate);

		drawStatusIndicatorCell(
			COL[1],
			ROW[4],
			CELL_WIDTH,
			CELL_HEIGHT,
			"TCS",
			"tcsStatus",
			tcsOn,
			TFT_RED,
			forceUpdate);
		drawCell(COL[2], ROW[4], brakeBias, "brakeBias", "FUEL", "center", TFT_MAGENTA, 4, forceUpdate);

		// 剩餘圈數、位置、圈數、低油量警示
		drawCell(COL[3], ROW[3], tyrePressureFrontLeft, "tyrePressureFrontLeft", "REM", "center", TFT_CYAN, 4, forceUpdate);
		drawCell(COL[4], ROW[3], tyrePressureFrontRight, "tyrePressureFrontRight", "POS", "center", TFT_CYAN, 4, forceUpdate);
		drawCell(COL[3], ROW[4], tyrePressureRearLeft, "tyrePressureRearLeft", "LAP", "center", TFT_CYAN, 4, forceUpdate);
		drawCell(
			COL[4],
			ROW[4],
			fuelStatus,
			"fuelAlert",
			"F/ST",
			"center",
			fuelStatusColor,
			4,
			forceUpdate);
	}

	void drawColoredButton(int x, int y, int width, int height, String label, uint16_t color) // disegna i pulsanti della pagina 2
	{
		// Draw the dark background for the button
		tft.fillRoundRect(x, y, width, height, 10, TFT_BLACK);

		// Draw the top and bottom color edges
		tft.fillRoundRect(x + 2, y + 14, width - 4, height / 5, 10, color);						  // Top edge
		tft.fillRoundRect(x + 2, y + height - height / 5 - 14, width - 4, height / 5, 10, color); // Bottom edge

		// Draw the inner color
		tft.fillRoundRect(x + 2, y + height / 5 + 2, width - 4, height * 3 / 5 - 4, 10, TFT_PURPLE);

		// Draw the label text in white
		tft.setTextColor(TFT_WHITE, TFT_PURPLE);
		tft.setTextDatum(MC_DATUM);
		tft.drawString(label, x + width / 2, y + height / 2);
		tft.setTextDatum(TL_DATUM);
	}

	void drawPage2() // pagina pulsanti
	{
		int buttonWidth = SCREEN_WIDTH / 3;
		int buttonHeight = SCREEN_HEIGHT / 2;
		String buttonLabels[] = {"In-game", "Offline", "Pit", "TC", "ABS", "Streamig"}; // Sostituisci con le etichette desiderate
		uint16_t buttonColors[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_ORANGE, TFT_PURPLE};

		for (int i = 0; i < numButtons; i++)
		{
			int x = (i % 3) * buttonWidth;
			int y = (i / 3) * buttonHeight;
			drawColoredButton(x, y, buttonWidth, buttonHeight, buttonLabels[i], buttonColors[i]);
		}
	}

	void drawGear(int32_t x, int32_t y)
	{
		// draw gear only when it changes
		if (gear != prev_gear)
		{
			// tft.loadFont("Formula1_Regular_web_072pt7b", SPIFFS);
			tft.setTextColor(TFT_YELLOW, TFT_BLACK);
			tft.setTextSize(8);
			tft.setTextDatum(MC_DATUM);
			tft.setCursor(x + 12, y + HALF_CELL_HEIGHT);
			tft.print(gear);
			tft.setTextSize(1);
			tft.setTextDatum(TL_DATUM);

			prev_gear = gear;
		}
	}

	boolean isDrawGearRpmRedRec()
	{
		if (rpmPercent >= rpmRedLineSetting)
		{
			return true;
		}
		return false;
	}

	void drawRpmMeter(int32_t x, int32_t y, int width, int height)
	{
		const int borderSize = 2;
		const int frameHeight = height - 2;

		const int innerX = x + borderSize;
		const int innerY = y + borderSize;
		const int innerWidth = width - borderSize * 2;
		const int innerHeight = frameHeight - borderSize * 2;

		const int segmentCount = 20;
		const int segmentGap = 2;

		const int safeRpmPercent =
			constrain(rpmPercent, 0, 100);

		const int safePrevRpmPercent =
			constrain(prev_rpmPercent, 0, 100);

		// 目前應亮幾格
		int activeSegments =
			(safeRpmPercent * segmentCount + 99) / 100;

		activeSegments =
			constrain(activeSegments, 0, segmentCount);

		// 上一幀亮幾格
		int previousActiveSegments =
			(safePrevRpmPercent * segmentCount + 99) / 100;

		previousActiveSegments =
			constrain(previousActiveSegments, 0, segmentCount);

		const int totalGapWidth =
			segmentGap * (segmentCount - 1);

		const int segmentWidth =
			(innerWidth - totalGapWidth) / segmentCount;

		const int markerX = rpmAlertRangeValid
			? innerX + lroundf((innerWidth - 1) *
				constrain(rpmRedLineSetting / 100.0f, 0.0f, 1.0f))
			: -1;
		const int previousMarkerX = prevData["classicMinAlertMarkerX"].toInt();
		const bool markerChanged = markerX != previousMarkerX;
		const bool fullRedraw = forceUpdate || markerChanged;

		/*
		 * 首次繪製或切回主畫面時：
		 * 清一次內部，再完整畫出目前的 Bar。
		 */
		if (fullRedraw)
		{
			tft.fillRect(
				x,
				y,
				width,
				frameHeight + 10,
				TFT_BLACK);

			previousActiveSegments = 0;

			tft.drawRect(
				x,
				y,
				width,
				frameHeight,
				TFT_WHITE);
		}

		/*
		 * RPM 上升：
		 * 只畫新增加的格子。
		 */
		if (activeSegments > previousActiveSegments)
		{
			for (
				int i = previousActiveSegments;
				i < activeSegments;
				i++)
			{
				const int segmentX =
					innerX +
					i * (segmentWidth + segmentGap);

				const int segmentPercent =
					((i + 1) * 100) / segmentCount;

				uint16_t segmentColor;

				if (segmentPercent >= rpmRedLineSetting)
				{
					segmentColor = TFT_RED;
				}
				else if (
					segmentPercent >=
					rpmRedLineSetting - 5)
				{
					segmentColor = TFT_ORANGE;
				}
				else
				{
					segmentColor = TFT_GREEN;
				}

				tft.fillRect(
					segmentX,
					innerY,
					segmentWidth,
					innerHeight,
					segmentColor);
			}
		}

		/*
		 * RPM 下降：
		 * 只清除消失的格子。
		 *
		 * 不清整條，因此不會出現黑一下再重畫。
		 */
		else if (activeSegments < previousActiveSegments)
		{
			for (
				int i = activeSegments;
				i < previousActiveSegments;
				i++)
			{
				const int segmentX =
					innerX +
					i * (segmentWidth + segmentGap);

				tft.fillRect(
					segmentX,
					innerY,
					segmentWidth,
					innerHeight,
					TFT_BLACK);
			}
		}

		/*
		 * 若 RPM 仍在同一個分段內，不做任何繪圖。
		 * 例如 51% → 54% 都是同樣格數，畫面完全不更新。
		 */

		if (markerX >= 0)
		{
			const uint16_t markerColor = tft.color565(255, 52, 42);
			const int arrowTipY = y + frameHeight + 1;
			tft.drawFastVLine(markerX, y, arrowTipY - y + 1, markerColor);
			tft.fillTriangle(markerX, arrowTipY,
				markerX - 6, y + frameHeight + 9,
				markerX + 6, y + frameHeight + 9,
				markerColor);
		}

		prev_rpmPercent = safeRpmPercent;
		prevData["classicMinAlertMarkerX"] = String(markerX);
	}

	void drawRpmMeter2(int32_t x, int32_t y, int width, int height)
	{
		const int borderSize = 2;

		const int frameHeight = height - 2;

		const int innerX = x + borderSize;
		const int innerY = y + borderSize;

		const int innerWidth = width - borderSize * 2;
		const int innerHeight = frameHeight - borderSize * 2;

		int safeRpmPercent = constrain(rpmPercent, 0, 100);

		int meterWidth =
			(innerWidth * safeRpmPercent) / 100;

		/*
		 * RPM 下降時，只清除新長度後面的區域。
		 * 所有座標都要以 innerX 為基準。
		 */
		if (prev_rpmPercent > safeRpmPercent)
		{
			int clearX = innerX + meterWidth;
			int clearWidth = innerWidth - meterWidth;

			if (clearWidth > 0)
			{
				tft.fillRect(
					clearX,
					innerY,
					clearWidth,
					innerHeight,
					TFT_BLACK);
			}
		}

		uint16_t meterColor;

		if (safeRpmPercent >= rpmRedLineSetting)
		{
			meterColor = TFT_RED;
		}
		else if (safeRpmPercent >= rpmRedLineSetting - 5)
		{
			meterColor = TFT_ORANGE;
		}
		else
		{
			meterColor = TFT_GREEN;
		}

		/*
		 * 填色從 innerX 開始，避免蓋到左側白框。
		 */
		if (meterWidth > 0)
		{
			tft.fillRect(
				innerX,
				innerY,
				meterWidth,
				innerHeight,
				meterColor);
		}

		/*
		 * 首次繪製或完整重畫時重畫外框。
		 */
		if (prev_rpmPercent == 50 || forceUpdate)
		{
			tft.drawRect(
				x,
				y,
				width,
				frameHeight,
				TFT_WHITE);
		}

		prev_rpmPercent = safeRpmPercent;
	}

	void drawCell2(int32_t x, int32_t y, String data, String id, String name = "Data", String align = "center", int32_t color = TFT_WHITE, int fontSize = 4, bool forceUpdate = false)
	{
		const static int titleHeight = 19;
		const static int hPadding = 5;
		const static int vPadding = 1;

		tft.setTextColor(color, TFT_BLACK);

		bool dataChanged = (prevData[id] != data) || forceUpdate;
		bool colorChanged = (prevColor[id] != color) || forceUpdate;

		if (dataChanged || colorChanged)
		{

			if (align == "left")
			{

				if (colorChanged)
					tft.drawRoundRect(x, y, CELL_WIDTH * 2 - 1, CELL_HEIGHT - 2, 5, color); // Rectangle
				if (colorChanged)
					tft.drawString(name, x + hPadding, y + vPadding, 2);	   // Title
				tft.drawString(data, x + hPadding, y + titleHeight, fontSize); // Data
			}
			else if (align == "right")
			{
				if (colorChanged)
					tft.drawRoundRect(x - (CELL_WIDTH * 2), y, CELL_WIDTH * 2 - 1, CELL_HEIGHT - 2, 5, color); // Rectangle
				if (colorChanged)
					tft.drawRightString(name, x - hPadding, y + vPadding, 2);		// Title
				tft.drawRightString(data, x - hPadding, y + titleHeight, fontSize); // Data
			}
			else // "center"
			{
				if (colorChanged)
					tft.drawRoundRect(x, y, CELL_WIDTH - 2, CELL_HEIGHT - 2, 5, color); // Rectangle
				if (colorChanged)
					tft.drawCentreString(name, x + HALF_CELL_WIDTH, y + vPadding, 2);		// Title
				tft.drawCentreString(data, x + HALF_CELL_WIDTH, y + titleHeight, fontSize); // Data
			}

			// Clean the previous speed if it was wider
			if (prevData[id].length() > data.length())
			{
				tft.setTextColor(TFT_BLACK, TFT_BLACK);
				if (align == "left")
				{
					tft.drawString(prevData[id], x + hPadding, y + titleHeight, fontSize);
				}
				else if (align == "right")
				{
					tft.drawRightString(prevData[id], x - hPadding, y + titleHeight, fontSize);
				}
				else
				{
					tft.drawCentreString(prevData[id], x + HALF_CELL_WIDTH, y + titleHeight, fontSize);
				}
			}

			prevData[id] = data;
			prevColor[id] = color;
		}
	}

	uint16_t interpolateTyreColor(
		uint8_t redStart,
		uint8_t greenStart,
		uint8_t blueStart,
		uint8_t redEnd,
		uint8_t greenEnd,
		uint8_t blueEnd,
		float amount)
	{
		amount = constrain(amount, 0.0f, 1.0f);

		return tft.color565(
			static_cast<uint8_t>(redStart + (redEnd - redStart) * amount),
			static_cast<uint8_t>(greenStart + (greenEnd - greenStart) * amount),
			static_cast<uint8_t>(blueStart + (blueEnd - blueStart) * amount));
	}

	uint16_t tyreTemperatureColor(float temperature)
	{
		if (!isfinite(temperature) || temperature <= 0.0f)
		{
			return TFT_DARKGREY;
		}

		if (temperature <= 50.0f)
		{
			return tft.color565(0, 20, 120);
		}

		if (temperature < 65.0f)
		{
			return interpolateTyreColor(
				0, 20, 120,
				0, 200, 255,
				(temperature - 50.0f) / 15.0f);
		}

		if (temperature < 85.0f)
		{
			return interpolateTyreColor(
				0, 200, 255,
				0, 220, 60,
				(temperature - 65.0f) / 20.0f);
		}

		if (temperature < 92.5f)
		{
			return interpolateTyreColor(
				0, 220, 60,
				255, 220, 0,
				(temperature - 85.0f) / 7.5f);
		}

		if (temperature < 100.0f)
		{
			return interpolateTyreColor(
				255, 220, 0,
				255, 0, 0,
				(temperature - 92.5f) / 7.5f);
		}

		return TFT_RED;
	}

	void drawTyreTemperatureStrip(
		int32_t x,
		int32_t y,
		int width,
		int height,
		bool forceUpdate = false,
		uint8_t gap = 2)
	{
		const int blockWidth = (width - gap) / 2;
		const int blockHeight = (height - gap) / 2;
		const String gearId = "tyreTemperatureGear";
		const bool gearChanged = prevData[gearId] != gear;

		for (int tyreIndex = 0; tyreIndex < 4; tyreIndex++)
		{
			const int column = tyreIndex % 2;
			const int row = tyreIndex / 2;
			const int blockX = x + column * (blockWidth + gap);
			const int blockY = y + row * (blockHeight + gap);
			const int currentWidth = column == 1
				? width - blockWidth - gap
				: blockWidth;
			const int currentHeight = row == 1
				? height - blockHeight - gap
				: blockHeight;
			const float temperature = tyreTemperatures[tyreIndex];
			const uint16_t color = tyreTemperatureColor(temperature);
			const String colorId = "tyreTemperatureColor" + String(tyreIndex);

			if (!forceUpdate && !gearChanged && prevColor[colorId] == color)
			{
				continue;
			}

			tft.fillRoundRect(
				blockX,
				blockY,
				currentWidth,
				currentHeight,
				DashboardLayout::TYRE_CORNER_RADIUS,
				color);

			prevColor[colorId] = color;
		}

		prevData[gearId] = gear;
	}

	void drawLapCellSprite(
		int32_t x,
		int32_t y,
		const String &data,
		const String &id,
		const String &name,
		uint16_t color,
		bool forceUpdate = false)
	{
		const int cellWidth = CELL_WIDTH * 2 - 1;
		const int cellHeight = CELL_HEIGHT - 2;

		bool dataChanged =
			prevData[id] != data || forceUpdate;

		bool colorChanged =
			prevColor[id] != color || forceUpdate;

		if (!dataChanged && !colorChanged)
		{
			return;
		}

		// 共用一張離屏畫布
		static LGFX_Sprite sprite(&tft);
		static bool spriteCreated = false;

		if (!spriteCreated)
		{
			sprite.setColorDepth(16);
			sprite.createSprite(cellWidth, cellHeight);
			spriteCreated = true;
		}

		// 所有內容先在記憶體裡畫好
		sprite.fillSprite(TFT_BLACK);

		sprite.drawRoundRect(
			0,
			0,
			cellWidth,
			cellHeight,
			5,
			color);

		sprite.setTextColor(color, TFT_BLACK);
		sprite.setTextDatum(TL_DATUM);

		// 標題
		sprite.drawString(
			name,
			5,
			1,
			2);

		// 計時數字
		sprite.drawString(
			data,
			5,
			19,
			4);

		// 完整畫面一次貼上 TFT
		sprite.pushSprite(x, y);

		prevData[id] = data;
		prevColor[id] = color;
	}

	void drawStatusIndicatorCell(
		int32_t x,
		int32_t y,
		int32_t width,
		int32_t height,
		const String &label,
		const String &id,
		bool active,
		uint16_t activeColor,
		bool forceUpdate = false)
	{
		const String state = active ? "1" : "0";
		if (!forceUpdate && prevData[id] == state)
		{
			return;
		}

		tft.fillRect(x, y, width, height, TFT_BLACK);
		tft.drawRoundRect(x, y, width, height, 5, TFT_DARKGREY);

		tft.setTextPadding(0);
		tft.setTextDatum(TC_DATUM);
		tft.setTextColor(TFT_WHITE, TFT_BLACK);
		tft.drawString(label, x + width / 2, y + 3, 2);

		const int32_t radius = 7;
		const int32_t centerX = x + width / 2;
		const int32_t centerY = y + 31;
		const uint16_t lampColor = active ? activeColor : TFT_DARKGREY;

		tft.fillCircle(centerX, centerY, radius, lampColor);
		tft.drawCircle(centerX, centerY, radius + 1, active ? TFT_WHITE : TFT_DARKGREY);

		// Restore the default text state so later cells are not shifted.
		tft.setTextDatum(TL_DATUM);
		tft.setTextPadding(0);

		prevData[id] = state;
	}

	void drawPedalProgressCells(
		int32_t x,
		int32_t y,
		int32_t totalWidth,
		int32_t totalHeight,
		int throttlePercent,
		int throttleAppliedPercent,
		int brakePercent,
		int brakeAppliedPercent,
		bool forceUpdate = false)
	{
		throttlePercent = constrain(throttlePercent, 0, 100);
		throttleAppliedPercent = constrain(throttleAppliedPercent, 0, 100);
		brakePercent = constrain(brakePercent, 0, 100);
		brakeAppliedPercent = constrain(brakeAppliedPercent, 0, 100);

		// 上下兩格，而不是左右兩格。
		const int32_t gap = 2;
		const int32_t cellHeight = (totalHeight - gap) / 2;
		const int32_t throttleY = y;
		const int32_t brakeY = y + cellHeight + gap;

		auto updateOneCell = [&](
								 int32_t cellY,
								 int percent,
								 int appliedPercent,
								 const char *id,
								 uint16_t color)
		{
			const String state = String(percent) + ":" + String(appliedPercent);
			const bool firstDraw = true;
			const int oldPercent = 0;
			const uint16_t inputColor = color == TFT_YELLOW
				? tft.color565(255, 245, 150)
				: tft.color565(110, 170, 255);

			if (!forceUpdate && prevData[id] == state)
			{
				return;
			}

			const int32_t spriteInset = 2;
			const int32_t spriteFillWidth = totalWidth - spriteInset * 2;
			const int32_t spriteFillHeight = cellHeight - spriteInset * 2;
			const int32_t spriteInputFill = spriteFillWidth * percent / 100;
			const int32_t spriteAppliedFill = spriteFillWidth * appliedPercent / 100;
			static LGFX_Sprite classicPedalSprite(&tft);
			static int32_t spriteWidth = 0;
			static int32_t spriteHeight = 0;
			if (spriteWidth != totalWidth || spriteHeight != cellHeight)
			{
				classicPedalSprite.deleteSprite();
				classicPedalSprite.setColorDepth(16);
				if (classicPedalSprite.createSprite(totalWidth, cellHeight) != nullptr)
				{
					spriteWidth = totalWidth;
					spriteHeight = cellHeight;
				}
			}

			if (spriteWidth == totalWidth && spriteHeight == cellHeight)
			{
				classicPedalSprite.fillSprite(TFT_BLACK);
				if (spriteInputFill > 0)
					classicPedalSprite.fillRect(spriteInset, spriteInset,
						spriteInputFill, spriteFillHeight, inputColor);
				if (spriteAppliedFill > 0)
					classicPedalSprite.fillRect(spriteInset, spriteInset,
						spriteAppliedFill, spriteFillHeight, color);
				classicPedalSprite.drawRoundRect(0, 0, totalWidth, cellHeight, 5,
					TFT_DARKGREY);
				classicPedalSprite.pushSprite(x, cellY);
				prevData[id] = state;
				return;
			}

			// Progress 幾乎填滿整個 cell，只留下 2 px 邊框。
			const int32_t inset = 2;
			const int32_t fillX = x + inset;
			const int32_t fillY = cellY + inset;
			const int32_t fillWidthMax = totalWidth - inset * 2;
			const int32_t fillHeight = cellHeight - inset * 2;
			const int32_t oldFill = fillWidthMax * oldPercent / 100;
			const int32_t newFill = fillWidthMax * percent / 100;

			// 第一次只建立格子；之後僅更新變動區段，避免閃爍。
			if (firstDraw || forceUpdate)
			{
				tft.fillRect(x, cellY, totalWidth, cellHeight, TFT_BLACK);
				tft.drawRoundRect(x, cellY, totalWidth, cellHeight, 5, TFT_DARKGREY);

				if (newFill > 0)
				{
					tft.fillRect(fillX, fillY, newFill, fillHeight,
						inputColor);
				}
			}
			else if (newFill > oldFill)
			{
				// 增加時只補上新增的區段。
				tft.fillRect(
					fillX + oldFill,
					fillY,
					newFill - oldFill,
					fillHeight,
					inputColor);
			}
			else if (newFill < oldFill)
			{
				// 減少時只擦掉多出的區段。
				tft.fillRect(
					fillX + newFill,
					fillY,
					oldFill - newFill,
					fillHeight,
					TFT_BLACK);
			}

			// 保險重畫邊框，避免填色蓋到外框。
			const int32_t appliedFill = fillWidthMax * appliedPercent / 100;
			if (appliedFill > 0)
			{
				tft.fillRect(fillX, fillY, appliedFill, fillHeight, color);
			}
			tft.drawRoundRect(x, cellY, totalWidth, cellHeight, 5, TFT_DARKGREY);
			prevData[id] = state;
		};

		updateOneCell(throttleY, throttlePercent, throttleAppliedPercent,
			"pedalThrottle", TFT_YELLOW);
		updateOneCell(brakeY, brakePercent, brakeAppliedPercent,
			"pedalBrake", TFT_BLUE);
	}

	void drawCell(
		int32_t x,
		int32_t y,
		const String &data,
		const String &id,
		const String &name,
		const String &align,
		uint16_t color,
		uint8_t fontSize,
		bool forceUpdate = false)
	{
		// Each normal cell sets its own text origin, independent of prior widgets.
		tft.setTextDatum(TL_DATUM);
		tft.setTextPadding(0);

		const bool dataChanged =
			forceUpdate || prevData[id] != data;

		const bool colorChanged =
			forceUpdate || prevColor[id] != color;

		if (!dataChanged && !colorChanged)
		{
			return;
		}

		const int32_t titleHeight = 18;
		const int32_t hPadding = 5;

		/*
		 * 依照原本 Cell 的配置決定可使用的文字寬度。
		 *
		 * left / right 通常是跨兩格的大欄位；
		 * center 通常是單格欄位。
		 */
		int32_t textAreaWidth;

		if (align == "left" || align == "right")
		{
			textAreaWidth = CELL_WIDTH * 2 - hPadding * 2;
		}
		else
		{
			textAreaWidth = CELL_WIDTH - hPadding * 2;
		}

		/*
		 * 邊框與標題只有在強制更新或顏色變動時重畫。
		 * 數值每次改變時，不需要重畫整個框。
		 */
		if (forceUpdate || colorChanged)
		{
			if (align == "right")
			{
				tft.drawRoundRect(
					x - CELL_WIDTH * 2 + 1,
					y,
					CELL_WIDTH * 2 - 2,
					CELL_HEIGHT - 1,
					5,
					color);
			}
			else if (align == "left")
			{
				tft.drawRoundRect(
					x,
					y,
					CELL_WIDTH * 2 - 2,
					CELL_HEIGHT - 1,
					5,
					color);
			}
			else
			{
				tft.drawRoundRect(
					x,
					y,
					CELL_WIDTH - 1,
					CELL_HEIGHT - 1,
					5,
					color);
			}

			tft.setTextColor(color, TFT_BLACK);
			tft.setTextPadding(0);

			if (align == "right")
			{
				tft.drawRightString(
					name,
					x - hPadding,
					y + 2,
					2);
			}
			else
			{
				tft.drawString(
					name,
					x + hPadding,
					y + 2,
					2);
			}
		}

		/*
		 * 不再先把舊文字塗黑。
		 *
		 * setTextPadding() 會在畫新文字時，使用背景色補滿指定寬度，
		 * 所以像 100 變成 99 時，不會留下最後面的 0。
		 */
		tft.setTextColor(color, TFT_BLACK);
		tft.setTextPadding(textAreaWidth);

		if (align == "left")
		{
			tft.drawString(
				data,
				x + hPadding,
				y + titleHeight,
				fontSize);
		}
		else if (align == "right")
		{
			tft.drawRightString(
				data,
				x - hPadding,
				y + titleHeight,
				fontSize);
		}
		else
		{
			tft.drawCentreString(
				data,
				x + HALF_CELL_WIDTH,
				y + titleHeight,
				fontSize);
		}

		/*
		 * 一定要恢復成 0，否則後面其他 drawString()
		 * 可能也會被自動補黑背景。
		 */
		tft.setTextPadding(0);

		prevData[id] = data;
		prevColor[id] = color;
	}

	void fadeToBrightness(uint8_t targetBrightness)
	{
		if (currentBrightness == targetBrightness)
			return;

		int brightness = currentBrightness;

		if (brightness < targetBrightness)
		{
			while (brightness < targetBrightness)
			{
				brightness += FADE_STEP;

				if (brightness > targetBrightness)
					brightness = targetBrightness;

				tft.setBrightness(brightness);
				delay(FADE_DELAY_MS);
			}
		}
		else
		{
			while (brightness > targetBrightness)
			{
				brightness -= FADE_STEP;

				if (brightness < targetBrightness)
					brightness = targetBrightness;

				tft.setBrightness(brightness);
				delay(FADE_DELAY_MS);
			}
		}

		currentBrightness = targetBrightness;
	}

	void fadeScreenOn()
	{
		fadeToBrightness(normalBrightness());
	}

	void fadeScreenOff()
	{
		fadeToBrightness(0);
	}

	void redrawAfterWifiResetDialog()
	{
		tft.fillScreen(TFT_BLACK);
		prevData.clear();
		prevColor.clear();
		prev_gear = "";
		prev_rpmPercent = 50;
		connectingScreenActive = false;
		connectingAnimationStep = 0;
		forceUpdate = true;
	}

	bool touchInside(int x, int y, int width, int height) const
	{
		return touchX >= x && touchX < x + width &&
			touchY >= y && touchY < y + height;
	}

	void drawSettingsButton(
		int x,
		int y,
		int width,
		int height,
		const String &label,
		bool pressed,
		bool active = false,
		bool destructive = false)
	{
		const uint16_t normalFill = destructive
			? tft.color565(92, 24, 28)
			: active
				? tft.color565(18, 74, 104)
				: tft.color565(42, 46, 52);
		const uint16_t fill = pressed ? tft.color565(38, 126, 150) : normalFill;
		const uint16_t border = active ? TFT_WHITE : TFT_LIGHTGREY;
		tft.fillRoundRect(x, y, width, height, 7, fill);
		tft.drawRoundRect(x, y, width, height, 7, border);
		if (active)
			tft.drawRoundRect(x + 1, y + 1, width - 2, height - 2, 6, border);
		tft.setTextColor(TFT_WHITE, fill);
		tft.setTextDatum(MC_DATUM);
		tft.drawString(label, x + width / 2, y + height / 2, 2);
	}

	void drawDeviceBrightnessValue()
	{
		static LGFX_Sprite brightnessSprite(&tft);
		static bool spriteCreated = false;
		if (!spriteCreated)
		{
			brightnessSprite.setColorDepth(16);
			spriteCreated = brightnessSprite.createSprite(116, 48) != nullptr;
		}
		if (spriteCreated)
		{
			brightnessSprite.fillSprite(TFT_BLACK);
			brightnessSprite.setTextColor(TFT_WHITE, TFT_BLACK);
			brightnessSprite.setTextDatum(MC_DATUM);
			brightnessSprite.drawString(
				String(userBrightnessPercent) + "%", 58, 24, 4);
			brightnessSprite.pushSprite(102, 72);
		}
		else
		{
			tft.fillRect(102, 72, 116, 48, TFT_BLACK);
			tft.setTextColor(TFT_WHITE, TFT_BLACK);
			tft.setTextDatum(MC_DATUM);
			tft.drawString(String(userBrightnessPercent) + "%", X_CENTER, 96, 4);
		}
	}

	void drawSettingsScreen(int pressedButton = -1)
	{
		tft.fillScreen(TFT_BLACK);
		tft.setTextPadding(0);
		tft.setTextDatum(MC_DATUM);

		if (settingsScreen == SettingsScreen::Main)
		{
			tft.setTextColor(TFT_WHITE, TFT_BLACK);
			tft.drawString("SETTINGS", X_CENTER, 25, 4);
			drawSettingsButton(30, 55, 260, 48, "SELECT THEME", pressedButton == 0);
			drawSettingsButton(30, 113, 260, 48, "DEVICE SETTINGS", pressedButton == 1);
			drawSettingsButton(30, 171, 260, 48, "BACK", pressedButton == 2);
		}
		else if (settingsScreen == SettingsScreen::ThemeSelection)
		{
			tft.setTextColor(TFT_WHITE, TFT_BLACK);
			tft.drawString("SELECT THEME", X_CENTER, 22, 4);
			const DashboardTheme themes[] = {
				DashboardTheme::Classic,
				DashboardTheme::GT3,
				DashboardTheme::Retro,
				DashboardTheme::Radar};
			for (int i = 0; i < 4; ++i)
			{
				const bool active = activeDashboardTheme == themes[i];
				const int buttonX = i % 2 == 0 ? 20 : 165;
				const int buttonY = i < 2 ? 50 : 112;
				drawSettingsButton(buttonX, buttonY, 135, 50,
					dashboardThemeName(themes[i]),
					pressedButton == i, active);
			}
			drawSettingsButton(30, 180, 260, 38, "BACK", pressedButton == 4);
		}
		else if (settingsScreen == SettingsScreen::DeviceSettings)
		{
			tft.setTextColor(TFT_WHITE, TFT_BLACK);
			tft.drawString("DEVICE SETTINGS", X_CENTER, 20, 4);
			tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
			tft.drawString("BRIGHTNESS", X_CENTER, 53, 2);
			drawSettingsButton(30, 72, 70, 48, "-", pressedButton == 0);
			drawSettingsButton(220, 72, 70, 48, "+", pressedButton == 1);
			drawDeviceBrightnessValue();
			drawSettingsButton(30, 132, 260, 38, "RESET WIFI", pressedButton == 2,
				false, true);
			drawSettingsButton(30, 182, 260, 38, "BACK", pressedButton == 3);
		}
		else if (settingsScreen == SettingsScreen::WifiResetConfirmation)
		{
			tft.setTextColor(TFT_WHITE, TFT_BLACK);
			tft.drawString("Reset saved Wi-Fi?", X_CENTER, 60, 4);
			tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
			tft.drawString("Device will restart", X_CENTER, 95, 2);
			drawSettingsButton(25, 135, 120, 62, "CANCEL", pressedButton == 0);
			drawSettingsButton(175, 135, 120, 62, "RESET", pressedButton == 1,
				false, true);
		}

		tft.setTextDatum(TL_DATUM);
	}

	void redrawSettingsButton(int button, bool pressed)
	{
		if (settingsScreen == SettingsScreen::Main)
		{
			if (button == 0)
				drawSettingsButton(30, 55, 260, 48, "SELECT THEME", pressed);
			else if (button == 1)
				drawSettingsButton(30, 113, 260, 48, "DEVICE SETTINGS", pressed);
			else if (button == 2)
				drawSettingsButton(30, 171, 260, 48, "BACK", pressed);
		}
		else if (settingsScreen == SettingsScreen::ThemeSelection)
		{
			if (button >= 0 && button <= 3)
			{
				const DashboardTheme themes[] = {
					DashboardTheme::Classic,
					DashboardTheme::GT3,
					DashboardTheme::Retro,
					DashboardTheme::Radar};
				const bool active = activeDashboardTheme == themes[button];
				const int buttonX = button % 2 == 0 ? 20 : 165;
				const int buttonY = button < 2 ? 50 : 112;
				drawSettingsButton(buttonX, buttonY, 135, 50,
					dashboardThemeName(themes[button]),
					pressed, active);
			}
			else if (button == 4)
			{
				drawSettingsButton(30, 180, 260, 38, "BACK", pressed);
			}
		}
		else if (settingsScreen == SettingsScreen::DeviceSettings)
		{
			if (button == 0)
				drawSettingsButton(30, 72, 70, 48, "-", pressed);
			else if (button == 1)
				drawSettingsButton(220, 72, 70, 48, "+", pressed);
			else if (button == 2)
				drawSettingsButton(30, 132, 260, 38, "RESET WIFI", pressed,
					false, true);
			else if (button == 3)
				drawSettingsButton(30, 182, 260, 38, "BACK", pressed);
		}
		else if (settingsScreen == SettingsScreen::WifiResetConfirmation)
		{
			if (button == 0)
				drawSettingsButton(25, 135, 120, 62, "CANCEL", pressed);
			else if (button == 1)
				drawSettingsButton(175, 135, 120, 62, "RESET", pressed,
					false, true);
		}
		tft.setTextDatum(TL_DATUM);
	}

	void showSettingsScreen(SettingsScreen screen)
	{
		screenSleeping = false;
		screenOffByUser = false;
		tft.setBrightness(normalBrightness());
		currentBrightness = normalBrightness();
		settingsScreen = screen;
		wifiResetConfirmOpen = screen == SettingsScreen::WifiResetConfirmation;
		settingsPressedButton = -1;
		settingsLastInteractionTime = millis();
		drawSettingsScreen();
	}

	void closeSettings()
	{
		settingsScreen = SettingsScreen::Closed;
		wifiResetConfirmOpen = false;
		settingsPressedButton = -1;
		redrawAfterWifiResetDialog();
	}

	void showWifiResetConfirm()
	{
		showSettingsScreen(SettingsScreen::WifiResetConfirmation);
	}

#if 0
	void showWifiResetConfirmLegacy()
	{
		// 即使原本處於暗屏，也要先亮起確認畫面。
		screenSleeping = false;
		screenOffByUser = false;
		tft.setBrightness(normalBrightness());
		currentBrightness = normalBrightness();

		tft.fillScreen(TFT_BLACK);
		tft.setTextPadding(0);
		tft.setTextDatum(MC_DATUM);

		tft.setTextColor(TFT_WHITE, TFT_BLACK);
		tft.drawString("Reset saved Wi-Fi?", SCREEN_WIDTH / 2, 60, 4);

		tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
		tft.drawString("Device will restart", SCREEN_WIDTH / 2, 95, 2);

		const int buttonY = 135;
		const int buttonHeight = 62;
		const int buttonWidth = 120;
		const int noX = 25;
		const int yesX = SCREEN_WIDTH - 25 - buttonWidth;

		tft.fillRoundRect(noX, buttonY, buttonWidth, buttonHeight, 8, TFT_DARKGREY);
		tft.drawRoundRect(noX, buttonY, buttonWidth, buttonHeight, 8, TFT_WHITE);
		tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
		tft.drawString("NO", noX + buttonWidth / 2, buttonY + buttonHeight / 2, 4);

		tft.fillRoundRect(yesX, buttonY, buttonWidth, buttonHeight, 8, TFT_RED);
		tft.drawRoundRect(yesX, buttonY, buttonWidth, buttonHeight, 8, TFT_WHITE);
		tft.setTextColor(TFT_WHITE, TFT_RED);
		tft.drawString("YES", yesX + buttonWidth / 2, buttonY + buttonHeight / 2, 4);

		tft.setTextDatum(TL_DATUM);
	}

#endif

	void showWifiResettingScreen()
	{
		tft.fillScreen(TFT_BLACK);
		tft.setTextPadding(0);
		tft.setTextDatum(MC_DATUM);

		tft.setTextColor(TFT_CYAN, TFT_BLACK);
		tft.drawString("WI-FI RESET", SCREEN_WIDTH / 2, 90, 4);

		tft.setTextColor(TFT_WHITE, TFT_BLACK);
		tft.drawString("Restarting...", SCREEN_WIDTH / 2, 145, 2);

		tft.setTextDatum(TL_DATUM);
	}

	bool takeWifiResetRequest()
	{
		if (!wifiResetRequested)
		{
			return false;
		}

		wifiResetRequested = false;
		return true;
	}

	int settingsButtonAtTouch() const
	{
		if (settingsScreen == SettingsScreen::Main)
		{
			if (touchInside(30, 55, 260, 48)) return 0;
			if (touchInside(30, 113, 260, 48)) return 1;
			if (touchInside(30, 171, 260, 48)) return 2;
		}
		else if (settingsScreen == SettingsScreen::ThemeSelection)
		{
			for (int i = 0; i < 4; ++i)
			{
				const int buttonX = i % 2 == 0 ? 20 : 165;
				const int buttonY = i < 2 ? 50 : 112;
				if (touchInside(buttonX, buttonY, 135, 50)) return i;
			}
			if (touchInside(30, 180, 260, 38)) return 4;
		}
		else if (settingsScreen == SettingsScreen::DeviceSettings)
		{
			if (touchInside(30, 72, 70, 48)) return 0;
			if (touchInside(220, 72, 70, 48)) return 1;
			if (touchInside(30, 132, 260, 38)) return 2;
			if (touchInside(30, 182, 260, 38)) return 3;
		}
		else if (settingsScreen == SettingsScreen::WifiResetConfirmation)
		{
			if (touchInside(25, 135, 120, 62)) return 0;
			if (touchInside(175, 135, 120, 62)) return 1;
		}
		return -1;
	}

	void activateSettingsButton(int button)
	{
		if (settingsScreen == SettingsScreen::Main)
		{
			if (button == 0)
				showSettingsScreen(SettingsScreen::ThemeSelection);
			else if (button == 1)
				showSettingsScreen(SettingsScreen::DeviceSettings);
			else if (button == 2)
				closeSettings();
		}
		else if (settingsScreen == SettingsScreen::ThemeSelection)
		{
			if (button >= 0 && button <= 3)
			{
				const DashboardTheme themes[] = {
					DashboardTheme::Classic,
					DashboardTheme::GT3,
					DashboardTheme::Retro,
					DashboardTheme::Radar};
				const DashboardTheme selectedTheme = themes[button];
				const bool changed = selectedTheme != activeDashboardTheme;
				settingsScreen = SettingsScreen::Closed;
				wifiResetConfirmOpen = false;
				settingsPressedButton = -1;
				selectDashboardTheme(selectedTheme, true);
				if (!changed) redrawAfterWifiResetDialog();
			}
			else if (button == 4)
			{
				showSettingsScreen(SettingsScreen::Main);
			}
		}
		else if (settingsScreen == SettingsScreen::DeviceSettings)
		{
			if (button == 0 || button == 1)
			{
				const int adjustment = button == 0
					? -BRIGHTNESS_STEP_PERCENT
					: BRIGHTNESS_STEP_PERCENT;
				userBrightnessPercent = constrain(
					static_cast<int>(userBrightnessPercent) + adjustment,
					static_cast<int>(MIN_BRIGHTNESS_PERCENT), 100);
				tft.setBrightness(normalBrightness());
				currentBrightness = normalBrightness();
				scheduleBrightnessSave();
				settingsLastInteractionTime = millis();
				drawDeviceBrightnessValue();
			}
			else if (button == 2)
			{
				showWifiResetConfirm();
			}
			else if (button == 3)
			{
				showSettingsScreen(SettingsScreen::Main);
			}
		}
		else if (settingsScreen == SettingsScreen::WifiResetConfirmation)
		{
			if (button == 0)
			{
				showSettingsScreen(SettingsScreen::DeviceSettings);
			}
			else if (button == 1)
			{
				showWifiResettingScreen();
				wifiResetRequested = true;
			}
		}
	}

	void readTouch()
	{
		saveBrightnessIfDue();
		if (!TOUCH_SCREEN_CONTROL_ENABLED)
		{
			return;
		}

		static bool wasTouched = false;
		static bool waitForReleaseAfterScreenChange = false;
		const bool isTouched = tft.getTouch(&touchX, &touchY);

		if (waitForReleaseAfterScreenChange)
		{
			if (!isTouched)
			{
				waitForReleaseAfterScreenChange = false;
				wasTouched = false;
			}
			return;
		}

		// Waking always takes precedence over Settings touch targets. The first
		// tap only restores the display; it never activates a hidden button.
		if (screenSleeping)
		{
			if (!isTouched && wasTouched)
			{
				screenSleeping = false;
				screenOffByUser = false;
				fadeScreenOn();
				forceUpdate = true;
				if (!previousGameRunning)
				{
					gameStoppedTimerStarted = true;
					gameStoppedTime = millis();
				}
				else
				{
					gameStoppedTimerStarted = false;
				}

				if (settingsScreen != SettingsScreen::Closed)
					drawSettingsScreen();
			}
			wasTouched = isTouched;
			return;
		}

		if (isTouched && !wasTouched && !previousGameRunning)
		{
			gameStoppedTimerStarted = true;
			gameStoppedTime = millis();
		}

		if (settingsScreen != SettingsScreen::Closed)
		{
			if (!isTouched && settingsLastInteractionTime != 0 &&
				millis() - settingsLastInteractionTime >= SETTINGS_TIMEOUT_MS)
			{
				closeSettings();
				wasTouched = false;
				return;
			}

			if (isTouched && !wasTouched)
			{
				settingsLastInteractionTime = millis();
				settingsPressedButton = settingsButtonAtTouch();
			}
			else if (!isTouched && wasTouched)
			{
				settingsLastInteractionTime = millis();
				const int releasedButton = settingsButtonAtTouch();
				const int pressedButton = settingsPressedButton;
				settingsPressedButton = -1;
				if (pressedButton >= 0 && releasedButton == pressedButton)
				{
					activateSettingsButton(pressedButton);
					waitForReleaseAfterScreenChange = true;
				}
			}

			wasTouched = isTouched;
			return;
		}

		if (!isTouched && wasTouched)
		{
			showSettingsScreen(SettingsScreen::Main);
			waitForReleaseAfterScreenChange = true;
		}

		wasTouched = isTouched;
	}

#if 0
	void readTouchLegacy()
	{
		if (!TOUCH_SCREEN_CONTROL_ENABLED)
		{
			return;
		}

		static bool wasTouched = false;
		static bool longPressTriggered = false;
		static bool waitForReleaseAfterDialog = false;
		static unsigned long touchStartTime = 0;

		const bool isTouched = tft.getTouch(&touchX, &touchY);

		// 長按開啟確認畫面後，必須先放開，避免同一次觸控誤按 YES／NO。
		if (waitForReleaseAfterDialog)
		{
			if (!isTouched)
			{
				waitForReleaseAfterDialog = false;
				wasTouched = false;
			}
			return;
		}

		// 確認畫面中的按鈕處理。
		if (wifiResetConfirmOpen)
		{
			if (isTouched && !wasTouched)
			{
				touchStartTime = millis();
			}

			if (!isTouched && wasTouched)
			{
				const int buttonY = 135;
				const int buttonHeight = 62;
				const int buttonWidth = 120;
				const int noX = 25;
				const int yesX = SCREEN_WIDTH - 25 - buttonWidth;

				const bool noPressed =
					touchX >= noX && touchX < noX + buttonWidth &&
					touchY >= buttonY && touchY < buttonY + buttonHeight;

				const bool yesPressed =
					touchX >= yesX && touchX < yesX + buttonWidth &&
					touchY >= buttonY && touchY < buttonY + buttonHeight;

				if (yesPressed)
				{
					showWifiResettingScreen();
					wifiResetRequested = true;
				}
				else if (noPressed)
				{
					wifiResetConfirmOpen = false;
					redrawAfterWifiResetDialog();
				}
			}

			wasTouched = isTouched;
			return;
		}

		// 手指剛碰到螢幕：開始計算長按時間。
		if (isTouched && !wasTouched)
		{
			touchStartTime = millis();
			longPressTriggered = false;
		}

		// 全螢幕持續按住 6 秒：顯示 Wi-Fi 重設確認畫面。
		if (isTouched &&
			!longPressTriggered &&
			millis() - touchStartTime >= WIFI_RESET_HOLD_MS)
		{
			longPressTriggered = true;
			wifiResetConfirmOpen = true;
			showWifiResetConfirm();
			waitForReleaseAfterDialog = true;
			wasTouched = isTouched;
			return;
		}

		// 未達 6 秒便放開：維持原本的短按亮屏／熄屏功能。
		if (!isTouched && wasTouched && !longPressTriggered)
		{
			if (screenSleeping)
			{
				screenSleeping = false;
				screenOffByUser = false;
				fadeScreenOn();
				forceUpdate = true;

				if (!previousGameRunning)
				{
					gameStoppedTimerStarted = true;
					gameStoppedTime = millis();
				}
				else
				{
					gameStoppedTimerStarted = false;
				}
			}
			else
			{
				screenOffByUser = true;
				screenSleeping = true;
				fadeScreenOff();
				gameStoppedTimerStarted = false;
			}
		}

		wasTouched = isTouched;
	}
#endif
};

#endif
