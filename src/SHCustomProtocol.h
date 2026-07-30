
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
#include <map>
#include <BleGamepad.h> // libreria bluetooth
#include <GT7DerivedMetrics.h>

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
}

int currentPage = 1;	  // Variabile per tenere traccia della pagina corrente
bool forceUpdate = false; // Variabile per forzare l'aggiornamento delle celle

class SHCustomProtocol
{
private:
	// Global variables
	int rpmPercent = 50;
	int prev_rpmPercent = 50;
	int rpmRedLineSetting = 90;
	String gear = "N";
	String prev_gear;
	String speed = "0";
	String currentLapTime = "00:00.00";
	String lastLapTime = "00:00.00";
	String bestLapTime = "00.00.00";
	String sessionBestLiveDeltaSeconds = "0.000";
	String sessionBestLiveDeltaProgressSeconds = "0.00";
	String tyrePressureFrontLeft = "00.0";
	String tyrePressureFrontRight = "00.0";
	String tyrePressureRearLeft = "00.0";
	String fuelAlertActive = "False";
	String tcLevel = "0";
	String tcActive = "0";
	String absLevel = "0";
	String absActive = "0";
	String isTCCutNull = "True";
	String tcTcCut = "0  0";
	String brakeBias = "0";
	String brake = "0";
	String lapInvalidated = "False";
	uint16_t touchX, touchY; // definisce i due interi per gestione touchscreen
	int numButtons = 6;		 // Variabile per il numero di pulsanti

	String gameRunning = "False";

	static constexpr uint8_t FADE_STEP = 5;
	static constexpr uint8_t FADE_DELAY_MS = 4;
	static constexpr unsigned long SCREEN_SLEEP_TIMEOUT = 5UL * 60UL * 1000UL;
	// static constexpr unsigned long SCREEN_SLEEP_TIMEOUT = 10000;
	static constexpr uint8_t SCREEN_NORMAL_BRIGHTNESS = 255;
	static constexpr bool TOUCH_SCREEN_CONTROL_ENABLED = true;

	// Connecting 畫面動畫設定
	static constexpr unsigned long CONNECT_ANIMATION_INTERVAL = 1000;
	static constexpr int CONNECT_DOT_COUNT = 5;

	uint8_t currentBrightness = SCREEN_NORMAL_BRIGHTNESS;
	unsigned long gameStoppedTime = 0;

	bool gameStoppedTimerStarted = false;
	bool screenSleeping = false;

	// 是否由使用者手動關閉
	bool screenOffByUser = false;

	// 全螢幕長按 5 秒後顯示 Wi-Fi 重設確認畫面
	static constexpr unsigned long WIFI_RESET_HOLD_MS = 5000UL;
	bool wifiResetConfirmOpen = false;
	bool wifiResetRequested = false;

	// 記錄上一筆 GT7 執行狀態，用來偵測 False -> True
	bool previousGameRunning = false;

	// 避免第一筆資料被誤判為狀態切換
	bool gameRunningInitialized = false;

#if INCLUDE_GT7_WIFI
	// 最近一次真正收到新 GT7 UDP 封包的時間
	uint32_t lastGT7PacketTime = 0;
	int32_t lastGT7PacketId = -1;
	bool hasReceivedGT7Packet = false;
	bool gt7CarOnTrack = false;

