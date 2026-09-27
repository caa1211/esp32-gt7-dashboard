	void drawMonoValue(int32_t x, int32_t y, int32_t width, int32_t height,
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
		const int32_t textX = datum == MR_DATUM ? x + width - 2
			: (datum == MC_DATUM ? x + width / 2 : x + 2);
		tft.drawString(value, textX, y + height / 2, font);
		tft.setTextSize(1.0f);
		tft.setTextDatum(TL_DATUM);
		prevData[id] = cacheState;
	}

	void drawMonoGear(const String &gear, uint16_t color, bool forceUpdate)
	{
		const String cacheState = gear + ":" + String(color);
		if (!forceUpdate && prevData["monoGear"] == cacheState) return;

		static constexpr int32_t x = 106;
		static constexpr int32_t y = 56;
		static constexpr int32_t width = 108;
		static constexpr int32_t height = 80;
		tft.fillRect(x, y, width, height, TFT_BLACK);

		if (gear == "N")
		{
			// Font 7 only contains digits. Draw a matching heavy, angular N for neutral.
			static constexpr int32_t left = 134;
			static constexpr int32_t top = 63;
			static constexpr int32_t glyphWidth = 52;
			static constexpr int32_t glyphHeight = 66;
			static constexpr int32_t stroke = 8;
			tft.fillRect(left, top, stroke, glyphHeight, color);
			tft.fillRect(left + glyphWidth - stroke, top, stroke, glyphHeight, color);
			tft.fillTriangle(left + stroke, top,
				left + stroke, top + 12,
				left + glyphWidth - stroke, top + glyphHeight, color);
			tft.fillTriangle(left + stroke, top,
				left + glyphWidth - stroke, top + glyphHeight - 12,
				left + glyphWidth - stroke, top + glyphHeight, color);
		}
		else
		{
			tft.setTextColor(color, TFT_BLACK);
			tft.setTextDatum(MC_DATUM);
			tft.setTextSize(1.62f, 1.52f);
			tft.drawString(gear, x + width / 2, y + height / 2, 7);
			tft.setTextSize(1.0f);
			tft.setTextDatum(TL_DATUM);
		}

		prevData["monoGear"] = cacheState;
	}

	void drawMonoValueBuffered(int32_t x, int32_t y, int32_t width,
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
			drawMonoValue(x, y, width, height, value, id, color, font,
				datum, true, scaleX, scaleY);
			return;
		}
		sprite.fillSprite(TFT_BLACK);
		sprite.setTextColor(color, TFT_BLACK);
		sprite.setTextDatum(datum);
		sprite.setTextSize(scaleX, scaleY);
		const int32_t textX = datum == MR_DATUM ? width - 2
			: (datum == MC_DATUM ? width / 2 : 2);
		sprite.drawString(value, textX, height / 2, font);
		sprite.setTextSize(1.0f);
		sprite.pushSprite(x, y);
		prevData[id] = cacheState;
		releaseSpriteAfterThemePreview(sprite, spriteCreated);
	}

	void drawMonoSpeed(const String &speed, bool forceUpdate)
	{
		if (!forceUpdate && prevData["monoSpeed"] == speed) return;
		static LGFX_Sprite sprite(&tft);
		static bool spriteCreated = false;
		if (!spriteCreated)
		{
			sprite.setColorDepth(16);
			spriteCreated = sprite.createSprite(108, 48) != nullptr;
		}
		if (!spriteCreated) return;
		const uint16_t white = tft.color565(235, 235, 231);
		const uint16_t muted = tft.color565(154, 156, 158);
		sprite.fillSprite(TFT_BLACK);
		sprite.setTextDatum(MC_DATUM);
		sprite.setTextColor(white, TFT_BLACK);
		sprite.setTextSize(1.20f, 1.08f);
		sprite.drawString(speed, 54, 20, 4);
		sprite.setTextSize(1.0f);
		sprite.setTextColor(muted, TFT_BLACK);
		sprite.drawString("km/h", 54, 39, 1);
		sprite.pushSprite(106, 137);
		prevData["monoSpeed"] = speed;
		releaseSpriteAfterThemePreview(sprite, spriteCreated);
	}

	void drawMonoRpm(const DashboardState &state, bool forceUpdate)
	{
		static constexpr int SEGMENT_COUNT = 30;
		static constexpr int SEGMENT_X = 11;
		static constexpr int SEGMENT_Y = 17;
		static constexpr int SEGMENT_PITCH = 10;
		static constexpr int SEGMENT_W = 8;
		static constexpr int SEGMENT_H = 17;
		static constexpr float PULSE_HZ = 3.0f;
		static constexpr uint32_t FRAME_MS = 24;
		static int previousActive = -1;
		static bool previousPulse = false;
		static uint32_t pulseStart = 0;
		static uint32_t lastFrame = 0;
		static uint8_t previousMix = 0;

		const uint16_t activeBase = tft.color565(228, 228, 224);
		const uint16_t inactive = tft.color565(68, 70, 72);
		const uint16_t tick = tft.color565(150, 152, 154);
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
			float peak = 0.32f;
			if (warning)
			{
				const float span = max(1, 100 - state.rpmRedLineSetting);
				const float progress = constrain(
					(state.rpmPercent - state.rpmRedLineSetting) / span,
					0.0f, 1.0f);
				peak += 0.24f * progress;
			}
			if (state.revLimitAlertActive) peak = 0.72f;
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

		const String markerState = String(state.rpmRedLineSetting) + ":" +
			String(state.rpmAlertRangeValid ? 1 : 0);
		const bool markerChanged = prevData["monoRpmMarker"] != markerState;
		const bool pulseEnded = !pulse && previousPulse;
		if (forceUpdate || active != previousActive || markerChanged ||
			pulseFrame || pulseEnded)
		{
			for (int i = 0; i < SEGMENT_COUNT; ++i)
			{
				const bool fillChanged = previousActive < 0 ||
					(i >= min(active, previousActive) &&
					 i < max(active, previousActive));
				const bool pulseChanged = i < active && (pulseFrame || pulseEnded);
				if (!forceUpdate && !fillChanged && !pulseChanged) continue;
				const uint16_t color = i < active
					? (pulse ? blendRgb565(activeBase, TFT_WHITE, whiteMix)
						: activeBase)
					: inactive;
				tft.fillRect(SEGMENT_X + i * SEGMENT_PITCH, SEGMENT_Y,
					SEGMENT_W, SEGMENT_H, color);
			}

			if (forceUpdate)
			{
				for (int i = 0; i < 5; ++i)
				{
					const int x = SEGMENT_X + SEGMENT_W / 2 +
						lroundf(i * (SEGMENT_COUNT - 1) * SEGMENT_PITCH / 4.0f);
					tft.drawFastVLine(x, 6, 6, tick);
				}
			}

			if (forceUpdate || markerChanged)
			{
				tft.fillRect(SEGMENT_X - 4, 35,
					(SEGMENT_COUNT - 1) * SEGMENT_PITCH + SEGMENT_W + 8,
					9, TFT_BLACK);
				if (state.rpmAlertRangeValid)
				{
					const int markerX = SEGMENT_X + SEGMENT_W / 2 + lroundf(
						constrain(state.rpmRedLineSetting / 100.0f, 0.0f, 1.0f) *
						(SEGMENT_COUNT - 1) * SEGMENT_PITCH);
					tft.fillTriangle(markerX, 35, markerX - 4, 42,
						markerX + 4, 42, activeBase);
				}
			}
		}

		previousActive = active;
		previousPulse = pulse;
		previousMix = pulse ? whiteMix : 0;
		prevData["monoRpmMarker"] = markerState;
	}

	void drawMonoPedals(const DashboardState &state, bool forceUpdate)
	{
		const int brakeInput = constrain(state.absLevel.toInt(), 0, 100);
		const int brakeApplied = constrain(state.absFilteredLevel.toInt(), 0, 100);
		const int throttleInput = constrain(state.tcLevel.toInt(), 0, 100);
		const int throttleApplied = constrain(state.tcFilteredLevel.toInt(), 0, 100);
		const String cacheState = state.absLevel + ":" + state.tcLevel + ":" + String(brakeInput) + ":" + String(brakeApplied) +
			":" + String(throttleInput) + ":" + String(throttleApplied);
		if (!forceUpdate && prevData["monoPedals"] == cacheState) return;

		static LGFX_Sprite sprite(&tft);
		static bool spriteCreated = false;
		if (!spriteCreated)
		{
			sprite.setColorDepth(16);
			spriteCreated = sprite.createSprite(138, 20) != nullptr;
		}
		if (!spriteCreated) return;
		const uint16_t frame = tft.color565(104, 106, 108);
		const uint16_t input = tft.color565(94, 96, 98);
		const uint16_t applied = tft.color565(225, 225, 221);
		sprite.fillSprite(TFT_BLACK);
		sprite.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
		sprite.setTextDatum(TL_DATUM);
		sprite.drawString("B", 0, 1, 1);
		sprite.drawString("T", 0, 11, 1);
		const int values[2][2] = {
			{brakeInput, brakeApplied}, {throttleInput, throttleApplied}};
		for (int row = 0; row < 2; ++row)
		{
			const int y = 2 + row * 10;
			sprite.drawRect(12, y, 124, 7, frame);
            if ((row == 0 ? state.absLevel : state.tcLevel) == "--") sprite.drawString("--", 65, y, 1);
			const int inputW = 122 * values[row][0] / 100;
			const int appliedW = 122 * values[row][1] / 100;
			if (inputW > 0) sprite.fillRect(13, y + 1, inputW, 5, input);
			if (appliedW > 0) sprite.fillRect(13, y + 1, appliedW, 5, applied);
		}
		sprite.pushSprite(180, 218);
		prevData["monoPedals"] = cacheState;
		releaseSpriteAfterThemePreview(sprite, spriteCreated);
	}

	void drawMonoTyres(const DashboardState &state, bool forceUpdate,
		LGFX_Sprite &sprite, bool &spriteCreated)
	{
		String cacheState;
		for (int i = 0; i < 4; ++i)
		{
			if (i > 0) cacheState += ":";
			cacheState += isnan(state.tyreTemperatures[i])
				? "--" : String(lroundf(state.tyreTemperatures[i]));
		}
		if (!forceUpdate && prevData["monoTyres"] == cacheState) return;
		if (!spriteCreated)
		{
			sprite.setColorDepth(16);
			spriteCreated = sprite.createSprite(176, 20) != nullptr;
		}
		if (!spriteCreated) return;
		sprite.fillSprite(TFT_BLACK);
		sprite.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
		sprite.setTextDatum(ML_DATUM);
		sprite.drawString("TYRE", 2, 10, 1);
		const char *labels[] = {"FL", "FR", "RL", "RR"};
		for (int i = 0; i < 4; ++i)
		{
			const String value = String(labels[i]) +
				(isnan(state.tyreTemperatures[i])
					? String("--") : String(lroundf(state.tyreTemperatures[i])));
			sprite.setTextDatum(MC_DATUM);
			sprite.drawString(value, 54 + i * 33, 10, 1);
		}
		sprite.pushSprite(2, 218);
		prevData["monoTyres"] = cacheState;
		releaseSpriteAfterThemePreview(sprite, spriteCreated);
	}

	void drawMonoStatusDot(int centerX, int centerY, bool active,
		const String &id, uint16_t color, bool forceUpdate)
	{
		const String cacheState = active ? "1" : "0";
		if (!forceUpdate && prevData[id] == cacheState) return;
		tft.fillRect(centerX - 7, centerY - 5, 14, 10, TFT_BLACK);
		if (active)
			tft.fillRoundRect(centerX - 6, centerY - 4, 12, 8, 2, color);
		else
			tft.drawRoundRect(centerX - 6, centerY - 4, 12, 8, 2, color);
		prevData[id] = cacheState;
	}

	void drawMonoFuelGauge(int fuelProgressPercent, bool fuelValueValid, bool forceUpdate)
	{
		static constexpr int SEGMENTS = 10;
		const int percent = fuelValueValid ? constrain(fuelProgressPercent, 0, 100) : 100;
		const int active = constrain((percent * SEGMENTS + 99) / 100, 0, SEGMENTS);
		const String cacheState = String(active);
		if (!forceUpdate && prevData["monoFuelGauge"] == cacheState) return;
		const uint16_t filled = tft.color565(225, 225, 221);
		const uint16_t empty = tft.color565(68, 70, 72);
		const int x = 131;
		const int y = 199;
		for (int i = 0; i < SEGMENTS; ++i)
			tft.fillRect(x + i * 7, y, 5, 8, i < active ? filled : empty);
		prevData["monoFuelGauge"] = cacheState;
	}

	void drawMonoDashboard(const DashboardState &state, bool forceUpdate)
	{
		static LGFX_Sprite currentSprite(&tft);
		static LGFX_Sprite deltaSprite(&tft);
		static LGFX_Sprite tyreSprite(&tft);
		static bool currentSpriteCreated = false;
		static bool deltaSpriteCreated = false;
		static bool tyreSpriteCreated = false;
		const uint16_t white = tft.color565(235, 235, 231);
		const uint16_t muted = tft.color565(154, 156, 158);
		const uint16_t frame = tft.color565(76, 78, 80);
		static constexpr int32_t fuelLabelX = 220;
		static constexpr int32_t remLabelX = 274;
		const int32_t fuelValueX = fuelLabelX +
			static_cast<int32_t>(state.fuelLabel.length()) * 6 + 2;
		const int32_t remValueX = remLabelX + 3 * 6 + 2;

		if (forceUpdate)
		{
			tft.fillScreen(TFT_BLACK);
			tft.setTextPadding(0);
			tft.setTextSize(1.0f);
			tft.setTextDatum(TL_DATUM);
			tft.setTextColor(muted, TFT_BLACK);
			tft.drawFastVLine(102, 46, 141, frame);
			tft.drawFastVLine(218, 46, 141, frame);
			tft.drawFastHLine(5, 94, 91, frame);
			tft.drawFastHLine(224, 94, 91, frame);
			tft.drawFastHLine(5, 141, 91, frame);
			tft.drawFastHLine(224, 141, 91, frame);
			tft.drawFastHLine(0, 190, 320, frame);
			tft.drawFastHLine(0, 215, 320, frame);

			tft.drawString("LAP", 6, 50, 1);
			tft.drawString("BEST", 6, 98, 1);
			tft.drawString("DELTA", 6, 145, 1);
			tft.drawString("POSITION", 225, 50, 1);
			tft.drawString("CURRENT", 225, 98, 1);
			tft.drawString("LAST", 225, 145, 1);

			// Balanced status row: driving aids, centred fuel gauge, fuel and REM.
			tft.drawRect(110, 196, 9, 13, muted);
			tft.drawRect(112, 198, 5, 4, muted);
			tft.drawFastHLine(109, 211, 12, muted);
			tft.drawFastVLine(120, 200, 8, muted);
			tft.drawLine(118, 198, 121, 201, muted);
			tft.setTextDatum(ML_DATUM);
			tft.drawString("ABS", 6, 203, 1);
			tft.drawString("TCS", 51, 203, 1);
			tft.drawString(state.fuelLabel, fuelLabelX, 203, 1);
			tft.drawString("REM", remLabelX, 203, 1);
			tft.setTextDatum(TL_DATUM);
		}

		drawMonoRpm(state, forceUpdate);
		drawMonoValue(4, 63, 93, 27, state.tyrePressureRearLeft,
			"monoLap", white, 2, MC_DATUM, forceUpdate, 1.34f, 1.22f);
		drawMonoValue(4, 111, 93, 27, state.bestLapTime,
			"monoBest", white, 1, MC_DATUM, forceUpdate, 1.52f, 1.38f);
		uint16_t deltaColor = white;
		if (state.sessionBestLiveDeltaSeconds.startsWith("-")) deltaColor = white;
		else if (state.sessionBestLiveDeltaSeconds.startsWith("+") &&
			state.sessionBestLiveDeltaSeconds != "+0.000") deltaColor = muted;
		String delta = state.sessionBestLiveDeltaSeconds;
		if (delta.startsWith("+") || delta.startsWith("-"))
			delta = delta.substring(0, 1) + " " + delta.substring(1);
		drawMonoValueBuffered(4, 158, 93, 25, delta, "monoDelta", deltaColor,
			1, MC_DATUM, forceUpdate, deltaSprite, deltaSpriteCreated,
			1.52f, 1.38f);

		drawMonoGear(state.gear, white, forceUpdate);
		drawMonoSpeed(state.speed, forceUpdate);

		drawMonoValue(223, 63, 94, 27, state.tyrePressureFrontRight,
			"monoPos", white, 2, MC_DATUM, forceUpdate, 1.42f, 1.24f);
		drawMonoValueBuffered(223, 111, 94, 27, state.currentLapTime,
			"monoCurrent", state.lapInvalidated == "True" ? muted : white,
			1, MC_DATUM, forceUpdate, currentSprite, currentSpriteCreated,
			1.52f, 1.38f);
		drawMonoValue(223, 158, 94, 25, state.lastLapTime,
			"monoLast", white, 1, MC_DATUM, forceUpdate, 1.52f, 1.38f);

		drawMonoFuelGauge(state.fuelProgressPercent, state.fuelValueValid, forceUpdate);
		drawMonoValue(fuelValueX, 196, 23, 15, state.fuelDisplayValue,
			"monoFuelValue", white, 1, ML_DATUM, forceUpdate);
		drawMonoValue(remValueX, 196, 24, 15, state.tyrePressureFrontLeft,
			"monoRem", white, 1, ML_DATUM, forceUpdate);
		drawMonoStatusDot(35, 203, isActiveValue(state.absActive),
			"monoAbs", white, forceUpdate);
		drawMonoStatusDot(80, 203, isActiveValue(state.tcActive),
			"monoTcs", white, forceUpdate);
		drawMonoTyres(state, forceUpdate, tyreSprite, tyreSpriteCreated);
		drawMonoPedals(state, forceUpdate);
	}
