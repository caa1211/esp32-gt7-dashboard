
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

static LGFX tft;

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

int currentPage = 1; // Variabile per tenere traccia della pagina corrente
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
    uint16_t touchX, touchY;	// definisce i due interi per gestione touchscreen

	int numButtons = 6; // Variabile per il numero di pulsanti

	String gameRunning = "False";

	static constexpr unsigned long SCREEN_SLEEP_TIMEOUT = 5UL * 60UL * 1000UL;

	// static constexpr unsigned long SCREEN_SLEEP_TIMEOUT = 10000;

	static constexpr uint8_t SCREEN_NORMAL_BRIGHTNESS = 255;

	unsigned long gameStoppedTime = 0;

	bool gameStoppedTimerStarted = false;
	bool screenSleeping = false;

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
	//    Serial.begin(115200);   //x debug
    //    Serial.println("Test seriale avviato!"); //x debug
		
		tft.init();
		tft.setRotation(3);
		tft.fillScreen(TFT_BLACK);

		screenSleeping = false;
		gameStoppedTimerStarted = false;

		bleGamepadConfig.setAutoReport(true);  // in false non invia i comandi a windows
        bleGamepadConfig.setAxesMax(32760);
        bleGamepadConfig.setIncludeSlider1(false);
        bleGamepadConfig.setIncludeXAxis(false);
        bleGamepadConfig.setIncludeYAxis(false);
        bleGamepadConfig.setIncludeZAxis(false);
        bleGamepadConfig.setIncludeRxAxis(false);
        bleGamepadConfig.setIncludeRyAxis(false);
        bleGamepadConfig.setIncludeRzAxis(false);
        bleGamepadConfig.setButtonCount(numButtons);  // Variabile per il numero di pulsanti
        bleGamepad.begin(&bleGamepadConfig);
    // Serial.println("Configurazione BleGamepad completata.");   //x debug
	}

	void read()
	{
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

		const bool isGameRunning =
			gameRunning == "True" ||
			gameRunning == "true" ||
			gameRunning == "TRUE" ||
			gameRunning == "1";

		if (isGameRunning)
		{
			// 遊戲已連線，取消關屏倒數
			gameStoppedTimerStarted = false;

			// 如果螢幕已休眠，立即恢復背光
			if (screenSleeping)
			{
				screenSleeping = false;
				tft.setBrightness(SCREEN_NORMAL_BRIGHTNESS);
				forceUpdate = true;
			}
		}
		else
		{
			// 第一次收到遊戲未連線狀態時，開始倒數
			if (!gameStoppedTimerStarted)
			{
				gameStoppedTimerStarted = true;
				gameStoppedTime = millis();
			}
		}
	}

	// Called once per arduino loop, timing can't be predicted,
	// but it's called between each command sent to the arduino
	void loop()
	{

		if (!screenSleeping &&
			gameStoppedTimerStarted &&
			millis() - gameStoppedTime >= SCREEN_SLEEP_TIMEOUT)
		{
			screenSleeping = true;
			tft.setBrightness(0);
		}
		if (screenSleeping)
		{
			return;
		}

		readTouch(); // Leggi il touchscreen dello schermo

        static int lastPage = currentPage;
        if (currentPage != lastPage) {
            tft.fillScreen(TFT_BLACK);
            forceUpdate = true;
            lastPage = currentPage;
        }
		
		if (currentPage == 1) {
            drawPage1(forceUpdate);
        } else {
            drawPage2();
        }

        forceUpdate = false;

		if (tft.getTouch(&touchX, &touchY)) {
            if (touchX < 50 && touchY < 50) {
                // Cambia pagina quando tocchi nell'angolo in alto a sinistra
                currentPage = (currentPage == 1) ? 2 : 1;
            }
        }
	}

	void idle() {}

	bool isActiveValue(String value)
	{
		value.trim();
		return value == "True" ||
			   value == "true" ||
			   value == "TRUE" ||
			   value == "1";
	}

	void drawPage1(bool forceUpdate = false) {
		drawRpmMeter(0, 0, SCREEN_WIDTH, HALF_CELL_HEIGHT);
		drawGear(COL[2], COL[1]);
				
		// First+Second Column (Lap times)
		drawCell(COL[0], ROW[1], bestLapTime, "bestLapTime", "Best Lap", "left", TFT_WHITE, 4, forceUpdate);
		drawCell(COL[0], ROW[2], lastLapTime, "lastLapTime", "Last Lap", "left", TFT_WHITE, 4, forceUpdate);
		drawCell(COL[0], ROW[3], currentLapTime, "currenLapTime", "Current Lap", "left", lapInvalidated == "True" ? TFT_RED : TFT_WHITE, 4, forceUpdate);

		// Third Column (speed)
		drawCell(COL[2], ROW[3], speed, "speed", "Speed", "center", TFT_WHITE, 4, forceUpdate);

		// Fourth+Fifth Column (delta)
		drawCell(SCREEN_WIDTH, ROW[1], sessionBestLiveDeltaSeconds, "sessionBestLiveDeltaSeconds", "Delta", "right", sessionBestLiveDeltaSeconds.indexOf('-') >= 0 ? TFT_GREEN : TFT_RED, 4, forceUpdate);
		drawCell(SCREEN_WIDTH, ROW[2], sessionBestLiveDeltaProgressSeconds, "sessionBestLiveDeltaProgressSeconds", "Delta P", "right", sessionBestLiveDeltaProgressSeconds.indexOf('-') >= 0 ? TFT_GREEN : TFT_RED, 4, forceUpdate);

		// 油門、煞車、油量
		const uint16_t throttleColor = isActiveValue(tcActive) ? TFT_RED : TFT_YELLOW;
		const uint16_t brakeColor = isActiveValue(absActive) ? TFT_RED : TFT_BLUE;
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

		if (isTCCutNull == "False")
			drawCell(COL[0], ROW[4], tcTcCut, "tcTcCut", "TC TC2", "center", throttleColor, 4, forceUpdate);
		else
			drawCell(COL[0], ROW[4], tcLevel, "tcLevel", "THR", "center", throttleColor, 4, forceUpdate);

		drawCell(COL[1], ROW[4], absLevel, "absLevel", "BRK", "center", brakeColor, 4, forceUpdate);
		drawCell(COL[2], ROW[4], brakeBias, "brakeBias", "FUEL", "center", TFT_MAGENTA, 4, forceUpdate);

		// 剩餘圈數、位置、圈數、低油量警示
		drawCell(COL[3], ROW[3], tyrePressureFrontLeft, "tyrePressureFrontLeft", "F/LAP", "center", TFT_CYAN, 4, forceUpdate);
		drawCell(COL[4], ROW[3], tyrePressureFrontRight, "tyrePressureFrontRight", "POS", "center", TFT_CYAN, 4, forceUpdate);
		drawCell(COL[3], ROW[4], tyrePressureRearLeft, "tyrePressureRearLeft", "LAP", "center", TFT_CYAN, 4, forceUpdate);
		drawCell(
			COL[4],
			ROW[4],
			fuelStatus,
			"fuelAlert",
			"FUEL!",
			"center",
			fuelStatusColor,
			4,
			forceUpdate);
	}

	void drawColoredButton(int x, int y, int width, int height, String label, uint16_t color)   //disegna i pulsanti della pagina 2
	{
    // Draw the dark background for the button
    tft.fillRoundRect(x, y, width, height, 10, TFT_BLACK);

    // Draw the top and bottom color edges
    tft.fillRoundRect(x + 2, y + 14, width - 4, height / 5, 10, color); // Top edge
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

    for (int i = 0; i < numButtons; i++) {
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
		int meterWidth = (width * rpmPercent) / 100;

		int yPlusOne = y + 1;
		int innerWidth = width - meterWidth - 1;
		int innerHeight = height - 4;

		if (prev_rpmPercent > rpmPercent)
		{
			tft.fillRect(meterWidth, yPlusOne, innerWidth, innerHeight, TFT_BLACK); // clear the part after the current rect width
		}

		if (rpmPercent >= rpmRedLineSetting)
		{
			tft.fillRect(x, yPlusOne, meterWidth - 2, innerHeight, TFT_RED);
		}
		else if (rpmPercent >= rpmRedLineSetting - 5)
		{
			tft.fillRect(x, yPlusOne, meterWidth - 2, innerHeight, TFT_ORANGE);
		}
		else
		{
			tft.fillRect(x, yPlusOne, meterWidth - 2, innerHeight, TFT_GREEN);
		}

		// draw the frame only if it ont there
		if (prev_rpmPercent == 50) tft.drawRect(x, y, width, height-2, TFT_WHITE);
		
		prev_rpmPercent = rpmPercent;
	}

	void drawCell2(int32_t x, int32_t y, String data, String id, String name = "Data", String align = "center", int32_t color = TFT_WHITE, int fontSize = 4, bool forceUpdate = false)
	{
		const static int titleHeight = 19;
		const static int hPadding = 5;
		const static int vPadding = 1;

		tft.setTextColor(color, TFT_BLACK);

		bool dataChanged =  (prevData[id] != data) || forceUpdate;
		bool colorChanged =  (prevColor[id] != color) || forceUpdate;

		if (dataChanged || colorChanged) {

			if (align == "left")
			{
				
				if (colorChanged) tft.drawRoundRect(x, y, CELL_WIDTH * 2 - 1, CELL_HEIGHT - 2, 5, color);		// Rectangle
				if (colorChanged) tft.drawString(name, x + hPadding, y + vPadding, 2);						// Title
				tft.drawString(data, x + hPadding, y + titleHeight, fontSize); // Data
			}
			else if (align == "right")
			{
				if (colorChanged) tft.drawRoundRect(x - (CELL_WIDTH * 2), y, CELL_WIDTH * 2 - 1, CELL_HEIGHT - 2, 5, color); // Rectangle
				if (colorChanged) tft.drawRightString(name, x - hPadding, y + vPadding, 2);						// Title
				tft.drawRightString(data, x - hPadding, y + titleHeight, fontSize);	  // Data
			}
			else // "center"
			{
				if (colorChanged) tft.drawRoundRect(x, y, CELL_WIDTH - 2, CELL_HEIGHT - 2, 5, color);	 // Rectangle
				if (colorChanged) tft.drawCentreString(name, x + HALF_CELL_WIDTH, y + vPadding, 2);			 // Title
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

	void drawCell(
		int32_t x,
		int32_t y,
		String data,
		String id,
		String name = "Data",
		String align = "center",
		int32_t color = TFT_WHITE,
		int fontSize = 4,
		bool forceUpdate = false)
	{
		const static int titleHeight = 19;
		const static int hPadding = 5;
		const static int vPadding = 1;

		bool dataChanged =
			(prevData[id] != data) || forceUpdate;

		bool colorChanged =
			(prevColor[id] != color) || forceUpdate;

		if (!dataChanged && !colorChanged)
			return;

		/*
		 * 值有變動時，先用黑色重畫舊文字。
		 * 不判斷字串長度或實際寬度，避免任何殘影。
		 */
		if (dataChanged && prevData[id].length() > 0)
		{
			tft.setTextColor(TFT_BLACK, TFT_BLACK);

			if (align == "left")
			{
				tft.drawString(
					prevData[id],
					x + hPadding,
					y + titleHeight,
					fontSize);
			}
			else if (align == "right")
			{
				tft.drawRightString(
					prevData[id],
					x - hPadding,
					y + titleHeight,
					fontSize);
			}
			else
			{
				tft.drawCentreString(
					prevData[id],
					x + HALF_CELL_WIDTH,
					y + titleHeight,
					fontSize);
			}
		}

		// 設定新文字顏色
		tft.setTextColor(color, TFT_BLACK);

		if (align == "left")
		{
			if (colorChanged)
			{
				tft.drawRoundRect(
					x,
					y,
					CELL_WIDTH * 2 - 1,
					CELL_HEIGHT - 2,
					5,
					color);

				tft.drawString(
					name,
					x + hPadding,
					y + vPadding,
					2);
			}

			tft.drawString(
				data,
				x + hPadding,
				y + titleHeight,
				fontSize);
		}
		else if (align == "right")
		{
			if (colorChanged)
			{
				tft.drawRoundRect(
					x - CELL_WIDTH * 2,
					y,
					CELL_WIDTH * 2 - 1,
					CELL_HEIGHT - 2,
					5,
					color);

				tft.drawRightString(
					name,
					x - hPadding,
					y + vPadding,
					2);
			}

			tft.drawRightString(
				data,
				x - hPadding,
				y + titleHeight,
				fontSize);
		}
		else
		{
			if (colorChanged)
			{
				tft.drawRoundRect(
					x,
					y,
					CELL_WIDTH - 2,
					CELL_HEIGHT - 2,
					5,
					color);

				tft.drawCentreString(
					name,
					x + HALF_CELL_WIDTH,
					y + vPadding,
					2);
			}

			tft.drawCentreString(
				data,
				x + HALF_CELL_WIDTH,
				y + titleHeight,
				fontSize);
		}

		prevData[id] = data;
		prevColor[id] = color;
	}

	void readTouch()
	{
		if (currentPage == 2) {
			if (tft.getTouch(&touchX, &touchY)) {
				int buttonIndex = getTouchButtonIndex(touchX, touchY);

				if (buttonIndex != -1) {
					// Effetto di pressione
					int x = (buttonIndex % 3) * (SCREEN_WIDTH / 3);
					int y = (buttonIndex / 3) * (SCREEN_HEIGHT / 2);
					drawColoredButton(x, y, SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2, "Pressed", TFT_DARKGREY);

					bleGamepad.press(buttonIndex + 1);
					delay(100); // debounce delay
					bleGamepad.release(buttonIndex + 1);

					// Ripristina il pulsante dopo la pressione
					String buttonLabels[] = {"Button 1", "Button 2", "Button 3", "Button 4", "Button 5", "Button 6"};
					uint16_t buttonColors[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_ORANGE, TFT_PURPLE};
					drawColoredButton(x, y, SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2, buttonLabels[buttonIndex], buttonColors[buttonIndex]);
				}
			}
		}
	}

	int getTouchButtonIndex(int x, int y)
	{
		int buttonWidth = SCREEN_WIDTH / 3;
		int buttonHeight = SCREEN_HEIGHT / 2;
		int col = x / buttonWidth;
		int row = y / buttonHeight;
		int index = col + row * 3;
		return (index < numButtons) ? index : -1; // Restituisce l'indice del pulsante (0 a numButtons-1)
	}
};

#endif