	// GT7 衍生數據：ABS 與剩餘油量圈數。
	GT7DerivedMetrics derivedMetrics;

#endif

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
			return "--";
		}

		char buffer[16];
		snprintf(buffer, sizeof(buffer), "%+.3f", value);
		return String(buffer);
	}

	String formatLastBestDifference(int32_t lastLapMs, int32_t bestLapMs)
	{
		if (lastLapMs <= 0 || bestLapMs <= 0)
		{
			return "--";
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
		tft.setRotation(3);
		tft.setBrightness(SCREEN_NORMAL_BRIGHTNESS);
		currentBrightness = SCREEN_NORMAL_BRIGHTNESS;
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

		if (maxRpm > 0.0f)
		{
			rpmPercent = constrain(
				static_cast<int>((rpm / maxRpm) * 100.0f),
				0,
				100);

			rpmRedLineSetting = constrain(
				static_cast<int>((redLineRpm / maxRpm) * 100.0f),
				0,
				100);
		}
		else
		{
			rpmPercent = 0;
			rpmRedLineSetting = 90;
		}

		// 圈速：currentLap 只有使用 C Packet 時才有值
		currentLapTime = formatLapTimeMs(data.currentLap);
		lastLapTime = formatLapTimeMs(data.lastLaptime);
		bestLapTime = formatLapTimeMs(data.bestLaptime);

		// 圈數
		const int currentLap = max(0, static_cast<int>(data.lapCount));
		const int totalLapCount = max(0, static_cast<int>(data.totalLaps));

		// 上一圈與最佳圈的圈速差。新最佳圈會顯示 +0.000。
		sessionBestLiveDeltaSeconds =
			formatLastBestDifference(data.lastLaptime, data.bestLaptime);

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

		brakeBias = String(fuelPercent, 0);
		fuelAlertActive = String(fuelPercent, 1);

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

		const uint16_t flags = static_cast<uint16_t>(data.flags);

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

		// 暗屏時仍需讀取觸控，才能點擊喚醒或長按進入 Wi-Fi 重設。
		readTouch();

		// 確認畫面開啟時，不讓 Dashboard 或 Connecting 畫面蓋回來。
		if (wifiResetConfirmOpen)
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

		if (currentPage == 1)
		{
			drawPage1(forceUpdate);
		}
		else if (forceUpdate)
		{
			drawPage2();
		}

		forceUpdate = false;
	}

	void idle() {}

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

		char text[40];
		snprintf(
			text,
			sizeof(text),
			"Touch & hold %ds to change WiFi",
			RESET_WAITING_TIME);

		tft.drawCentreString(
			text,
			SCREEN_WIDTH / 2,
			SCREEN_HEIGHT - 26,
			1);

		tft.drawCentreString(
			"v1.2.0  |  by caa1211",
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

	void drawPage1(bool forceUpdate = false)
	{
		drawRpmMeter(0, 0, SCREEN_WIDTH, HALF_CELL_HEIGHT);
		drawGear(COL[2], COL[1]);

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
		if (sessionBestLiveDeltaSeconds != "--")
		{
			lastBestColor = sessionBestLiveDeltaSeconds.startsWith("+") &&
									sessionBestLiveDeltaSeconds != "+0.000"
								? TFT_RED
								: TFT_GREEN;
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
			constrain(absLevel.toInt(), 0, 100),
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
		else
		{
			fuelStatus = "PIT";
			fuelStatusColor = TFT_RED;
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

		/*
		 * 首次繪製或切回主畫面時：
		 * 清一次內部，再完整畫出目前的 Bar。
		 */
		if (forceUpdate)
		{
			tft.fillRect(
				innerX,
				innerY,
				innerWidth,
				innerHeight,
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

		prev_rpmPercent = safeRpmPercent;
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
		int brakePercent,
		bool forceUpdate = false)
	{
		throttlePercent = constrain(throttlePercent, 0, 100);
		brakePercent = constrain(brakePercent, 0, 100);

		// 上下兩格，而不是左右兩格。
		const int32_t gap = 2;
		const int32_t cellHeight = (totalHeight - gap) / 2;
		const int32_t throttleY = y;
		const int32_t brakeY = y + cellHeight + gap;

		auto updateOneCell = [&](
								 int32_t cellY,
								 int percent,
								 const char *id,
								 uint16_t color)
		{
			const bool firstDraw = prevData.count(id) == 0;
			const int oldPercent = firstDraw
									   ? 0
									   : constrain(prevData[id].toInt(), 0, 100);

			if (!forceUpdate && !firstDraw && oldPercent == percent)
			{
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
					tft.fillRect(fillX, fillY, newFill, fillHeight, color);
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
					color);
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
			tft.drawRoundRect(x, cellY, totalWidth, cellHeight, 5, TFT_DARKGREY);
			prevData[id] = String(percent);
		};

		updateOneCell(throttleY, throttlePercent, "pedalThrottle", TFT_YELLOW);
		updateOneCell(brakeY, brakePercent, "pedalBrake", TFT_BLUE);
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
		fadeToBrightness(SCREEN_NORMAL_BRIGHTNESS);
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

	void showWifiResetConfirm()
	{
		// 即使原本處於暗屏，也要先亮起確認畫面。
		screenSleeping = false;
		screenOffByUser = false;
		tft.setBrightness(SCREEN_NORMAL_BRIGHTNESS);
		currentBrightness = SCREEN_NORMAL_BRIGHTNESS;

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

	void readTouch()
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
};

#endif
