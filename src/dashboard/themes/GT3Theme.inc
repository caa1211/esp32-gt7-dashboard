	void drawPage1(DashboardState &state, bool forceUpdate = false)
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

			const char *bottomLabels[] = {state.fuelLabel.c_str(), "LEFT", "LAP", "POS", "ABS", "TCS"};
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

		drawRpmMeterGT3(state, forceUpdate);
		drawDashboardValueBuffered(DashboardLayout::LEFT_X, DashboardLayout::SPEED_VALUE_Y,
			DashboardLayout::SIDE_WIDTH, DashboardLayout::SPEED_VALUE_H,
			state.speed, "gt3Speed", TFT_WHITE, &fonts::DejaVu56,
			MC_DATUM, forceUpdate, speedSprite, speedSpriteCreated, 0.72f, 0.72f);

		uint16_t deltaColor = TFT_WHITE;
		if (state.sessionBestLiveDeltaSeconds.startsWith("-")) deltaColor = TFT_GREEN;
		else if (state.sessionBestLiveDeltaSeconds.startsWith("+") && state.sessionBestLiveDeltaSeconds != "+0.000") deltaColor = TFT_RED;
		drawDashboardValue(DashboardLayout::LEFT_X, DashboardLayout::DELTA_VALUE_Y,
			DashboardLayout::SIDE_WIDTH, DashboardLayout::DELTA_VALUE_H,
			state.sessionBestLiveDeltaSeconds, "gt3Delta", deltaColor, &fonts::FreeSans18pt7b,
			MC_DATUM, forceUpdate, 0.72f, 0.92f);

		// The gear datum is tied to the physical screen center, independent of side columns.
		drawCenteredGear(state.gear, forceUpdate, gearSprite, gearSpriteCreated);
		drawGearShiftArcs(state.gear, forceUpdate);
		drawTyreTemperatureStrip(DashboardLayout::TYRE_X, DashboardLayout::TYRE_Y,
			DashboardLayout::TYRE_W, DashboardLayout::TYRE_H,
			state.gear, state.tyreTemperatures, forceUpdate, 1);
		drawPedalProgress(DashboardLayout::BRAKE_BAR_X, DashboardLayout::PEDAL_BAR_Y,
			DashboardLayout::PEDAL_BAR_W, DashboardLayout::PEDAL_BAR_H,
			state.absLevel == "--" ? -1 : state.absLevel.toInt(), state.absFilteredLevel.toInt(), "gt3BrakeBar",
			tft.color565(218, 220, 224), forceUpdate);
		drawPedalProgress(DashboardLayout::THROTTLE_BAR_X, DashboardLayout::PEDAL_BAR_Y,
			DashboardLayout::PEDAL_BAR_W, DashboardLayout::PEDAL_BAR_H,
			state.tcLevel == "--" ? -1 : state.tcLevel.toInt(), state.tcFilteredLevel.toInt(), "gt3ThrottleBar",
			tft.color565(218, 220, 224), forceUpdate);

		drawDashboardValue(DashboardLayout::RIGHT_TEXT_X, DashboardLayout::LAP_VALUE_Y[0],
			DashboardLayout::SIDE_WIDTH - DashboardLayout::INNER_PADDING, DashboardLayout::LAP_VALUE_H,
			state.bestLapTime, "gt3Best", TFT_WHITE, &fonts::FreeSans12pt7b,
			MC_DATUM, forceUpdate, 0.72f, 0.90f);
		drawDashboardValue(DashboardLayout::RIGHT_TEXT_X, DashboardLayout::LAP_VALUE_Y[1],
			DashboardLayout::SIDE_WIDTH - DashboardLayout::INNER_PADDING, DashboardLayout::LAP_VALUE_H,
			state.lastLapTime, "gt3Last", TFT_WHITE, &fonts::FreeSans12pt7b,
			MC_DATUM, forceUpdate, 0.72f, 0.90f);
		drawDashboardValueBuffered(DashboardLayout::RIGHT_TEXT_X, DashboardLayout::LAP_VALUE_Y[2],
			DashboardLayout::SIDE_WIDTH - DashboardLayout::INNER_PADDING, DashboardLayout::LAP_VALUE_H,
			state.currentLapTime, "gt3Current",
			state.lapInvalidated == "True" ? TFT_RED : TFT_WHITE, &fonts::FreeSans12pt7b,
			MC_DATUM, forceUpdate, currentLapSprite, currentLapSpriteCreated, 0.72f, 0.90f);

		const bool absOn = isActiveValue(state.absActive);
		const bool tcsOn = isActiveValue(state.tcActive);
		uint16_t leftColor = TFT_WHITE;
		if (state.tyrePressureFrontLeft != "--" && state.tyrePressureFrontLeft.toFloat() <= 1.0f)
			leftColor = TFT_RED;
		String lapCounterDisplay = state.tyrePressureRearLeft;
		lapCounterDisplay.replace("/", " / ");
		drawFuelIndicator(DashboardLayout::BOTTOM_X[0] + 4, DashboardLayout::BOTTOM_VALUE_Y,
			DashboardLayout::BOTTOM_W[0] - 8, DashboardLayout::BOTTOM_VALUE_H,
			state.fuelDisplayValue, state.fuelProgressPercent,
			state.fuelValueValid, state.fuelIsEV, forceUpdate);
		drawDashboardValue(DashboardLayout::BOTTOM_X[1] + 2, DashboardLayout::BOTTOM_VALUE_Y,
			DashboardLayout::BOTTOM_W[1] - 4, DashboardLayout::BOTTOM_VALUE_H,
			state.tyrePressureFrontLeft, "gt3Left", leftColor, &fonts::FreeSans12pt7b,
			MC_DATUM, forceUpdate, 0.82f, 0.80f);
		drawDashboardValue(DashboardLayout::BOTTOM_X[2] + 2, DashboardLayout::BOTTOM_VALUE_Y,
			DashboardLayout::BOTTOM_W[2] - 4, DashboardLayout::BOTTOM_VALUE_H,
			lapCounterDisplay, "gt3Lap", TFT_WHITE, &fonts::FreeSans12pt7b,
			MC_DATUM, forceUpdate, 0.56f, 0.74f);
		drawDashboardValue(DashboardLayout::BOTTOM_X[3] + 2, DashboardLayout::BOTTOM_VALUE_Y,
			DashboardLayout::BOTTOM_W[3] - 4, DashboardLayout::BOTTOM_VALUE_H,
			state.tyrePressureFrontRight, "gt3Pos", tft.color565(160, 60, 255), &fonts::FreeSans12pt7b,
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

	void drawGearShiftArcs(const String &displayGear, bool forceUpdate)
	{
		const String id = "gt3GearArcs";
		if (!forceUpdate && prevData[id] == displayGear) return;

		const int gearNumber = displayGear.toInt();
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

		prevData[id] = displayGear;
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
		releaseSpriteAfterThemePreview(sprite, spriteCreated);
	}

	void drawCenteredGear(const String &displayGear, bool forceUpdate,
		LGFX_Sprite &sprite, bool &spriteCreated)
	{
		const String id = "gt3Gear";
		if (!forceUpdate && prevData[id] == displayGear) return;

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
				width, height, displayGear, id, TFT_WHITE, &fonts::DejaVu72,
				MC_DATUM, forceUpdate, 1.25f, 1.25f);
			return;
		}

		sprite.setTextPadding(0);
		sprite.setTextColor(TFT_WHITE, TFT_BLACK);
		sprite.setTextSize(1.25f, 1.25f);
		sprite.setFont(&fonts::DejaVu72);
		sprite.setTextDatum(TL_DATUM);

		const int initialX = (width - sprite.textWidth(displayGear)) / 2;
		const int initialY = (height - sprite.fontHeight()) / 2;
		sprite.fillSprite(TFT_BLACK);
		sprite.drawString(displayGear, initialX, initialY);

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
		sprite.drawString(displayGear, correctedX, correctedY);
		sprite.pushSprite(DashboardLayout::GEAR_VALUE_X, DashboardLayout::GEAR_VALUE_Y);
		sprite.setFont(&fonts::Font0);
		sprite.setTextSize(1.0f);
		prevData[id] = displayGear;
		releaseSpriteAfterThemePreview(sprite, spriteCreated);
	}

	void drawFuelIndicator(int32_t x, int32_t y, int32_t width, int32_t height,
		const String &value, int progressPercent, bool valueValid, bool isEV,
		bool forceUpdate)
	{
		const String id = "gt3Fuel";
		uint16_t color = TFT_WHITE;
		if (valueValid && !isEV)
		{
			const float fuelPercent = progressPercent;
			if (fuelPercent <= 9.0f) color = TFT_RED;
			else if (fuelPercent <= 24.0f) color = TFT_YELLOW;
		}
		const String state = value + ":" + String(progressPercent) + ":" +
			String(valueValid ? 1 : 0) + ":" + String(isEV ? 1 : 0) + ":" + String(color);
		if (!forceUpdate && prevData[id] == state) return;

		tft.fillRect(x, y, width, height, TFT_BLACK);
		const int32_t pumpX = x + 4;
		const int32_t pumpHeight = 18;
		const int32_t pumpY = y + (height - pumpHeight) / 2 - 2;

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
        if (inputPercent < 0) {
            if (forceUpdate || prevData[id] != "--") {
                tft.fillRect(x, y, width, height, TFT_BLACK);
                tft.drawRoundRect(x, y, width, height, 3, TFT_DARKGREY);
                tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK); tft.setTextDatum(MC_DATUM);
                tft.drawString("--", x + width / 2, y + height / 2, 1);
                prevData[id] = "--";
            }
            return;
        }
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
			pedalSpriteCreated = pedalSprite.createSprite(
				DashboardLayout::PEDAL_BAR_W,
				DashboardLayout::PEDAL_BAR_H) != nullptr;
		}
		if (!pedalSpriteCreated)
		{
			tft.fillRect(x, y, width, height, TFT_BLACK);
			if (inputFill > 0)
				tft.fillRect(x + 1, y + 1 + innerHeight - inputFill,
					innerWidth, inputFill, inputColor);
			if (appliedFill > 0)
				tft.fillRect(x + 1, y + 1 + innerHeight - appliedFill,
					innerWidth, appliedFill, appliedColor);
			tft.drawRoundRect(x, y, width, height,
				DashboardLayout::TYRE_CORNER_RADIUS, tft.color565(82, 86, 92));
			prevData[id] = state;
			return;
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
		releaseSpriteAfterThemePreview(pedalSprite, pedalSpriteCreated);

		prevData[id] = state;
	}

	void drawRpmMeterGT3(DashboardState &state, bool forceUpdate)
	{
		// Shift-warning tuning: alert onset starts at peak brightness.
		static constexpr float RPM_SHIFT_PULSE_FREQUENCY_HZ = 3.0f;
		static constexpr float RPM_SHIFT_PULSE_MIN_WHITE_MIX = 0.25f;
		static constexpr float RPM_SHIFT_PULSE_NEAR_MAX_WHITE_MIX = 0.45f;
		static constexpr float RPM_SHIFT_PULSE_REV_ALERT_WHITE_MIX = 0.60f;
		static constexpr uint32_t RPM_SHIFT_PULSE_FRAME_INTERVAL_MS = 16;

		const int segmentCount = 36;
		const int segmentPitch = 8;
		const int active = constrain((state.rpmPercent * segmentCount + 99) / 100, 0, segmentCount);
		const int previous = forceUpdate ? -1 : constrain((state.prev_rpmPercent * segmentCount + 99) / 100, 0, segmentCount);
		const String gradientState = String(state.rpmRedLineSetting) + ":" +
			String(state.rpmAlertRangeValid ? 1 : 0);
		const bool redlineChanged = prevData["gt3Redline"] != gradientState;
		const int displayedRpm = ((state.engineRpm + 25) / 50) * 50;
		const String rpmValueText = state.engineRpm < 0 ? "--" : String(displayedRpm);
		const uint32_t now = millis();
		const bool rpmWarningActive = state.rpmAlertRangeValid &&
			state.rpmPercent >= state.rpmRedLineSetting;
		const bool pulseActive = rpmWarningActive || state.revLimitAlertActive;
		const bool revAlertStarted = state.revLimitAlertActive && !state.revLimitAlertWasActive;
		const bool pulseStarted = (pulseActive && !state.rpmPulseWasActive) || revAlertStarted;
		const bool pulseEnded = !pulseActive && state.rpmPulseWasActive;
		const String rpmTextState = rpmValueText + ":" + String(pulseActive ? 1 : 0);
		const bool rpmTextChanged = forceUpdate ||
			prevData["gt3RpmValue"] != rpmTextState;
		if (pulseStarted) state.rpmPulseStartTime = now;

		float pulsePeakWhiteMix = RPM_SHIFT_PULSE_MIN_WHITE_MIX;
		if (rpmWarningActive)
		{
			const float warningSpan = max(1, 100 - state.rpmRedLineSetting);
			const float warningProgress = constrain(
				(state.rpmPercent - state.rpmRedLineSetting) / warningSpan, 0.0f, 1.0f);
			pulsePeakWhiteMix +=
				(RPM_SHIFT_PULSE_NEAR_MAX_WHITE_MIX - RPM_SHIFT_PULSE_MIN_WHITE_MIX) *
				warningProgress;
		}
		if (state.revLimitAlertActive)
			pulsePeakWhiteMix = RPM_SHIFT_PULSE_REV_ALERT_WHITE_MIX;

		uint8_t pulseWhiteMix = 0;
		bool pulseFrameChanged = false;
		if (pulseActive &&
			(pulseStarted || now - state.lastRpmPulseFrameTime >= RPM_SHIFT_PULSE_FRAME_INTERVAL_MS))
		{
			const float elapsedSeconds = (now - state.rpmPulseStartTime) / 1000.0f;
			const float pulse = 0.5f + 0.5f * cosf(
				TWO_PI * RPM_SHIFT_PULSE_FREQUENCY_HZ * elapsedSeconds);
			pulseWhiteMix = static_cast<uint8_t>(lroundf(
				255.0f * pulsePeakWhiteMix * pulse));
			pulseFrameChanged = pulseStarted || pulseWhiteMix != state.lastRpmPulseWhiteMix;
			state.lastRpmPulseFrameTime = now;
		}
		else if (pulseActive)
		{
			pulseWhiteMix = state.lastRpmPulseWhiteMix;
		}

		if (!forceUpdate && active == previous && !redlineChanged &&
			!pulseFrameChanged && !pulseEnded && !rpmTextChanged) return;
		const float minAlertPosition = state.rpmAlertRangeValid
			? constrain(state.rpmRedLineSetting / 100.0f, 0.30f, 0.95f)
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
		const int minAlertMarkerX = state.rpmAlertRangeValid
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
		if (state.rpmAlertRangeValid)
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
		state.prev_rpmPercent = state.rpmPercent;
		prevData["gt3Redline"] = gradientState;
		prevData["gt3MinAlertMarkerX"] = String(minAlertMarkerX);
		state.revLimitAlertWasActive = state.revLimitAlertActive;
		state.rpmPulseWasActive = pulseActive;
		state.lastRpmPulseWhiteMix = pulseActive ? pulseWhiteMix : 0;
	}
