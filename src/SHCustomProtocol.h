
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
#include <qrcode.h>
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
	Mono = 4,
	Pocket = 5,
	Endurance = 6,
};

struct DashboardThemeDescriptor
{
	DashboardTheme id;
	const char *name;
};

static constexpr DashboardThemeDescriptor DASHBOARD_THEMES[] = {
	{DashboardTheme::Classic, "CLASSIC"},
	{DashboardTheme::GT3, "GT3"},
	{DashboardTheme::Retro, "RETRO"},
	{DashboardTheme::Radar, "RADAR"},
	{DashboardTheme::Mono, "MONO"},
	{DashboardTheme::Pocket, "POCKET"},
	{DashboardTheme::Endurance, "ENDURANCE"},
};
static constexpr size_t DASHBOARD_THEME_COUNT =
	sizeof(DASHBOARD_THEMES) / sizeof(DASHBOARD_THEMES[0]);

// Phase 1 renderer preview hook. Normal firmware loads the persisted theme;
// development builds can select a valid enum value without modifying storage.
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
	bool fuelIsEV = false;
	bool fuelValueValid = false;
	String fuelLabel = "FUEL";
	String fuelDisplayValue = "--";
	int fuelProgressPercent = 100;
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
	tft.setTextPadding(0);
	tft.setTextDatum(MC_DATUM);

	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.drawString("WI-FI SETUP", X_CENTER, 18, 4);

	static constexpr uint8_t QR_VERSION = 3;
	static constexpr int QR_SCALE = 4;
	static constexpr int QR_QUIET_MODULES = 4;
	static constexpr int QR_X = 10;
	static constexpr int QR_Y = 47;
	uint8_t qrData[128] = {};
	QRCode qr;
	if (qrcode_initText(&qr, qrData, QR_VERSION, ECC_LOW,
		"WIFI:T:nopass;S:GT7-DASH-SETUP;;") == 0)
	{
		const int outerSize = (qr.size + QR_QUIET_MODULES * 2) * QR_SCALE;
		tft.fillRect(QR_X, QR_Y, outerSize, outerSize, TFT_WHITE);
		for (uint8_t y = 0; y < qr.size; ++y)
		{
			for (uint8_t x = 0; x < qr.size; ++x)
			{
				if (!qrcode_getModule(&qr, x, y)) continue;
				tft.fillRect(
					QR_X + (x + QR_QUIET_MODULES) * QR_SCALE,
					QR_Y + (y + QR_QUIET_MODULES) * QR_SCALE,
					QR_SCALE, QR_SCALE, TFT_BLACK);
			}
		}
	}

	tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
	tft.drawString("Scan to join", 235, 53, 2);
	tft.drawString("or connect manually", 235, 75, 1);
	tft.drawString("Wi-Fi", 235, 94, 1);
	tft.setTextColor(TFT_CYAN, TFT_BLACK);
	tft.drawString("GT7-DASH-SETUP", 235, 116, 2);
	tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
	tft.drawString("Then open", 235, 143, 1);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.drawString("192.168.4.1", 235, 164, 2);

	tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
	tft.drawCentreString(
		String("v") + GT7_DASH_VERSION,
		X_CENTER,
		SCREEN_HEIGHT - 14,
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
	DashboardTheme previewDashboardTheme = DashboardTheme::GT3;
	DashboardState themePreviewData;
	bool previewFullscreen = false;
	bool dashboardPreferencesReady = false;

	uint16_t touchX, touchY; // definisce i due interi per gestione touchscreen
	int numButtons = 6;		 // Variabile per il numero di pulsanti

	// A lightweight correction applied after the panel driver's own touch
	// mapping. Persisting only four rotations keeps recovery deterministic.
	enum class TouchRotation : uint8_t
	{
		Deg0 = 0,
		Deg90 = 1,
		Deg180 = 2,
		Deg270 = 3,
	};
	TouchRotation touchRotation = TouchRotation::Deg0;
	TouchRotation pendingTouchRotation = TouchRotation::Deg0;
	bool touchCalibrationVerified = false;
	bool touchCalibrationEntryArmed = false;
	bool touchCalibrationAwaitingSecondTap = false;
	unsigned long touchCalibrationFirstTapTime = 0;
	uint16_t originalTouchX = 0;
	uint16_t originalTouchY = 0;

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
		TouchCalibration,
	};

	static constexpr unsigned long SETTINGS_TIMEOUT_MS = 15000UL;
	static constexpr unsigned long TOUCH_CALIBRATION_DOUBLE_TAP_MS = 3000UL;
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
		for (size_t i = 0; i < DASHBOARD_THEME_COUNT; ++i)
			if (static_cast<uint8_t>(DASHBOARD_THEMES[i].id) == value) return true;
		return false;
	}

	static const char *dashboardThemeName(DashboardTheme theme)
	{
		for (size_t i = 0; i < DASHBOARD_THEME_COUNT; ++i)
			if (DASHBOARD_THEMES[i].id == theme) return DASHBOARD_THEMES[i].name;
		return "GT3";
	}

	static size_t dashboardThemeIndex(DashboardTheme theme)
	{
		for (size_t i = 0; i < DASHBOARD_THEME_COUNT; ++i)
			if (DASHBOARD_THEMES[i].id == theme) return i;
		return 0;
	}

	static void applyTouchRotation(
		uint16_t sourceX,
		uint16_t sourceY,
		TouchRotation rotation,
		uint16_t &resultX,
		uint16_t &resultY)
	{
		const uint32_t x = constrain(sourceX, 0, SCREEN_WIDTH - 1);
		const uint32_t y = constrain(sourceY, 0, SCREEN_HEIGHT - 1);
		switch (rotation)
		{
		case TouchRotation::Deg90:
			resultX = static_cast<uint16_t>(
				(y * (SCREEN_WIDTH - 1) + (SCREEN_HEIGHT - 1) / 2) /
				(SCREEN_HEIGHT - 1));
			resultY = static_cast<uint16_t>(
				(((SCREEN_WIDTH - 1) - x) * (SCREEN_HEIGHT - 1) +
					(SCREEN_WIDTH - 1) / 2) /
				(SCREEN_WIDTH - 1));
			break;
		case TouchRotation::Deg180:
			resultX = SCREEN_WIDTH - 1 - x;
			resultY = SCREEN_HEIGHT - 1 - y;
			break;
		case TouchRotation::Deg270:
			resultX = static_cast<uint16_t>(
				(((SCREEN_HEIGHT - 1) - y) * (SCREEN_WIDTH - 1) +
					(SCREEN_HEIGHT - 1) / 2) /
				(SCREEN_HEIGHT - 1));
			resultY = static_cast<uint16_t>(
				(x * (SCREEN_HEIGHT - 1) + (SCREEN_WIDTH - 1) / 2) /
				(SCREEN_WIDTH - 1));
			break;
		case TouchRotation::Deg0:
		default:
			resultX = static_cast<uint16_t>(x);
			resultY = static_cast<uint16_t>(y);
			break;
		}
	}

	int calibrationRotationForTouch(uint16_t rawX, uint16_t rawY) const
	{
		constexpr int targetX = SCREEN_WIDTH / 2;
		constexpr int targetY = 18;
		constexpr int hitHalfWidth = 48;
		constexpr int hitHeight = 26;
		int closestRotation = -1;
		uint32_t closestDistance = UINT32_MAX;
		for (uint8_t value = 0; value < 4; ++value)
		{
			uint16_t candidateX = 0;
			uint16_t candidateY = 0;
			applyTouchRotation(rawX, rawY,
				static_cast<TouchRotation>(value), candidateX, candidateY);
			if (abs(static_cast<int>(candidateX) - targetX) > hitHalfWidth ||
				candidateY > hitHeight)
				continue;
			const int dx = static_cast<int>(candidateX) - targetX;
			const int dy = static_cast<int>(candidateY) - targetY;
			const uint32_t distance = dx * dx + dy * dy;
			if (distance < closestDistance)
			{
				closestDistance = distance;
				closestRotation = value;
			}
		}
		return closestRotation;
	}

	void prepareThemePreviewData()
	{
		themePreviewData.rpmPercent = 68;
		themePreviewData.prev_rpmPercent = 68;
		themePreviewData.rpmRedLineSetting = 78;
		themePreviewData.engineRpm = 5600;
		themePreviewData.rpmAlertRangeValid = true;
		themePreviewData.revLimitAlertActive = false;
		themePreviewData.gear = "4";
		themePreviewData.speed = "140";
		themePreviewData.currentLapTime = "01:24.631";
		themePreviewData.lastLapTime = "01:25.104";
		themePreviewData.bestLapTime = "01:24.382";
		themePreviewData.sessionBestLiveDeltaSeconds = "-0.237";
		themePreviewData.sessionBestLiveDeltaProgressSeconds = "-0.24";
		// These legacy protocol fields currently carry REM, POS, LAP and FUEL.
		themePreviewData.tyrePressureFrontLeft = "4";
		themePreviewData.tyrePressureFrontRight = "5";
		themePreviewData.tyrePressureRearLeft = "3/10";
		themePreviewData.brakeBias = "80";
		themePreviewData.fuelAlertActive = "60";
		themePreviewData.fuelIsEV = false;
		themePreviewData.fuelValueValid = true;
		themePreviewData.fuelLabel = "FUEL";
		themePreviewData.fuelDisplayValue = "80";
		themePreviewData.fuelProgressPercent = 80;
		themePreviewData.tcLevel = "68";
		themePreviewData.tcFilteredLevel = "62";
		themePreviewData.absLevel = "36";
		themePreviewData.absFilteredLevel = "31";
		themePreviewData.absActive = "1";
		themePreviewData.tcActive = "0";
		themePreviewData.lapInvalidated = "False";
		themePreviewData.tyreTemperatures[0] = 82.0f;
		themePreviewData.tyreTemperatures[1] = 79.0f;
		themePreviewData.tyreTemperatures[2] = 76.0f;
		themePreviewData.tyreTemperatures[3] = 78.0f;
		themePreviewData.gameRunning = "True";
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
			const uint8_t storedTouchRotation = dashboardPreferences.getUChar(
				"touchRot", static_cast<uint8_t>(TouchRotation::Deg0));
			touchRotation = storedTouchRotation <= static_cast<uint8_t>(TouchRotation::Deg270)
				? static_cast<TouchRotation>(storedTouchRotation)
				: TouchRotation::Deg0;
		}
		pendingTouchRotation = touchRotation;

		activeDashboardTheme = isValidDashboardTheme(storedTheme)
			? static_cast<DashboardTheme>(storedTheme)
			: DashboardTheme::GT3;

