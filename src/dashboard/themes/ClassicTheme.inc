	void drawPage1Legacy(DashboardState &state, bool forceUpdate = false)
	{
		drawRpmMeter(state, 0, 0, SCREEN_WIDTH, HALF_CELL_HEIGHT);
		drawGear(state.gear, COL[2], COL[1]);
		drawTyreTemperatureStrip(
			COL[2] + 1,
			ROW[3] - 22,
			CELL_WIDTH - 2,
			18,
			state.gear,
			state.tyreTemperatures,
			forceUpdate);

		drawLapCellSprite(
			COL[0],
			ROW[1],
			state.bestLapTime,
			"bestLapTime",
			"Best Lap",
			TFT_WHITE,
			forceUpdate);

		drawLapCellSprite(
			COL[0],
			ROW[2],
			state.lastLapTime,
			"lastLapTime",
			"Last Lap",
			TFT_WHITE,
			forceUpdate);

		drawLapCellSprite(
			COL[0],
			ROW[3],
			state.currentLapTime,
			"currentLapTime",
			"Current Lap",
			state.lapInvalidated == "True" ? TFT_RED : TFT_WHITE,
			forceUpdate);

		// Third Column (speed)
		drawCell(COL[2], ROW[3], state.speed, "speed", "Speed", "center", TFT_WHITE, 4, forceUpdate);

		// 右上：上一圈相對最佳圈的差值。
		uint16_t lastBestColor = TFT_WHITE;
		if (state.sessionBestLiveDeltaSeconds.startsWith("-"))
		{
			lastBestColor = TFT_GREEN;
		}
		else if (
			state.sessionBestLiveDeltaSeconds.startsWith("+") &&
			state.sessionBestLiveDeltaSeconds != "+0.000")
		{
			lastBestColor = TFT_RED;
		}
		drawCell(
			SCREEN_WIDTH,
			ROW[1],
			state.sessionBestLiveDeltaSeconds,
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
			state.tcLevel == "--" ? -1 : constrain(state.tcLevel.toInt(), 0, 100),
			constrain(state.tcFilteredLevel.toInt(), 0, 100),
			state.absLevel == "--" ? -1 : constrain(state.absLevel.toInt(), 0, 100),
			constrain(state.absFilteredLevel.toInt(), 0, 100),
			forceUpdate);

		// ABS、TCS 狀態與油量
		const bool tcsOn = isActiveValue(state.tcActive);
		const bool absOn = isActiveValue(state.absActive);
		const uint16_t tcsColor = tcsOn ? TFT_YELLOW : TFT_DARKGREY;
		const uint16_t absColor = absOn ? TFT_RED : TFT_DARKGREY;
		float fuelPercentValue = state.fuelProgressPercent;

		// Fuel
		String fuelStatus;
		uint16_t fuelStatusColor;

		if (!state.fuelValueValid || state.fuelIsEV)
		{
			fuelStatus = "--";
			fuelStatusColor = TFT_DARKGREY;
		}
		else if (fuelPercentValue > 50.0f)
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
		drawCell(COL[2], ROW[4], state.fuelDisplayValue, "brakeBias", state.fuelLabel, "center", TFT_MAGENTA, 4, forceUpdate);

		// 剩餘圈數、位置、圈數、低油量警示
		drawCell(COL[3], ROW[3], state.tyrePressureFrontLeft, "tyrePressureFrontLeft", "REM", "center", TFT_CYAN, 4, forceUpdate);
		drawCell(COL[4], ROW[3], state.tyrePressureFrontRight, "tyrePressureFrontRight", "POS", "center", TFT_CYAN, 4, forceUpdate);
		drawCell(COL[3], ROW[4], state.tyrePressureRearLeft, "tyrePressureRearLeft", "LAP", "center", TFT_CYAN, 4, forceUpdate);
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

	void drawGear(const String &displayGear, int32_t x, int32_t y)
	{
		// draw gear only when it changes
		if (displayGear != prev_gear)
		{
			// tft.loadFont("Formula1_Regular_web_072pt7b", SPIFFS);
			tft.setTextColor(TFT_YELLOW, TFT_BLACK);
			tft.setTextSize(8);
			tft.setTextDatum(MC_DATUM);
			tft.setCursor(x + 12, y + HALF_CELL_HEIGHT);
			tft.print(displayGear);
			tft.setTextSize(1);
			tft.setTextDatum(TL_DATUM);

			prev_gear = displayGear;
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

	void drawRpmMeter(DashboardState &state, int32_t x, int32_t y,
		int width, int height)
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
			constrain(state.rpmPercent, 0, 100);

		const int safePrevRpmPercent =
			constrain(state.prev_rpmPercent, 0, 100);

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

		const int markerX = state.rpmAlertRangeValid
			? innerX + lroundf((innerWidth - 1) *
				constrain(state.rpmRedLineSetting / 100.0f, 0.0f, 1.0f))
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

				if (segmentPercent >= state.rpmRedLineSetting)
				{
					segmentColor = TFT_RED;
				}
				else if (
					segmentPercent >=
					state.rpmRedLineSetting - 5)
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

		state.prev_rpmPercent = safeRpmPercent;
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
		const String &displayGear,
		const float temperatures[4],
		bool forceUpdate = false,
		uint8_t gap = 2)
	{
		const int blockWidth = (width - gap) / 2;
		const int blockHeight = (height - gap) / 2;
		const String gearId = "tyreTemperatureGear";
		const bool gearChanged = prevData[gearId] != displayGear;

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
			const float temperature = temperatures[tyreIndex];
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

		prevData[gearId] = displayGear;
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
			spriteCreated = sprite.createSprite(cellWidth, cellHeight) != nullptr;
		}

		if (!spriteCreated)
		{
			// A failed heap allocation previously left all three Classic lap cells
			// permanently blank. Draw directly for this frame and retry allocation
			// on a later update instead of caching an invisible result.
			tft.fillRect(x, y, cellWidth, cellHeight, TFT_BLACK);
			tft.drawRoundRect(x, y, cellWidth, cellHeight, 5, color);
			tft.setTextColor(color, TFT_BLACK);
			tft.setTextDatum(TL_DATUM);
			tft.drawString(name, x + 5, y + 1, 2);
			tft.drawString(data, x + 5, y + 19, 4);
			return;
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
		releaseSpriteAfterThemePreview(sprite, spriteCreated);
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
		throttlePercent = constrain(throttlePercent, -1, 100);
		throttleAppliedPercent = constrain(throttleAppliedPercent, 0, 100);
		brakePercent = constrain(brakePercent, -1, 100);
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
            if (percent < 0) {
                if (forceUpdate || prevData[id] != "--") {
                    tft.fillRect(x, cellY, totalWidth, cellHeight, TFT_BLACK);
                    tft.drawRoundRect(x, cellY, totalWidth, cellHeight, 5, TFT_DARKGREY);
                    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK); tft.setTextDatum(MC_DATUM);
                    tft.drawString("--", x + totalWidth / 2, cellY + cellHeight / 2, 2);
                    prevData[id] = "--";
                }
                return;
            }
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
				if (settingsScreen == SettingsScreen::ThemeSelection)
				{
					classicPedalSprite.deleteSprite();
					spriteWidth = 0;
					spriteHeight = 0;
				}
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
