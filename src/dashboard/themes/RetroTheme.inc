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
		releaseSpriteAfterThemePreview(sprite, spriteCreated);
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
		drawRetroTextValue(5, 25, 49, 20, state.engineRpm < 0 ? "--" : String(displayedRpm), "retroRpm",
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
		const String value = state.absLevel + ":" + state.tcLevel + ":" + String(brakeInput) + ":" + String(brakeApplied) + ":" +
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
		if (!spriteCreated)
		{
			// Keep the complete pedal panel visible if heap fragmentation after a
			// theme switch temporarily prevents allocating the sprite.
			tft.fillRect(143, 204, 93, 33, paper);
			tft.setTextColor(ink, paper);
			tft.setTextDatum(TL_DATUM);
			tft.drawString("B", 151, 208, 1);
			tft.drawString("T", 151, 221, 1);
			const int values[2][2] = {
				{brakeInput, brakeApplied}, {throttleInput, throttleApplied}};
			for (int row = 0; row < 2; ++row)
			{
				const int y = 211 + row * 13;
				tft.drawRect(164, y, 68, 7, faint);
                if ((row == 0 ? state.absLevel : state.tcLevel) == "--") tft.drawString("--", 189, y, 1);
				const int inputW = 66 * values[row][0] / 100;
				const int appliedW = 66 * values[row][1] / 100;
				if (inputW > 0) tft.fillRect(165, y + 1, inputW, 5, ink);
				if (appliedW > 0) tft.fillRect(165, y + 1, appliedW, 5, applied);
			}
			prevData["retroPedals"] = value;
			return;
		}
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
            if ((row == 0 ? state.absLevel : state.tcLevel) == "--") pedalSprite.drawString("--", barX + 25, y, 1);
			const int inputW = (barW - 2) * values[row][0] / 100;
			const int appliedW = (barW - 2) * values[row][1] / 100;
			if (inputW > 0) pedalSprite.fillRect(barX + 1, y + 1, inputW, barH - 2, ink);
			if (appliedW > 0) pedalSprite.fillRect(barX + 1, y + 1, appliedW, barH - 2, applied);
		}
		pedalSprite.pushSprite(143, 204);
		prevData["retroPedals"] = value;
		releaseSpriteAfterThemePreview(pedalSprite, spriteCreated);
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
			tft.drawString("CURRENT", 192, 124, 1);
			tft.drawString(state.fuelLabel, 8, 166, 1);
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
		drawRetroTextValueBuffered(190, 134, 124, 14, state.currentLapTime,
			"retroCurrent", currentColor, 2, MR_DATUM, forceUpdate,
			currentSprite, currentSpriteCreated, 0.9f, 0.82f);

		uint16_t deltaColor = ink;
		if (state.sessionBestLiveDeltaSeconds.startsWith("-")) deltaColor = green;
		else if (state.sessionBestLiveDeltaSeconds.startsWith("+") &&
			state.sessionBestLiveDeltaSeconds != "+0.000") deltaColor = red;
		drawRetroTextValue(190, 148, 124, 8, state.sessionBestLiveDeltaSeconds,
			"retroDelta", deltaColor, 1, MR_DATUM, forceUpdate);

		drawRetroTextValue(7, 177, 68, 18, state.fuelDisplayValue, "retroFuel", ink,
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
