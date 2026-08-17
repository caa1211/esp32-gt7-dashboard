# Dashboard Theme Development Guide

This guide explains how to add or maintain a dashboard theme without duplicating telemetry, connection, touch, or settings logic.

## Architecture

The dashboard has three layers:

1. GT7 parsing and derived metrics update the canonical `DashboardState`.
2. `renderDashboard()` selects a renderer from the persisted `DashboardTheme` enum.
3. Each renderer reads the same state and controls presentation only.

Theme code lives primarily in `src/SHCustomProtocol.h`. A renderer must not parse packets, own a second telemetry model, reset Wi-Fi, or change the touch workflow.

The included themes are:

| Enum | Stored value | Renderer | Purpose |
| --- | ---: | --- | --- |
| `Classic` | 0 | `drawPage1Legacy()` | Original five-column engineering UI |
| `GT3` | 1 | `drawPage1()` | Default dark motorsport UI |
| `Retro` | 2 | `drawRetroDashboard()` | Warm paper-and-ink instrument UI |

Persisted values are part of the device settings format. Never reorder or reuse an existing enum value.

Display brightness is stored separately in the same `gt7dash` Preferences namespace. It uses a validated percentage (`20`–`100`) rather than a menu index. Changes are applied immediately and written after a short idle delay to avoid unnecessary flash writes. Wi-Fi reset must not clear theme or brightness preferences.

## Data available to renderers

`DashboardState` is the renderer contract. It includes:

- RPM percentage, engine RPM, alert range validity, min-alert position and rev-limit state
- gear and speed
- current, last and best lap times and live delta
- tyre temperatures and the existing REM/LAP/POS display fields
- fuel display data
- throttle/brake input and filtered applied values
- ABS and TCS state
- game-running state

If a new telemetry field is genuinely required by every theme, add it to the shared state and populate it in the parser/derived-data layer. Do not obtain it inside one renderer. A theme should not invent unsupported values merely to match a reference image.

## Adding a theme

1. Append a new explicit value to `DashboardTheme`; do not change existing values.
2. Extend `isValidDashboardTheme()` and `dashboardThemeName()`.
3. Add a renderer branch in `renderDashboard()`.
4. Implement a renderer that accepts `const DashboardState &` and `forceUpdate`.
5. Add the theme to both theme arrays used by the Settings drawing and activation code.
6. Redesign Theme Selection if the buttons no longer fit comfortably.
7. Document the theme in both root README files.
8. Build and test both `esp32` (ILI9341) and `esp32-st7789` targets.

The current 320 x 240 Settings screen intentionally fits three large theme buttons plus Back. Three is not a firmware limit, but it is the recommended usability limit for the current menu. A fourth theme should introduce pagination or Previous/Next controls instead of smaller touch targets.

## Renderer rules

- Treat `forceUpdate` as a complete redraw request.
- Use theme-specific cache keys in `prevData` and `prevColor`, such as `retroSpeed`.
- Never depend on another theme having drawn first.
- Switching themes must leave no stale pixels; `invalidateDashboardRenderer()` clears shared caches and requests a full redraw.
- Keep geometry, colors and fonts local to the renderer or a theme-specific layout namespace.
- Avoid blocking animation. Use `millis()` and frame intervals.
- Only animate the region that needs animation.
- Keep RPM fill clamped to the available segment count and preserve a safe fallback when GT7 alert values are invalid.

## Preventing flicker

SPI displays expose a visible flash when code clears a frequently updated area and draws the replacement in separate transfers.

Use one of these approaches:

- Redraw only changed segments or pixels.
- Compose a small dynamic region in an `LGFX_Sprite`, then call `pushSprite()` once.
- Cache the rendered value and skip unchanged frames.

Do not allocate a full-screen 16-bit sprite on these boards. Prefer small persistent sprites for high-frequency fields such as speed, current time, pedals and tyre temperatures. If sprite allocation can fail, retain a direct-draw fallback where practical.

Fractionally scaling small bitmap fonts can create malformed or illegible glyphs on the 2.8-inch display. Prefer a native font size for small labels and timing values, then adjust the containing rectangle rather than squeezing the font.

## Shift-light behavior

All themes receive the same dynamic RPM state:

- RPM fill maps to the vehicle-specific alert range when valid.
- `rpmRedLineSetting` represents the min-alert position as a percentage of the active bar range.
- warning animation starts in the alert range and strengthens toward the upper range.
- `revLimitAlertActive` requests the strongest warning.

A theme may express this in its own visual language, but inactive elements and unrelated UI must not pulse. Animation must remain non-blocking.

## Build and hardware verification

Run both targets:

```powershell
platformio run -e esp32 -e esp32-st7789
```

For each display controller, verify:

- fresh boot and telemetry connection
- every telemetry field updates without stale pixels
- low, mid and alert-range RPM behavior
- repeated switching among all themes
- saved theme restoration after power cycling
- Settings Back, Device Settings brightness and timeout behavior
- Wi-Fi reset Cancel and confirmation
- automatic sleep, tap-to-wake and active-screen tap-to-Settings behavior
- no visible flicker in high-frequency fields
- no watchdog reset or obvious heap degradation during a driving session

Use photographs of the physical 2.8-inch display for typography and contrast approval. Desktop mockups do not reproduce RGB565 color, viewing angle, SPI update behavior or actual text legibility.

## Release checklist

After hardware acceptance:

1. Mark the phase complete in [THEME_SWITCH_PLAN.md](THEME_SWITCH_PLAN.md).
2. Choose the release version and update `VERSION` and `include/version.h`.
3. Add release notes.
4. Build and archive both controller binaries.
5. Update installer manifests and verify the selectable version list.
6. Commit the accepted checkpoint before publishing.

See [RELEASING.md](RELEASING.md) for binary staging and installer publishing details.