#if GT7_DASHBOARD_THEME_PREVIEW >= 0 && GT7_DASHBOARD_THEME_PREVIEW <= 6
		activeDashboardTheme =
			static_cast<DashboardTheme>(GT7_DASHBOARD_THEME_PREVIEW);
#endif

		renderedDashboardTheme = activeDashboardTheme;
		previewDashboardTheme = activeDashboardTheme;
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
		prepareThemePreviewData();
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
		// Normalize ICE and EV energy into separate display and progress values.
		// For EVs, fuelLevel is treated as remaining kWh. The fixed 60 kWh
		// reference is only a visual scale because GT7 does not expose capacity.
		static constexpr float EV_CAPACITY_THRESHOLD = 0.1f;
		static constexpr float EV_VISUAL_FULL_KWH = 60.0f;
		static constexpr float MAX_REASONABLE_EV_KWH = 500.0f;
		const bool validCapacity = isfinite(data.fuelCapacity) &&
			data.fuelCapacity >= 0.0f;
		const bool validFuelLevel = isfinite(data.fuelLevel) &&
			data.fuelLevel >= 0.0f && data.fuelLevel < MAX_REASONABLE_EV_KWH;
		// Powertrain classification comes only from the capacity field (0x48).
		// Keep it independent from fuelLevel so a transient/invalid energy value
		// cannot make an EV label flicker back to FUEL.
		const bool detectedEV = validCapacity &&
			data.fuelCapacity < EV_CAPACITY_THRESHOLD;
		const bool validEvFuel = detectedEV && validFuelLevel &&
			data.fuelLevel > 0.0f;
		const bool validIceFuel = validCapacity && validFuelLevel &&
			data.fuelCapacity >= EV_CAPACITY_THRESHOLD;
		const bool nextFuelValueValid = validEvFuel || validIceFuel;
		const String nextFuelLabel = detectedEV ? "EV" : "FUEL";

		if (fuelIsEV != detectedEV || fuelValueValid != nextFuelValueValid)
		{
			derivedMetrics.fuel.reset();
			if (fuelLabel != nextFuelLabel)
				forceUpdate = true;
		}

		fuelIsEV = detectedEV;
		fuelValueValid = nextFuelValueValid;
		fuelLabel = nextFuelLabel;

		if (validEvFuel)
		{
			// GT7 updates EV energy in coarse steps. A whole-number display keeps
			// every theme compact and avoids implying unavailable precision.
			fuelDisplayValue = String(data.fuelLevel, 0);
			fuelProgressPercent = constrain(
				static_cast<int>(lroundf(
					data.fuelLevel / EV_VISUAL_FULL_KWH * 100.0f)),
				0,
				100);
		}
		else if (validIceFuel)
		{
			const float fuelPercent = constrain(
				(data.fuelLevel / data.fuelCapacity) * 100.0f,
				0.0f,
				100.0f);
			fuelDisplayValue = String(fuelPercent, 0);
			fuelProgressPercent = constrain(
				static_cast<int>(lroundf(fuelPercent)), 0, 100);
		}
		else
		{
			fuelDisplayValue = "--";
			fuelProgressPercent = 100;
		}

		// Keep legacy fields synchronized for existing protocol helpers.
		brakeBias = fuelDisplayValue;
		fuelAlertActive = String(fuelProgressPercent);

		for (int tyreIndex = 0; tyreIndex < 4; tyreIndex++)
		{
			tyreTemperatures[tyreIndex] = data.tyreTemp[tyreIndex];
		}

		// 由獨立 library 估算剩餘油量圈數。
		// 注意：GT7FuelEstimator 接收的是公升，不是百分比。
		if (validIceFuel)
			derivedMetrics.fuel.update(data.fuelLevel, currentLap);

		const float estimatedFuelLaps = validIceFuel
			? derivedMetrics.fuel.remainingLaps()
			: -1.0f;

		if (!fuelIsEV && estimatedFuelLaps >= 0.0f)
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
		fuelIsEV = false;
		fuelValueValid = brakeBias != "--" && brakeBias.length() > 0;
		fuelLabel = "FUEL";
		fuelDisplayValue = brakeBias;
		fuelProgressPercent = fuelValueValid
			? constrain(static_cast<int>(lroundf(fuelAlertActive.toFloat())), 0, 100)
			: 100;

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

		// Touch calibration is an idle-only recovery aid. Live telemetry always
		// wins immediately, discarding any unconfirmed candidate so this screen
		// can never hold the dashboard or its runtime services open.
		if (settingsScreen == SettingsScreen::TouchCalibration && previousGameRunning)
		{
			closeSettings();
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
			renderDashboard(activeDashboardTheme,
				static_cast<DashboardState &>(*this), forceUpdate);
		}
		else if (forceUpdate)
		{
			drawPage2();
		}

		forceUpdate = false;
	}

	void idle() {}

	void renderDashboard(DashboardTheme theme, DashboardState &state, bool forceUpdate)
	{
		switch (theme)
		{
		case DashboardTheme::Classic:
			drawPage1Legacy(state, forceUpdate);
			break;
		case DashboardTheme::Retro:
			drawRetroDashboard(state, forceUpdate);
			break;
		case DashboardTheme::Radar:
			drawRadarDashboard(state, forceUpdate);
			break;
		case DashboardTheme::Mono:
			drawMonoDashboard(state, forceUpdate);
			break;
		case DashboardTheme::Pocket:
			drawPocketDashboard(state, forceUpdate);
			break;
		case DashboardTheme::Endurance:
			drawEnduranceDashboard(state, forceUpdate);
			break;
		case DashboardTheme::GT3:
		default:
#if GT7_DASHBOARD_LEGACY_UI
			drawPage1Legacy(state, forceUpdate);
#else
			drawPage1(state, forceUpdate);
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

	void releaseSpriteAfterThemePreview(LGFX_Sprite &sprite, bool &created)
	{
		if (settingsScreen != SettingsScreen::ThemeSelection || !created) return;
		sprite.deleteSprite();
		created = false;
	}

#include "dashboard/DashboardIcons.inc"

#include "dashboard/themes/RetroTheme.inc"

#include "dashboard/themes/RadarTheme.inc"

#include "dashboard/themes/MonoTheme.inc"

#include "dashboard/themes/PocketTheme.inc"

#include "dashboard/themes/EnduranceTheme.inc"

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

	void drawTouchCalibrationHint(bool confirm)
	{
		const char *label = confirm ? "TOUCH AGAIN" : "TOUCH SETUP";
		const uint16_t textColor = confirm
			? tft.color565(218, 111, 148)
			: tft.color565(105, 105, 105);
		constexpr int iconRadius = 4;
		constexpr int iconGap = 5;
		tft.setTextFont(1);
		const int textWidth = tft.textWidth(label);
		const int groupWidth = iconRadius * 2 + iconGap + textWidth;
		const int groupX = X_CENTER - groupWidth / 2;
		const int iconX = groupX + iconRadius - 2;
		const int textX = groupX + iconRadius * 2 + iconGap;

		tft.fillRect(62, 4, 196, 29, TFT_BLACK);
		tft.drawCircle(iconX, 17, iconRadius, textColor);
		tft.fillCircle(iconX, 17, 1, textColor);
		tft.setTextDatum(ML_DATUM);
		tft.setTextColor(textColor, TFT_BLACK);
		tft.drawString(label, textX, 18, 1);
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

		// The asymmetric top-centre label lets a mismatched touch rotation be
		// inferred after two matching intentional taps while telemetry is idle.
		drawTouchCalibrationHint(false);

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

#include "dashboard/themes/GT3Theme.inc"

#include "dashboard/themes/ClassicTheme.inc"

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

	void clearThemePreviewRenderCache()
	{
		tft.fillScreen(TFT_BLACK);
		prevData.clear();
		prevColor.clear();
		prev_gear = "";
		themePreviewData.prev_rpmPercent = 50;
	}

	void drawThemeSelectorOverlay()
	{
		const uint16_t overlay = tft.color565(31, 34, 38);
		const uint16_t divider = tft.color565(82, 86, 92);
		const uint16_t inactive = tft.color565(82, 86, 92);
		const uint16_t applyFill = tft.color565(48, 70, 82);

		tft.fillRect(0, 0, SCREEN_WIDTH, 40, overlay);
		tft.drawFastHLine(0, 39, SCREEN_WIDTH, divider);
		tft.setTextDatum(MC_DATUM);
		tft.setTextColor(TFT_WHITE, overlay);
		for (int offset = -1; offset <= 1; ++offset)
		{
			tft.drawLine(42 + offset, 10, 32 + offset, 20, TFT_WHITE);
			tft.drawLine(32 + offset, 20, 42 + offset, 30, TFT_WHITE);
			tft.drawLine(SCREEN_WIDTH - 42 + offset, 10,
				SCREEN_WIDTH - 32 + offset, 20, TFT_WHITE);
			tft.drawLine(SCREEN_WIDTH - 32 + offset, 20,
				SCREEN_WIDTH - 42 + offset, 30, TFT_WHITE);
		}
		const String previewThemeName = dashboardThemeName(previewDashboardTheme);
		tft.setTextFont(2);
		tft.drawString(previewThemeName, X_CENTER, 17);
		tft.setTextFont(1);

		const int indicatorWidth = 6;
		const int indicatorHeight = 3;
		const int indicatorGap = 3;
		const int totalWidth = static_cast<int>(DASHBOARD_THEME_COUNT) * indicatorWidth +
			(static_cast<int>(DASHBOARD_THEME_COUNT) - 1) * indicatorGap;
		const int indicatorX = (SCREEN_WIDTH - totalWidth) / 2;
		const size_t previewIndex = dashboardThemeIndex(previewDashboardTheme);
		const size_t appliedIndex = dashboardThemeIndex(activeDashboardTheme);
		const uint16_t appliedColor = tft.color565(92, 176, 112);
		for (size_t i = 0; i < DASHBOARD_THEME_COUNT; ++i)
		{
			const uint16_t indicatorColor = i == appliedIndex
				? appliedColor
				: (i == previewIndex ? TFT_LIGHTGREY : inactive);
			tft.fillRect(indicatorX + static_cast<int>(i) *
				(indicatorWidth + indicatorGap), 31,
				indicatorWidth, indicatorHeight, indicatorColor);
		}

		tft.fillRect(0, 198, SCREEN_WIDTH, 42, overlay);
		tft.drawFastHLine(0, 198, SCREEN_WIDTH, divider);
		tft.fillRoundRect(12, 204, 140, 30, 5, overlay);
		tft.drawRoundRect(12, 204, 140, 30, 5, TFT_LIGHTGREY);
		tft.fillRoundRect(168, 204, 140, 30, 5, applyFill);
		tft.drawRoundRect(168, 204, 140, 30, 5, TFT_LIGHTGREY);
		tft.setTextColor(TFT_WHITE, overlay);
		tft.drawString("CANCEL", 82, 219, 2);
		tft.setTextColor(TFT_WHITE, applyFill);
		tft.drawString("APPLY", 238, 219, 2);
		tft.setTextDatum(TL_DATUM);
	}

	void drawTapToReturnHint()
	{
		const uint16_t hintFill = tft.color565(210, 82, 126);
		const int hintWidth = 112;
		const int hintHeight = 20;
		const int hintX = (SCREEN_WIDTH - hintWidth) / 2;
		const int hintY = SCREEN_HEIGHT - 30;
		tft.fillRoundRect(hintX, hintY, hintWidth, hintHeight,
			hintHeight / 2, hintFill);
		tft.setTextDatum(MC_DATUM);
		tft.setTextColor(TFT_WHITE, hintFill);
		tft.drawString("TAP TO RETURN", X_CENTER,
			hintY + hintHeight / 2, 1);
		tft.setTextDatum(TL_DATUM);
	}

	void renderThemePreview()
	{
		clearThemePreviewRenderCache();
		renderDashboard(previewDashboardTheme, themePreviewData, true);
		if (previewFullscreen)
			drawTapToReturnHint();
		else
			drawThemeSelectorOverlay();
	}

	void drawTouchCalibrationScreen(int pressedButton = -1)
	{
		tft.fillScreen(TFT_BLACK);
		tft.setTextPadding(0);
		tft.setTextDatum(MC_DATUM);
		tft.setTextColor(TFT_WHITE, TFT_BLACK);
		tft.drawString("TOUCH SETUP", X_CENTER, 27, 4);

		tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
		tft.drawString(
			touchCalibrationVerified ? "Touch verified" : "Tap the target to verify",
			X_CENTER, 59, 2);

		const uint16_t targetColor = touchCalibrationVerified
			? tft.color565(88, 190, 130)
			: tft.color565(210, 82, 126);
		tft.drawCircle(252, 104, 15, targetColor);
		tft.drawCircle(252, 104, 7, targetColor);
		tft.drawFastHLine(232, 104, 41, targetColor);
		tft.drawFastVLine(252, 84, 41, targetColor);

		drawSettingsButton(25, 169, 125, 50, "CANCEL", pressedButton == 1);
		drawSettingsButton(170, 169, 125, 50, "SAVE", pressedButton == 2,
			touchCalibrationVerified);
		tft.setTextDatum(TL_DATUM);
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
			renderThemePreview();
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
		else if (settingsScreen == SettingsScreen::TouchCalibration)
		{
			drawTouchCalibrationScreen(pressedButton);
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
			// Preview controls intentionally have no pressed-state redraw. Avoiding
			// a second full overlay pass keeps touch feedback flicker-free.
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
		else if (settingsScreen == SettingsScreen::TouchCalibration)
		{
			if (button == 1)
				drawSettingsButton(25, 169, 125, 50, "CANCEL", pressed);
			else if (button == 2)
				drawSettingsButton(170, 169, 125, 50, "SAVE", pressed,
					touchCalibrationVerified);
		}
		tft.setTextDatum(TL_DATUM);
	}

	void showSettingsScreen(SettingsScreen screen)
	{
		screenSleeping = false;
		screenOffByUser = false;
		tft.setBrightness(normalBrightness());
		currentBrightness = normalBrightness();
		if (screen == SettingsScreen::ThemeSelection &&
			settingsScreen != SettingsScreen::ThemeSelection)
		{
			previewDashboardTheme = activeDashboardTheme;
			previewFullscreen = false;
		}
		settingsScreen = screen;
		wifiResetConfirmOpen = screen == SettingsScreen::WifiResetConfirmation;
		settingsPressedButton = -1;
		settingsLastInteractionTime = millis();
		drawSettingsScreen();
	}

	void closeSettings()
	{
		settingsScreen = SettingsScreen::Closed;
		previewFullscreen = false;
		wifiResetConfirmOpen = false;
		settingsPressedButton = -1;
		touchCalibrationVerified = false;
		touchCalibrationEntryArmed = false;
		touchCalibrationAwaitingSecondTap = false;
		touchCalibrationFirstTapTime = 0;
		pendingTouchRotation = touchRotation;
		redrawAfterWifiResetDialog();
	}

	void showTouchCalibration(TouchRotation candidate)
	{
		pendingTouchRotation = candidate;
		touchCalibrationVerified = false;
		showSettingsScreen(SettingsScreen::TouchCalibration);
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
			if (previewFullscreen) return 5;
			if (touchInside(0, 0, SCREEN_WIDTH / 3, 40)) return 0;
			if (touchInside(SCREEN_WIDTH * 2 / 3, 0,
				SCREEN_WIDTH - SCREEN_WIDTH * 2 / 3, 40)) return 1;
			if (touchInside(0, 40, SCREEN_WIDTH, 158)) return 2;
			if (touchInside(0, 198, SCREEN_WIDTH / 2, 42)) return 3;
			if (touchInside(SCREEN_WIDTH / 2, 198, SCREEN_WIDTH / 2, 42)) return 4;
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
		else if (settingsScreen == SettingsScreen::TouchCalibration)
		{
			if (touchInside(224, 76, 56, 56)) return 0;
			const bool originalCancel =
				originalTouchX >= 25 && originalTouchX < 150 &&
				originalTouchY >= 169 && originalTouchY < 219;
			if (touchInside(25, 169, 125, 50) || originalCancel) return 1;
			if (touchCalibrationVerified && touchInside(170, 169, 125, 50)) return 2;
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
			if (button == 0 || button == 1)
			{
				const size_t currentIndex = dashboardThemeIndex(previewDashboardTheme);
				const size_t nextIndex = button == 0
					? (currentIndex + DASHBOARD_THEME_COUNT - 1) % DASHBOARD_THEME_COUNT
					: (currentIndex + 1) % DASHBOARD_THEME_COUNT;
				previewDashboardTheme = DASHBOARD_THEMES[nextIndex].id;
				settingsLastInteractionTime = millis();
				renderThemePreview();
			}
			else if (button == 2)
			{
				previewFullscreen = true;
				settingsLastInteractionTime = millis();
				renderThemePreview();
			}
			else if (button == 3)
			{
				closeSettings();
			}
			else if (button == 4)
			{
				const DashboardTheme selectedTheme = previewDashboardTheme;
				const bool changed = selectedTheme != activeDashboardTheme;
				settingsScreen = SettingsScreen::Closed;
				wifiResetConfirmOpen = false;
				settingsPressedButton = -1;
				previewFullscreen = false;
				selectDashboardTheme(selectedTheme, true);
				if (!changed) redrawAfterWifiResetDialog();
			}
			else if (button == 5)
			{
				previewFullscreen = false;
				settingsLastInteractionTime = millis();
				renderThemePreview();
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
		else if (settingsScreen == SettingsScreen::TouchCalibration)
		{
			if (button == 0)
			{
				touchCalibrationVerified = true;
				settingsLastInteractionTime = millis();
				drawTouchCalibrationScreen();
			}
			else if (button == 1)
			{
				closeSettings();
			}
			else if (button == 2 && touchCalibrationVerified)
			{
				touchRotation = pendingTouchRotation;
				if (dashboardPreferencesReady)
					dashboardPreferences.putUChar(
						"touchRot", static_cast<uint8_t>(touchRotation));
				closeSettings();
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
		uint16_t rawTouchX = 0;
		uint16_t rawTouchY = 0;
		const bool isTouched = tft.getTouch(&rawTouchX, &rawTouchY);
		if (isTouched)
		{
			applyTouchRotation(
				rawTouchX, rawTouchY, touchRotation, originalTouchX, originalTouchY);
			const TouchRotation effectiveRotation =
				settingsScreen == SettingsScreen::TouchCalibration
					? pendingTouchRotation
					: touchRotation;
			applyTouchRotation(
				rawTouchX, rawTouchY, effectiveRotation, touchX, touchY);
		}

		if (touchCalibrationAwaitingSecondTap &&
			millis() - touchCalibrationFirstTapTime > TOUCH_CALIBRATION_DOUBLE_TAP_MS)
		{
			touchCalibrationAwaitingSecondTap = false;
			touchCalibrationFirstTapTime = 0;
			pendingTouchRotation = touchRotation;
			if (!previousGameRunning && !screenSleeping &&
				settingsScreen == SettingsScreen::Closed)
				drawTouchCalibrationHint(false);
		}

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
			if (settingsScreen == SettingsScreen::Closed)
			{
				const int candidate = calibrationRotationForTouch(rawTouchX, rawTouchY);
				touchCalibrationEntryArmed = candidate >= 0;
				if (candidate >= 0)
				{
					if (touchCalibrationAwaitingSecondTap &&
						candidate != static_cast<int>(pendingTouchRotation))
					{
						touchCalibrationAwaitingSecondTap = false;
						touchCalibrationFirstTapTime = 0;
					}
					pendingTouchRotation = static_cast<TouchRotation>(candidate);
				}
			}
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
			if (touchCalibrationEntryArmed)
			{
				if (touchCalibrationAwaitingSecondTap &&
					millis() - touchCalibrationFirstTapTime <= TOUCH_CALIBRATION_DOUBLE_TAP_MS)
				{
					touchCalibrationAwaitingSecondTap = false;
					showTouchCalibration(pendingTouchRotation);
				}
				else
				{
					touchCalibrationAwaitingSecondTap = true;
					touchCalibrationFirstTapTime = millis();
					drawTouchCalibrationHint(true);
				}
			}
			else
			{
				touchCalibrationAwaitingSecondTap = false;
				touchCalibrationFirstTapTime = 0;
				showSettingsScreen(SettingsScreen::Main);
			}
			touchCalibrationEntryArmed = false;
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
