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
| `Radar` | 3 | `drawRadarDashboard()` | Circular tachometer-focused dark UI |
| `Mono` | 4 | `drawMonoDashboard()` | Monochrome retro-digital instrument UI |
| `Pocket` | 5 | `drawPocketDashboard()` | Four-tone handheld pixel instrument UI |

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
5. Append the theme to the centralized `DASHBOARD_THEMES` catalog. Theme Selection uses
   this catalog for looping navigation and indicators, so it must not maintain a separate
   theme count or list.
6. Verify the real renderer with the selector's fixed mock telemetry. Do not add preview
   bitmap assets or write mock values into the live/global telemetry state.
7. Document the theme in both root README files.
8. Build and test both `esp32` (ILI9341) and `esp32-st7789` targets.

The 320 x 240 Theme Selection screen previews one real renderer at a time and cycles through
the centralized catalog. Adding a theme should therefore not require shrinking touch targets
or redesigning the selector.

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

## High-frequency UI fields

Treat every field according to how often it changes. Static frames and labels may be drawn
only during a full redraw. Discrete values such as gear, lap and position should redraw only
when their displayed value changes. Continuous values such as speed, RPM, current time,
pedals and tyre temperatures may change on nearly every telemetry packet and need an explicit
partial-redraw strategy.

For every high-frequency field:

1. Define one fixed rectangular ownership area that contains every pixel the field can draw.
   Include the widest expected value, sign, decimal point, anti-aliased edge and any pulse or
   color state. That area must not overlap borders, labels or another dynamic field.
2. Build a cache key from every input that can alter the pixels, not only the numeric value.
   Include formatted text, color, active state, fill width or animation frame as applicable.
3. If the cache key is unchanged and `forceUpdate` is false, do no drawing and no clearing.
4. When it changes, compose the complete ownership area off-screen in a small persistent
   `LGFX_Sprite`: clear it to the theme background, draw all layers in their final order, then
   transfer it with one `pushSprite()` call.
5. Update the cache only after the final drawing operation succeeds. A failed sprite creation
   must not make later frames look successfully rendered.

Pedal bars and other layered indicators must redraw their background, commanded/input layer,
filtered/applied layer and border together in the same sprite. Updating those layers in
separate screen transactions commonly creates a visible flash or a temporary one-layer state.

Segmented RPM displays usually do not need a sprite for the entire arc. Cache the active
segment count and relevant color/pulse state, then redraw only segments whose final appearance
changed. A segment must always be drawn with the same geometry whether active or inactive;
do not let clearing rectangles, adjacent panels or endpoint decoration cover part of it.
Animated shift warnings should be frame-limited with `millis()` and must not force unrelated
dashboard fields to redraw.

Avoid these patterns in a live renderer:

- clearing a screen rectangle and returning before its replacement is drawn;
- clearing an area larger than the field owns;
- drawing a border before a later dynamic clear that overlaps it;
- comparing raw floating-point telemetry when the displayed value is rounded;
- allocating and deleting a sprite on every frame;
- using a full-screen sprite to solve a small-field flicker problem;
- calling `fillScreen()` or forcing a full layout redraw for ordinary telemetry changes;
- storing preview/mock telemetry in the canonical runtime state.

When a field still flickers, first test it with a constant displayed value. If it flickers while
the cache key is constant, another drawing region is overlapping it. If it flickers only when
the value changes, its clear-and-redraw path is visible or its sprite bounds/background are
incorrect. Inspect ownership rectangles and render order before adding delays or increasing
the global refresh rate.

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
