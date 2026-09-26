	static uint8_t pocketGlyphRow(char c, int row)
	{
		static constexpr uint8_t DIGITS[10][7] = {
			{14, 17, 19, 21, 25, 17, 14}, {4, 12, 4, 4, 4, 4, 14},
			{14, 17, 1, 2, 4, 8, 31}, {30, 1, 1, 14, 1, 1, 30},
			{2, 6, 10, 18, 31, 2, 2}, {31, 16, 16, 30, 1, 1, 30},
			{14, 16, 16, 30, 17, 17, 14}, {31, 1, 2, 4, 8, 8, 8},
			{14, 17, 17, 14, 17, 17, 14}, {14, 17, 17, 15, 1, 1, 14}};
		if (c >= '0' && c <= '9') return DIGITS[c - '0'][row];
		static constexpr uint8_t N[7] = {17, 25, 25, 21, 19, 19, 17};
		static constexpr uint8_t R[7] = {30, 17, 17, 30, 20, 18, 17};
		static constexpr uint8_t DASH[7] = {0, 0, 0, 14, 0, 0, 0};
		static constexpr uint8_t SLASH[7] = {1, 2, 2, 4, 8, 8, 16};
		if (c == 'N') return N[row];
		if (c == 'R') return R[row];
		if (c == '-') return DASH[row];
		if (c == '/') return SLASH[row];
		return 0;
	}

	void drawPocketPixelText(LGFX_Sprite &sprite, const String &text,
		int centerX, int y, int scale, uint16_t color)
	{
		const int glyphW = 5 * scale;
		const int pitch = 6 * scale;
		const int startX = centerX - (text.length() * pitch - scale) / 2;
		for (size_t i = 0; i < text.length(); ++i)
		{
			for (int row = 0; row < 7; ++row)
			{
				const uint8_t bits = pocketGlyphRow(text[i], row);
				for (int col = 0; col < 5; ++col)
					if (bits & (1 << (4 - col)))
						sprite.fillRect(startX + i * pitch + col * scale,
							y + row * scale, scale, scale, color);
			}
		}
	}

	void drawPocketGearSpeed(const DashboardState &state, bool forceUpdate)
	{
		const String cacheState = state.gear + ":" + state.speed;
		if (!forceUpdate && prevData["pocketGearSpeed"] == cacheState) return;
		static LGFX_Sprite sprite(&tft);
		static bool created = false;
		if (!created)
		{
			sprite.setColorDepth(16);
			created = sprite.createSprite(158, 105) != nullptr;
		}
		if (!created) return;
		const uint16_t pale = tft.color565(188, 205, 124);
		const uint16_t darkest = tft.color565(23, 43, 18);
		sprite.fillSprite(pale);
		drawPocketPixelText(sprite, state.gear, 79, 4, 8, darkest);
		drawPocketPixelText(sprite, state.speed, 79, 72, 4, darkest);
		sprite.setTextColor(darkest, pale);
		sprite.setTextDatum(ML_DATUM);
		sprite.setTextSize(1.25f, 1.20f);
		sprite.drawString("KM/H", 120, 94, 1);
		sprite.setTextSize(1.0f);
		sprite.pushSprite(7, 50);
		prevData["pocketGearSpeed"] = cacheState;
		releaseSpriteAfterThemePreview(sprite, created);
	}

	void drawPocketValue(int x, int y, int w, int h, const String &value,
		const String &id, uint16_t color, uint16_t background, uint8_t font,
		float scaleX, float scaleY, bool forceUpdate,
		textdatum_t datum = MC_DATUM)
	{
		const String cacheState = value + ":" + String(color);
		if (!forceUpdate && prevData[id] == cacheState) return;
		tft.fillRect(x, y, w, h, background);
		tft.setTextColor(color, background);
		tft.setTextDatum(datum);
		tft.setTextSize(scaleX, scaleY);
		const int textX = datum == MR_DATUM ? x + w - 2
			: (datum == ML_DATUM ? x + 2 : x + w / 2);
		tft.drawString(value, textX, y + h / 2, font);
		tft.setTextSize(1.0f);
		tft.setTextDatum(TL_DATUM);
		prevData[id] = cacheState;
	}

	void drawPocketPixelValue(int x, int y, int w, int h,
		const String &value, const String &id, uint16_t color,
		uint16_t background, int scale, bool forceUpdate)
	{
		if (!forceUpdate && prevData[id] == value) return;
		tft.fillRect(x, y, w, h, background);
		const int pitch = 6 * scale;
		const int textWidth = value.length() * pitch - scale;
		const int startX = x + (w - textWidth) / 2;
		const int startY = y + (h - 7 * scale) / 2;
		for (size_t i = 0; i < value.length(); ++i)
			for (int row = 0; row < 7; ++row)
			{
				const uint8_t bits = pocketGlyphRow(value[i], row);
				for (int col = 0; col < 5; ++col)
					if (bits & (1 << (4 - col)))
						tft.fillRect(startX + i * pitch + col * scale,
							startY + row * scale, scale, scale, color);
			}
		prevData[id] = value;
	}

	void drawPocketValueBuffered(int x, int y, int w, int h,
		const String &value, const String &id, uint16_t color,
		uint16_t background, uint8_t font, float scaleX, float scaleY,
		bool forceUpdate, LGFX_Sprite &sprite, bool &created)
	{
		const String cacheState = value + ":" + String(color);
		if (!forceUpdate && prevData[id] == cacheState) return;
		if (!created)
		{
			sprite.setColorDepth(16);
			created = sprite.createSprite(w, h) != nullptr;
		}
		if (!created)
		{
			drawPocketValue(x, y, w, h, value, id, color, background,
				font, scaleX, scaleY, true);
			return;
		}
		sprite.fillSprite(background);
		sprite.setTextColor(color, background);
		sprite.setTextDatum(MC_DATUM);
		sprite.setTextSize(scaleX, scaleY);
		sprite.drawString(value, w / 2, h / 2, font);
		sprite.setTextSize(1.0f);
		sprite.pushSprite(x, y);
		prevData[id] = cacheState;
		releaseSpriteAfterThemePreview(sprite, created);
	}

	void drawPocketPixelFrame(int x, int y, int w, int h, uint16_t color)
	{
		static constexpr int T = 2;
		static constexpr int FOLD = 6;
		static constexpr int DEPTH = 6;
		// Orthogonal inward folds: across, inward, back to the outer side.
		tft.fillRect(x + FOLD, y, w - FOLD * 2, T, color);
		tft.fillRect(x + FOLD - T, y, T, DEPTH, color);
		tft.fillRect(x, y + DEPTH - T, FOLD, T, color);
		tft.fillRect(x + w - FOLD, y, T, DEPTH, color);
		tft.fillRect(x + w - FOLD, y + DEPTH - T, FOLD, T, color);

		tft.fillRect(x, y + DEPTH - T, T, h - (DEPTH - T) * 2, color);
		tft.fillRect(x + w - T, y + DEPTH - T, T,
			h - (DEPTH - T) * 2, color);

		tft.fillRect(x, y + h - DEPTH, FOLD, T, color);
		tft.fillRect(x + FOLD - T, y + h - DEPTH, T, DEPTH, color);
		tft.fillRect(x + FOLD, y + h - T, w - FOLD * 2, T, color);
		tft.fillRect(x + w - FOLD, y + h - DEPTH, FOLD, T, color);
		tft.fillRect(x + w - FOLD, y + h - DEPTH, T, DEPTH, color);
	}

	void drawPocketRpm(const DashboardState &state, bool forceUpdate)
	{
		static constexpr int COUNT = 14;
		static constexpr int X = 24;
		static constexpr int Y = 190;
		static constexpr int PITCH = 10;
		static int previousActive = -1;
		static uint8_t previousPulseStep = 255;
		const uint16_t darkest = tft.color565(23, 43, 18);
		const uint16_t dark = tft.color565(76, 95, 47);
		const uint16_t light = tft.color565(154, 166, 92);
		const int active = constrain((state.rpmPercent * COUNT + 99) / 100, 0, COUNT);
		const bool warning = (state.rpmAlertRangeValid &&
			state.rpmPercent >= state.rpmRedLineSetting) || state.revLimitAlertActive;
		uint8_t pulseStep = 0;
		if (warning)
		{
			const float wave = 0.5f + 0.5f * cosf(TWO_PI * 3.0f * millis() / 1000.0f);
			pulseStep = static_cast<uint8_t>(lroundf(wave * 5.0f));
		}
		if (!forceUpdate && active == previousActive && pulseStep == previousPulseStep)
			return;
		const uint16_t activeColor = warning
			? blendRgb565(darkest, dark, pulseStep * 35) : darkest;
		const uint16_t pale = tft.color565(188, 205, 124);
		// Clear only the marker lane; never touch the surrounding 2 px frame.
		tft.fillRect(13, 182, 158, 7, pale);
		for (int i = 0; i < COUNT; ++i)
		{
			const uint16_t color = i < active ? activeColor : light;
			tft.fillRect(X + i * PITCH, Y, 8, 12, color);
		}
		if (state.rpmAlertRangeValid)
		{
			const int markerX = X + constrain(
				state.rpmRedLineSetting * ((COUNT - 1) * PITCH) / 100,
				0, (COUNT - 1) * PITCH);
			tft.fillTriangle(markerX, 188, markerX - 3, 182,
				markerX + 3, 182, darkest);
		}
		previousActive = active;
		previousPulseStep = pulseStep;
	}

	void drawPocketFuelAids(const DashboardState &state, bool forceUpdate)
	{
		const bool absOn = isActiveValue(state.absActive);
		const bool tcsOn = isActiveValue(state.tcActive);
		const String fuelRenderState = state.fuelDisplayValue + ":" +
			String(state.fuelProgressPercent) + ":" + state.fuelLabel + ":" +
			String(state.fuelValueValid ? 1 : 0);
		const bool fuelChanged = forceUpdate ||
			prevData["pocketFuel"] != fuelRenderState;
		const bool absChanged = forceUpdate ||
			prevData["pocketAbs"] != state.absActive;
		const bool tcsChanged = forceUpdate ||
			prevData["pocketTcs"] != state.tcActive;
		if (!fuelChanged && !absChanged && !tcsChanged) return;
		const uint16_t pale = tft.color565(188, 205, 124);
		const uint16_t darkest = tft.color565(23, 43, 18);
		const uint16_t dark = tft.color565(76, 95, 47);
		const uint16_t light = tft.color565(132, 146, 80);
		tft.setTextColor(darkest, pale);
		tft.setTextDatum(MC_DATUM);
		const bool validFuel = state.fuelValueValid;
		if (fuelChanged)
		{
			if (forceUpdate)
				tft.fillRect(176, 165, 44, 60, pale);
			else
				tft.fillRect(176, 165, 44, 18, pale);
			tft.setTextSize(1.0f);
			String displayedFuel = state.fuelDisplayValue;
			if (validFuel && !state.fuelIsEV) displayedFuel += "%";
			tft.drawString(validFuel ? displayedFuel : "--", 198, 174, 2);
			// The pump doubles as a coarse pixel fuel gauge. Its color boundary
			// always advances by complete horizontal rows: dark fuel below,
			// lighter empty space above. Missing telemetry is treated as full.
			const int fuelPercent = validFuel
				? constrain(state.fuelProgressPercent, 0, 100)
				: 100;
			drawDashboardFuelPumpIcon(191, 184, fuelPercent,
				darkest, light, pale);
			if (forceUpdate)
				tft.drawString(state.fuelLabel, 198, 220, 1);
		}

		// Match the RPM bar: inactive is pale, active is the solid dark tone.
		const uint16_t tcsColor = tcsOn ? darkest : light;

		if (absChanged)
		{
			if (forceUpdate)
				tft.fillRect(222, 165, 44, 60, pale);
			else
				tft.fillRect(228, 179, 33, 31, pale);
			drawDashboardAbsIcon(244, 194, absOn,
				darkest, light, pale);
			tft.setTextDatum(MC_DATUM);
			tft.setTextColor(darkest, pale);
			if (forceUpdate)
				tft.drawString("ABS", 244, 220, 1);
		}

		if (tcsChanged)
		{
			if (forceUpdate)
				tft.fillRect(268, 165, 38, 60, pale);
			else
				tft.fillRect(278, 180, 25, 31, pale);
			tft.setTextColor(darkest, pale);
			drawDashboardTcsIcon(279, 181, tcsColor, pale);
			if (forceUpdate)
				tft.drawString("TCS", 290, 220, 1);
		}

		// Three equal status columns.
		for (int y = 167; y < 227; y += 8)
		{
			tft.fillRect(220, y, 2, 4, dark);
			tft.fillRect(266, y, 2, 4, dark);
		}
		tft.setTextDatum(TL_DATUM);
		// Dynamic clearing must never leave the outer folded corners incomplete.
		drawPocketPixelFrame(10, 162, 300, 69, dark);
		prevData["pocketFuel"] = fuelRenderState;
		prevData["pocketAbs"] = state.absActive;
		prevData["pocketTcs"] = state.tcActive;
	}

	void drawPocketBottom(const DashboardState &state, bool forceUpdate)
	{
		String tyreState;
		for (int i = 0; i < 4; ++i)
			tyreState += (i ? ":" : "") +
				(isnan(state.tyreTemperatures[i]) ? String("--")
					: String(lroundf(state.tyreTemperatures[i])));
		const String cacheState = tyreState + ":" + state.absLevel + ":" +
			state.absFilteredLevel + ":" + state.tcLevel + ":" + state.tcFilteredLevel;
		if (!forceUpdate && prevData["pocketBottom"] == cacheState) return;
		const uint16_t pale = tft.color565(188, 205, 124);
		const uint16_t darkest = tft.color565(23, 43, 18);
		const uint16_t dark = tft.color565(76, 95, 47);
		static LGFX_Sprite sprite(&tft);
		static bool created = false;
		if (!created)
		{
			sprite.setColorDepth(16);
			created = sprite.createSprite(310, 20) != nullptr;
		}
		if (!created)
		{
			// Allocation can temporarily fail after browsing several themes. Keep
			// the panel functional and retry the buffered path on a later update.
			tft.fillRect(5, 216, 310, 20, pale);
			tft.setTextColor(darkest, pale);
			tft.setTextDatum(ML_DATUM);
			tft.drawString("TYRE", 8, 226, 1);
			const char *fallbackLabels[] = {"FL", "FR", "RL", "RR"};
			for (int i = 0; i < 4; ++i)
				tft.drawString(String(fallbackLabels[i]) +
					(isnan(state.tyreTemperatures[i]) ? String("--")
						: String(lroundf(state.tyreTemperatures[i]))),
					41 + i * 31, 226, 1);
			const int fallbackValues[] = {
				constrain(state.absFilteredLevel.toInt(), 0, 100),
				constrain(state.tcFilteredLevel.toInt(), 0, 100)};
			for (int row = 0; row < 2; ++row)
			{
				const int y = 218 + row * 9;
				tft.drawString(row == 0 ? "B" : "T", 179, y + 4, 1);
				tft.drawRect(190, y, 122, 7, dark);
                if ((row == 0 ? state.absLevel : state.tcLevel) == "--") tft.drawString("--", 240, y + 4, 1);
				if (fallbackValues[row] > 0)
					tft.fillRect(191, y + 1,
						120 * fallbackValues[row] / 100, 5, darkest);
			}
			prevData["pocketBottom"] = cacheState;
			return;
		}

		sprite.fillSprite(pale);
		sprite.setTextColor(darkest, pale);
		sprite.setTextDatum(ML_DATUM);
		sprite.drawString("TYRE", 3, 10, 1);
		const char *labels[] = {"FL", "FR", "RL", "RR"};
		for (int i = 0; i < 4; ++i)
		{
			const String value = String(labels[i]) +
				(isnan(state.tyreTemperatures[i]) ? String("--")
					: String(lroundf(state.tyreTemperatures[i])));
			sprite.drawString(value, 36 + i * 31, 10, 1);
		}
		const int values[] = {constrain(state.absFilteredLevel.toInt(), 0, 100),
			constrain(state.tcFilteredLevel.toInt(), 0, 100)};
		for (int row = 0; row < 2; ++row)
		{
			const int y = 2 + row * 9;
			sprite.drawString(row == 0 ? "B" : "T", 174, y + 4, 1);
			sprite.drawRect(185, y, 122, 7, dark);
            if ((row == 0 ? state.absLevel : state.tcLevel) == "--") sprite.drawString("--", 235, y + 4, 1);
			if (values[row] > 0)
				sprite.fillRect(186, y + 1, 120 * values[row] / 100, 5, darkest);
		}
		sprite.pushSprite(5, 216);
		prevData["pocketBottom"] = cacheState;
		releaseSpriteAfterThemePreview(sprite, created);
	}

	void drawPocketDashboard(const DashboardState &state, bool forceUpdate)
	{
		static LGFX_Sprite currentSprite(&tft);
		static bool currentSpriteCreated = false;
		const uint16_t pale = tft.color565(188, 205, 124);
		const uint16_t darkest = tft.color565(23, 43, 18);
		const uint16_t dark = tft.color565(76, 95, 47);
		if (forceUpdate)
		{
			tft.fillScreen(tft.color565(154, 166, 92));
			tft.fillRect(5, 5, 310, 230, pale);
			tft.setTextSize(1.0f);
			tft.setTextColor(pale, darkest);
			tft.setTextDatum(TL_DATUM);
			tft.fillRect(9, 14, 31, 18, darkest);
			// Single-pixel clipped corners keep the label chunky but less boxy.
			tft.drawPixel(9, 14, pale);
			tft.drawPixel(39, 14, pale);
			tft.drawPixel(9, 31, pale);
			tft.drawPixel(39, 31, pale);
			tft.drawString("LAP", 15, 19, 1);
			tft.fillRect(181, 14, 31, 18, darkest);
			tft.drawPixel(181, 14, pale);
			tft.drawPixel(211, 14, pale);
			tft.drawPixel(181, 31, pale);
			tft.drawPixel(211, 31, pale);
			tft.drawString("POS", 187, 19, 1);
			for (int x = 8; x < 312; x += 8) tft.fillRect(x, 39, 4, 2, dark);
			drawPocketPixelFrame(174, 49, 136, 106, dark);
			for (int y = 84; y < 150; y += 34)
				for (int x = 180; x < 306; x += 8) tft.fillRect(x, y, 4, 2, dark);
			tft.setTextColor(darkest, pale);
			tft.setTextDatum(ML_DATUM);
			tft.drawString("BEST", 180, 71, 1);
			tft.drawString("LAST", 180, 105, 1);
			tft.drawString("CUR", 180, 139, 1);
			tft.setTextDatum(TL_DATUM);
			drawPocketPixelFrame(10, 162, 300, 69, dark);
			// Match the other dividers: FUEL's left edge is a dotted line.
			for (int y = 167; y < 227; y += 8)
				tft.fillRect(173, y, 2, 4, dark);
			// RPM needs a slight optical correction to match the wider DELTA word.
			tft.setTextSize(1.30f, 1.20f);
			tft.drawString("RPM", 18, 173, 1);
			tft.setTextSize(1.20f, 1.15f);
			tft.setTextDatum(ML_DATUM);
			tft.drawString("DELTA", 18, 216, 1);
			tft.setTextSize(1.0f);
			tft.setTextDatum(TL_DATUM);
		}

		drawPocketPixelValue(43, 12, 90, 22, state.tyrePressureRearLeft,
			"pocketLap", darkest, pale, 2, forceUpdate);
		drawPocketPixelValue(215, 13, 92, 20, state.tyrePressureFrontRight,
			"pocketPos", darkest, pale, 2, forceUpdate);
		drawPocketGearSpeed(state, forceUpdate);
		drawPocketValue(210, 59, 97, 24, state.bestLapTime,
			"pocketBest", darkest, pale, 1, 1.65f, 1.50f, forceUpdate);
		drawPocketValue(210, 93, 97, 24, state.lastLapTime,
			"pocketLast", darkest, pale, 1, 1.65f, 1.50f, forceUpdate);
		drawPocketValueBuffered(210, 127, 97, 24, state.currentLapTime,
			"pocketCurrent", state.lapInvalidated == "True" ? dark : darkest,
			pale, 1, 1.65f, 1.50f, forceUpdate,
			currentSprite, currentSpriteCreated);
		drawPocketRpm(state, forceUpdate);
		drawPocketValue(66, 204, 102, 24, state.sessionBestLiveDeltaSeconds,
			"pocketDelta", darkest, pale, 2, 1.0f, 1.0f, forceUpdate, MR_DATUM);
		drawPocketFuelAids(state, forceUpdate);
	}
