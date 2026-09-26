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
		releaseSpriteAfterThemePreview(sprite, spriteCreated);
	}

	void drawRadarPedals(const DashboardState &state, bool forceUpdate)
	{
		const int brakeInput = constrain(state.absLevel.toInt(), 0, 100);
		const int brakeApplied = constrain(state.absFilteredLevel.toInt(), 0, 100);
		const int throttleInput = constrain(state.tcLevel.toInt(), 0, 100);
		const int throttleApplied = constrain(state.tcFilteredLevel.toInt(), 0, 100);
		const String cacheState = state.absLevel + ":" + state.tcLevel + ":" + String(brakeInput) + ":" + String(brakeApplied) +
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
            if ((row == 0 ? state.absLevel : state.tcLevel) == "--") pedalSprite.drawString("--", 25, y, 1);
			const int inputWidth = 43 * values[row][0] / 100;
			const int appliedWidth = 43 * values[row][1] / 100;
			if (inputWidth > 0)
				pedalSprite.fillRect(11, y + 1, inputWidth, 5, input);
			if (appliedWidth > 0)
				pedalSprite.fillRect(11, y + 1, appliedWidth, 5, applied);
		}
		pedalSprite.pushSprite(258, 171);
		prevData["radarPedals"] = cacheState;
		releaseSpriteAfterThemePreview(pedalSprite, spriteCreated);
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
		releaseSpriteAfterThemePreview(sprite, spriteCreated);
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
			String(state.rpmAlertRangeValid ? 1 : 0) + ":" + state.fuelLabel;
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
			tft.drawString(state.fuelLabel, 260, 25, 1);
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
		drawRadarValue(257, 36, 57, 24, state.fuelDisplayValue,
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
