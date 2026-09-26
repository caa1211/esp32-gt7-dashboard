	/*
	THESIS: A glance-first endurance display that refuses the usual equal-column telemetry grid.
	OWN-WORLD: Matte black, a charcoal gear tower, white speed slab, and cyan, magenta, yellow,
	green, and red instrument outlines.
	STORY: Read the shift state first, then gear and speed, then lap pace, tyres, pedals, and fuel.
	FIRST VIEWPORT: Shift lamps span the top; race context and tyres sit left, the drive state
	anchors the center, lap timing owns the right, and strategy status forms the bottom rail.
	FORM: Compact pit-wall telemetry plate adapted to the physical 320 x 240 display.
	*/

	void drawEnduranceValue(
		int32_t x, int32_t y, int32_t width, int32_t height,
		const String &value, const String &id, uint16_t foreground,
		uint16_t background, const lgfx::IFont *font, textdatum_t datum,
		bool forceUpdate, float scaleX = 1.0f, float scaleY = 1.0f)
	{
		const String cacheState = value + ":" + String(foreground) + ":" +
			String(background);
		if (!forceUpdate && prevData[id] == cacheState) return;

		tft.fillRect(x, y, width, height, background);
		tft.setTextPadding(0);
		tft.setTextColor(foreground, background);
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
		prevData[id] = cacheState;
	}

	void drawEnduranceValueBuffered(
		int32_t x, int32_t y, int32_t width, int32_t height,
		const String &value, const String &id, uint16_t foreground,
		uint16_t background, const lgfx::IFont *font, textdatum_t datum,
		bool forceUpdate, LGFX_Sprite &sprite, bool &spriteCreated,
		float scaleX = 1.0f, float scaleY = 1.0f)
	{
		const String cacheState = value + ":" + String(foreground) + ":" +
			String(background);
		if (!forceUpdate && prevData[id] == cacheState) return;

		if (!spriteCreated)
		{
			sprite.setColorDepth(16);
			spriteCreated = sprite.createSprite(width, height) != nullptr;
		}
		if (!spriteCreated)
		{
			drawEnduranceValue(x, y, width, height, value, id, foreground,
				background, font, datum, true, scaleX, scaleY);
			return;
		}

		sprite.fillSprite(background);
		sprite.setTextPadding(0);
		sprite.setTextColor(foreground, background);
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
		prevData[id] = cacheState;
		releaseSpriteAfterThemePreview(sprite, spriteCreated);
	}

	void drawEnduranceShiftLights(const DashboardState &state, bool forceUpdate)
	{
		static constexpr int LIGHT_COUNT = 14;
		static constexpr uint32_t FRAME_INTERVAL_MS = 32;
		static uint32_t pulseStartedAt = 0;
		static uint32_t lastFrameAt = 0;
		static uint8_t previousWhiteMix = 0;
		static bool previousWarning = false;

		const int activeLights = constrain(
			(state.rpmPercent * LIGHT_COUNT + 99) / 100, 0, LIGHT_COUNT);
		const bool warning = state.revLimitAlertActive ||
			(state.rpmAlertRangeValid &&
				state.rpmPercent >= state.rpmRedLineSetting);
		const uint32_t now = millis();
		if (warning && !previousWarning) pulseStartedAt = now;

		uint8_t whiteMix = previousWhiteMix;
		if (!warning)
		{
			whiteMix = 0;
		}
		else if (forceUpdate || !previousWarning ||
			now - lastFrameAt >= FRAME_INTERVAL_MS)
		{
			const float elapsed = (now - pulseStartedAt) / 1000.0f;
			const float wave = 0.5f + 0.5f * cosf(TWO_PI * 3.0f * elapsed);
			const float peak = state.revLimitAlertActive ? 0.72f : 0.48f;
			whiteMix = static_cast<uint8_t>(lroundf(255.0f * peak * wave));
			lastFrameAt = now;
		}

		const int alertLight = state.rpmAlertRangeValid
			? constrain(
				(state.rpmRedLineSetting * (LIGHT_COUNT - 1) + 50) / 100,
				0, LIGHT_COUNT - 1)
			: -1;
		const String cacheState = String(activeLights) + ":" +
			String(alertLight) + ":" + String(whiteMix) + ":" +
			String(warning ? 1 : 0);
		if (!forceUpdate && prevData["enduranceShiftLights"] == cacheState)
		{
			previousWarning = warning;
			return;
		}

		const uint16_t green = tft.color565(0, 226, 115);
		const uint16_t yellow = tft.color565(255, 225, 0);
		const uint16_t red = tft.color565(255, 43, 50);
		const uint16_t inactive = tft.color565(24, 27, 27);
		for (int i = 0; i < LIGHT_COUNT; ++i)
		{
			const int x = 76 + i * 13;
			const uint16_t zoneColor = i < 5 ? green : (i < 10 ? yellow : red);
			const uint16_t color = i < activeLights
				? (warning ? blendRgb565(zoneColor, TFT_WHITE, whiteMix) : zoneColor)
				: blendRgb565(zoneColor, inactive, 218);
			tft.fillRect(x - 6, 4, 13, 14, TFT_BLACK);
			tft.drawCircle(x, 11, 5,
				i < activeLights ? zoneColor : tft.color565(45, 48, 48));
			tft.fillCircle(x, 11, 4, color);
			if (i == alertLight)
				tft.drawCircle(x, 11, 6, tft.color565(255, 244, 178));
		}

		prevData["enduranceShiftLights"] = cacheState;
		previousWhiteMix = whiteMix;
		previousWarning = warning;
	}

	void drawEnduranceTyre(
		int index, int32_t valueX, int32_t valueY,
		int32_t tyreX, int32_t tyreY, float temperature,
		bool forceUpdate)
	{
		const bool valid = isfinite(temperature) && temperature > 0.0f;
		const String value = valid ? String(lroundf(temperature)) : "--";
		const uint16_t color = valid
			? tyreTemperatureColor(temperature)
			: tft.color565(44, 47, 48);
		const String id = "enduranceTyre" + String(index);
		const String cacheState = value + ":" + String(color);
		if (!forceUpdate && prevData[id] == cacheState) return;

		tft.fillRoundRect(tyreX, tyreY, 11, 22, 3, color);
		tft.drawFastVLine(tyreX + 5, tyreY + 2, 18,
			blendRgb565(color, TFT_BLACK, 126));

		static LGFX_Sprite valueSprite(&tft);
		static bool spriteCreated = false;
		if (!spriteCreated)
		{
			valueSprite.setColorDepth(16);
			spriteCreated = valueSprite.createSprite(24, 16) != nullptr;
		}
		if (spriteCreated)
		{
			valueSprite.fillSprite(TFT_BLACK);
			valueSprite.setTextColor(valid ? TFT_WHITE : tft.color565(90, 94, 95),
				TFT_BLACK);
			valueSprite.setTextDatum(MC_DATUM);
			valueSprite.setTextSize(1.0f);
			valueSprite.drawString(value, 12, 8, 2);
			valueSprite.pushSprite(valueX, valueY);
			releaseSpriteAfterThemePreview(valueSprite, spriteCreated);
		}
		else
		{
			tft.fillRect(valueX, valueY, 24, 16, TFT_BLACK);
			tft.setTextColor(valid ? TFT_WHITE : tft.color565(90, 94, 95),
				TFT_BLACK);
			tft.setTextDatum(MC_DATUM);
			tft.drawString(value, valueX + 12, valueY + 8, 2);
			tft.setTextDatum(TL_DATUM);
		}
		prevData[id] = cacheState;
	}

	void drawEnduranceTyres(const DashboardState &state, bool forceUpdate)
	{
		if (forceUpdate)
		{
			const uint16_t line = tft.color565(226, 229, 229);
			tft.drawRoundRect(4, 62, 88, 84, 4, line);
			tft.fillRect(28, 60, 40, 9, TFT_BLACK);
			tft.setTextColor(line, TFT_BLACK);
			tft.setTextDatum(MC_DATUM);
			tft.setTextSize(1.0f);
			tft.drawString("TYRES", 48, 64, 1);
			tft.setTextDatum(TL_DATUM);
		}

		drawEnduranceTyre(0, 6, 78, 32, 75,
			state.tyreTemperatures[0], forceUpdate);
		drawEnduranceTyre(1, 63, 78, 50, 75,
			state.tyreTemperatures[1], forceUpdate);
		drawEnduranceTyre(2, 6, 111, 32, 108,
			state.tyreTemperatures[2], forceUpdate);
		drawEnduranceTyre(3, 63, 111, 50, 108,
			state.tyreTemperatures[3], forceUpdate);
	}

	void drawEndurancePedals(const DashboardState &state, bool forceUpdate)
	{
		const int brakeInput = constrain(state.absLevel.toInt(), 0, 100);
		const int brakeApplied = constrain(state.absFilteredLevel.toInt(), 0, 100);
		const int throttleInput = constrain(state.tcLevel.toInt(), 0, 100);
		const int throttleApplied = constrain(state.tcFilteredLevel.toInt(), 0, 100);
		const String cacheState = state.absLevel + ":" + state.tcLevel + ":" + String(brakeInput) + ":" +
			String(brakeApplied) + ":" + String(throttleInput) + ":" +
			String(throttleApplied);
		if (!forceUpdate && prevData["endurancePedals"] == cacheState) return;

		const uint16_t colors[2] = {
			tft.color565(255, 43, 50),
			tft.color565(0, 226, 115)};
		const uint16_t dimColors[2] = {
			tft.color565(110, 28, 32),
			tft.color565(0, 88, 48)};
		const int input[2] = {brakeInput, throttleInput};
		const int applied[2] = {brakeApplied, throttleApplied};
		const char *labels[2] = {"BRK", "THR"};

		static LGFX_Sprite pedalSprite(&tft);
		static bool spriteCreated = false;
		if (!spriteCreated)
		{
			pedalSprite.setColorDepth(16);
			spriteCreated = pedalSprite.createSprite(88, 28) != nullptr;
		}
		if (spriteCreated)
		{
			pedalSprite.fillSprite(TFT_BLACK);
			for (int i = 0; i < 2; ++i)
			{
				const int x = i * 46;
				pedalSprite.drawRoundRect(x, 0, 42, 28, 4, colors[i]);
				pedalSprite.setTextColor(colors[i], TFT_BLACK);
				pedalSprite.setTextDatum(TC_DATUM);
				pedalSprite.drawString(labels[i], x + 21, 0, 1);
				pedalSprite.setTextColor(TFT_WHITE, TFT_BLACK);
				pedalSprite.setTextDatum(MC_DATUM);
				pedalSprite.drawString(((i == 0 ? state.absLevel : state.tcLevel) == "--" ? String("--") : String(input[i])), x + 21, 16, 2);
				pedalSprite.fillRect(x + 4, 24, 34, 2, tft.color565(38, 40, 40));
				pedalSprite.fillRect(x + 4, 24,
					34 * input[i] / 100, 2, dimColors[i]);
				pedalSprite.fillRect(x + 4, 26,
					34 * applied[i] / 100, 2, colors[i]);
			}
			pedalSprite.pushSprite(4, 150);
			releaseSpriteAfterThemePreview(pedalSprite, spriteCreated);
		}
		else
		{
			tft.fillRect(4, 150, 88, 28, TFT_BLACK);
			for (int i = 0; i < 2; ++i)
			{
				const int x = 4 + i * 46;
				tft.drawRoundRect(x, 150, 42, 28, 4, colors[i]);
				tft.setTextColor(colors[i], TFT_BLACK);
				tft.setTextDatum(TC_DATUM);
				tft.drawString(labels[i], x + 21, 150, 1);
				tft.setTextColor(TFT_WHITE, TFT_BLACK);
				tft.setTextDatum(MC_DATUM);
				tft.drawString(((i == 0 ? state.absLevel : state.tcLevel) == "--" ? String("--") : String(input[i])), x + 21, 166, 2);
				tft.fillRect(x + 4, 174, 34, 2, tft.color565(38, 40, 40));
				tft.fillRect(x + 4, 174, 34 * input[i] / 100, 2, dimColors[i]);
				tft.fillRect(x + 4, 176, 34 * applied[i] / 100, 2, colors[i]);
			}
			tft.setTextDatum(TL_DATUM);
		}
		prevData["endurancePedals"] = cacheState;
	}

	void drawEnduranceFuel(const DashboardState &state, bool forceUpdate)
	{
		const String cacheState = state.fuelLabel + ":" +
			state.fuelDisplayValue + ":" + String(state.fuelProgressPercent) +
			":" + String(state.fuelValueValid ? 1 : 0) + ":" +
			String(state.fuelIsEV ? 1 : 0) + ":" +
			state.tyrePressureFrontLeft;
		if (!forceUpdate && prevData["enduranceFuel"] == cacheState) return;

		uint16_t color = tft.color565(255, 225, 0);
		if (!state.fuelValueValid)
			color = tft.color565(95, 97, 82);
		else if (!state.fuelIsEV && state.fuelProgressPercent <= 10)
			color = tft.color565(255, 43, 50);
		else if (!state.fuelIsEV && state.fuelProgressPercent <= 25)
			color = tft.color565(255, 136, 0);

		tft.fillRect(4, 182, 118, 54, TFT_BLACK);
		tft.drawRoundRect(4, 182, 118, 54, 4, color);
		tft.fillRect(13, 180, 44, 9, TFT_BLACK);
		tft.setTextSize(1.0f);
		tft.setTextDatum(MC_DATUM);
		tft.setTextColor(color, TFT_BLACK);
		tft.drawString(state.fuelLabel, 35, 184, 1);

		const String fuelValue = state.fuelValueValid
			? state.fuelDisplayValue
			: "--";
		tft.setTextColor(TFT_WHITE, TFT_BLACK);
		tft.setTextSize(1.08f, 1.0f);
		tft.drawString(fuelValue, 34, 207, 2);
		tft.setTextSize(1.0f);
		tft.setTextColor(tft.color565(155, 158, 157), TFT_BLACK);
		tft.drawString(state.fuelIsEV ? "kWh" : "%", 59, 210, 1);

		tft.setTextColor(TFT_WHITE, TFT_BLACK);
		tft.drawString(state.tyrePressureFrontLeft, 94, 204, 2);
		tft.setTextColor(tft.color565(155, 158, 157), TFT_BLACK);
		tft.drawString("LAPS", 94, 219, 1);

		const int progress = state.fuelValueValid
			? constrain(state.fuelProgressPercent, 0, 100)
			: 0;
		tft.fillRoundRect(10, 228, 106, 4, 2, tft.color565(39, 41, 39));
		if (progress > 0)
			tft.fillRoundRect(10, 228, 106 * progress / 100, 4, 2, color);
		tft.setTextDatum(TL_DATUM);
		prevData["enduranceFuel"] = cacheState;
	}

	void drawEnduranceAid(
		int32_t x, const String &label, bool active,
		const String &id, uint16_t activeColor, bool forceUpdate)
	{
		const String cacheState = active ? "1" : "0";
		if (!forceUpdate && prevData[id] == cacheState) return;

		const uint16_t inactive = tft.color565(70, 73, 73);
		const uint16_t color = active ? activeColor : inactive;
		tft.fillRect(x, 182, 44, 54, TFT_BLACK);
		tft.drawRoundRect(x, 182, 44, 54, 4, color);
		tft.setTextSize(1.0f);
		tft.setTextDatum(MC_DATUM);
		tft.setTextColor(color, TFT_BLACK);
		tft.drawString(label, x + 22, 190, 1);
		if (active)
		{
			tft.fillRoundRect(x + 8, 202, 28, 23, 3, activeColor);
			tft.setTextColor(TFT_BLACK, activeColor);
			tft.drawString("ON", x + 22, 213, 2);
		}
		else
		{
			tft.drawRoundRect(x + 8, 202, 28, 23, 3, inactive);
			tft.setTextColor(tft.color565(105, 108, 108), TFT_BLACK);
			tft.drawString("OFF", x + 22, 213, 1);
		}
		tft.setTextDatum(TL_DATUM);
		prevData[id] = cacheState;
	}

	void drawEnduranceDashboard(const DashboardState &state, bool forceUpdate)
	{
		static LGFX_Sprite speedSprite(&tft);
		static LGFX_Sprite rpmSprite(&tft);
		static LGFX_Sprite currentLapSprite(&tft);
		static LGFX_Sprite deltaSprite(&tft);
		static bool speedSpriteCreated = false;
		static bool rpmSpriteCreated = false;
		static bool currentLapSpriteCreated = false;
		static bool deltaSpriteCreated = false;

		const uint16_t panel = tft.color565(23, 25, 24);
		const uint16_t line = tft.color565(70, 74, 73);
		const uint16_t muted = tft.color565(145, 149, 148);
		const uint16_t cyan = tft.color565(0, 207, 255);
		const uint16_t magenta = tft.color565(211, 21, 214);
		const uint16_t green = tft.color565(0, 226, 115);
		const uint16_t red = tft.color565(255, 43, 50);
		const String layoutState = state.fuelLabel + ":" +
			String(state.fuelIsEV ? 1 : 0);
		const bool redraw = forceUpdate ||
			prevData["enduranceLayout"] != layoutState;

		if (redraw)
		{
			tft.fillScreen(TFT_BLACK);
			tft.setTextPadding(0);
			tft.setTextSize(1.0f);
			tft.setTextDatum(MC_DATUM);

			tft.drawRoundRect(4, 23, 42, 34, 4, cyan);
			tft.drawRoundRect(50, 23, 42, 34, 4, magenta);
			tft.setTextColor(cyan, TFT_BLACK);
			tft.drawString("GRID", 25, 29, 1);
			tft.setTextColor(magenta, TFT_BLACK);
			tft.drawString("LAP", 71, 29, 1);

			tft.fillRoundRect(97, 23, 95, 104, 4, panel);
			tft.drawRoundRect(97, 23, 95, 104, 4, line);
			tft.setTextColor(muted, panel);
			tft.drawString("GEAR", 144, 30, 1);

			tft.fillRoundRect(97, 131, 95, 27, 4, TFT_WHITE);
			tft.setTextColor(tft.color565(48, 50, 49), TFT_WHITE);
			tft.drawString("KM/H", 177, 145, 1);
			tft.drawRoundRect(97, 162, 95, 16, 3, line);

			tft.drawRoundRect(197, 23, 119, 155, 4, cyan);
			tft.fillRect(219, 20, 75, 9, TFT_BLACK);
			tft.setTextColor(TFT_WHITE, TFT_BLACK);
			tft.drawString("LAP TIMES", 256, 24, 1);
			tft.drawFastHLine(202, 78, 109, tft.color565(0, 83, 104));
			tft.drawFastHLine(202, 122, 109, tft.color565(0, 83, 104));
			tft.setTextColor(muted, TFT_BLACK);
			tft.setTextDatum(MR_DATUM);
			tft.drawString("CURRENT", 310, 70, 1);
			tft.drawString("LAST", 310, 114, 1);
			tft.drawString("BEST", 310, 158, 1);
			tft.setTextDatum(TL_DATUM);
		}

		drawEnduranceShiftLights(state, redraw);
		drawEnduranceValue(7, 34, 36, 20, state.tyrePressureFrontRight,
			"endurancePosition", TFT_WHITE, TFT_BLACK,
			&fonts::FreeSans12pt7b, MC_DATUM, redraw, 0.82f, 0.84f);
		String lapDisplay = state.tyrePressureRearLeft;
		const int lapSeparator = lapDisplay.indexOf('/');
		if (lapSeparator > 0)
			lapDisplay = lapDisplay.substring(0, lapSeparator);
		drawEnduranceValue(53, 34, 36, 20, lapDisplay,
			"enduranceLap", TFT_WHITE, TFT_BLACK,
			&fonts::FreeSans12pt7b, MC_DATUM, redraw, 0.82f, 0.84f);

		drawEnduranceTyres(state, redraw);
		drawEndurancePedals(state, redraw);

		drawEnduranceValue(99, 36, 91, 88, state.gear,
			"enduranceGear", TFT_WHITE, panel,
			&fonts::DejaVu72, MC_DATUM, redraw, 1.0f, 1.0f);
		drawEnduranceValueBuffered(101, 133, 61, 23, state.speed,
			"enduranceSpeed", TFT_BLACK, TFT_WHITE,
			&fonts::FreeSans18pt7b, MC_DATUM, redraw,
			speedSprite, speedSpriteCreated, 0.80f, 0.78f);
		const int displayedRpm = ((state.engineRpm + 25) / 50) * 50;
		const uint16_t rpmColor = state.revLimitAlertActive ? red : muted;
		drawEnduranceValueBuffered(99, 163, 91, 14,
			(state.engineRpm < 0 ? String("--") : String(displayedRpm)) + " RPM", "enduranceRpm",
			rpmColor, TFT_BLACK, &fonts::FreeSans9pt7b,
			MC_DATUM, redraw, rpmSprite, rpmSpriteCreated, 0.61f, 0.62f);

		const uint16_t currentColor =
			state.lapInvalidated == "True" ? red : TFT_WHITE;
		drawEnduranceValueBuffered(201, 39, 111, 26,
			state.currentLapTime, "enduranceCurrent", currentColor, TFT_BLACK,
			&fonts::FreeSans12pt7b, MC_DATUM, redraw,
			currentLapSprite, currentLapSpriteCreated, 0.72f, 0.84f);
		drawEnduranceValue(201, 83, 111, 26,
			state.lastLapTime, "enduranceLast", TFT_WHITE, TFT_BLACK,
			&fonts::FreeSans12pt7b, MC_DATUM, redraw, 0.72f, 0.84f);
		drawEnduranceValue(201, 127, 111, 26,
			state.bestLapTime, "enduranceBest", TFT_WHITE, TFT_BLACK,
			&fonts::FreeSans12pt7b, MC_DATUM, redraw, 0.72f, 0.84f);

		drawEnduranceFuel(state, redraw);
		uint16_t deltaColor = TFT_WHITE;
		if (state.sessionBestLiveDeltaSeconds.startsWith("-")) deltaColor = green;
		else if (state.sessionBestLiveDeltaSeconds.startsWith("+") &&
			state.sessionBestLiveDeltaSeconds != "+0.000") deltaColor = red;
		if (redraw)
		{
			tft.drawRoundRect(127, 182, 91, 54, 4, line);
			tft.setTextColor(muted, TFT_BLACK);
			tft.setTextDatum(MC_DATUM);
			tft.drawString("LAST DELTA", 172, 190, 1);
			tft.setTextDatum(TL_DATUM);
		}
		drawEnduranceValueBuffered(130, 199, 85, 29,
			state.sessionBestLiveDeltaSeconds, "enduranceDelta",
			deltaColor, TFT_BLACK, &fonts::FreeSans12pt7b,
			MC_DATUM, redraw, deltaSprite, deltaSpriteCreated, 0.78f, 0.88f);
		drawEnduranceAid(223, "ABS", isActiveValue(state.absActive),
			"enduranceAbs", tft.color565(255, 211, 0), redraw);
		drawEnduranceAid(272, "TCS", isActiveValue(state.tcActive),
			"enduranceTcs", cyan, redraw);

		prevData["enduranceLayout"] = layoutState;
	}
