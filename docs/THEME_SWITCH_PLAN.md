# Dashboard Theme Switching Plan

See also: [Theme Development Guide](THEME_DEVELOPMENT.md).

## Radar Theme Extension

The post-1.3.0 Radar extension adds a fourth presentation-only renderer while preserving the existing shared telemetry, persistence, touch, brightness, Wi-Fi and sleep layers.

- [x] Append stable enum value `Radar = 3` without changing existing saved values.
- [x] Add a 2 x 2 Theme Selection grid with full-size touch targets and a consistent Back button.
- [x] Add a circular, segmented RPM renderer with vehicle-relative fill, alert marker and non-blocking shift pulse.
- [x] Present the same supported lap, position, timing, fuel, tyre, pedal, delta, ABS, TCS, gear and speed data.
- [x] Build both ILI9341 and ST7789 targets within existing memory limits.
- [x] Flash the ST7789 development build to COM3 for physical review.
- [x] Complete live-GT7 typography and flicker acceptance on the physical ST7789 display.
- [ ] Choose a release number and publish installer binaries after acceptance.

**Acceptance record:** The Radar renderer was iterated on the physical ST7789 board with live telemetry. Final fixes separate dynamic sprite bounds from the RPM arc, retain stable RPM segment geometry and endpoint ticks, use buffered high-frequency fields, and provide readable timing, pedal, tyre-temperature, ABS/TCS, RPM, speed and gear presentation. Both display targets build successfully; extended-session acceptance can continue as ordinary regression testing before packaging a release.

## Project Status

This is a major UI architecture renovation. Work proceeds one phase at a time, and every phase must pass its physical-device acceptance gate before the next phase begins.

Overall progress:

- [x] **Phase 1 — Theme infrastructure** — accepted on ST7789 hardware (2026-08-17)
- [x] **Phase 2 — Touch Settings and theme selection** — accepted on ST7789 and ILI9341 hardware (2026-08-17)
- [x] **Phase 3 — Classic 1.2.5 renderer** — accepted on ST7789 hardware (2026-08-17)
- [x] **Phase 4 — Retro renderer** — accepted on ST7789 hardware (2026-08-18)
- [x] **Phase 5 — Integration, regression testing and release** — released as 1.3.0 (2026-08-18)

Working rule for every phase:

1. Implement only the current phase's defined scope.
2. Build both ILI9341 and ST7789 targets.
3. Upload the ST7789 build to the development board when hardware verification is required.
4. Test against that phase's acceptance checklist on the physical display.
5. Record and fix failures without starting work from the next phase.
6. Mark the phase complete only after user approval.
7. Commit the accepted phase as a known-good rollback point.
8. Begin the next phase.

Update the checkboxes above as work is accepted. Implementation progress alone does not count as phase completion; the corresponding real-device checks must pass.

## Goal

Add an on-device theme system without changing the telemetry features already available in the dashboard.

The first release should provide three themes:

1. **Classic** — restore the dashboard appearance from version 1.2.5.
2. **GT3** — preserve the current version 1.2.7 dashboard exactly as the default theme.
3. **Retro** — add a new light-gray background, dark-text, vintage instrument-panel theme.

All themes must display the same supported dashboard information. A theme changes presentation only; it must not introduce a separate telemetry implementation or add/remove features.

## Design Principles

- Keep GT7 UDP parsing and derived telemetry independent from UI rendering.
- Give each theme its own layout and drawing functions.
- Avoid spreading `if (theme == ...)` conditions through existing drawing functions.
- Reuse existing drawing helpers only when they are genuinely shared.
- Preserve the current GT3 renderer as much as possible.
- Keep memory usage suitable for the ESP32; do not keep multiple full-screen sprites allocated.
- Assign stable numeric values to themes so future menu reordering does not change saved selections.

## Theme Model

Use a stable enum similar to:

```cpp
enum class DashboardTheme : uint8_t {
    Classic = 0,
    GT3 = 1,
    Retro = 2,
};
```

Rules:

- `GT3` is the default when no saved value exists.
- An unknown or invalid saved value falls back to `GT3`.
- New themes receive new numeric values; existing values must never be reassigned.
- Save the selected theme in the project's existing persistent settings storage.

## Rendering Architecture

The intended flow is:

```text
GT7 UDP packet
      ↓
Shared telemetry/dashboard state
      ↓
Theme dispatcher
      ├── Classic renderer
      ├── GT3 renderer
      └── Retro renderer
```

The shared state should continue to provide the existing values, including:

- RPM and GT7 alert range
- Rev-limit alert state
- Gear and speed
- Lap, position and lap times
- Delta
- Fuel
- Tyre data
- Brake and throttle
- ABS and TCS
- Connection and dashboard status

Each renderer owns:

- Coordinates and layout constants
- Fonts, colors and visual styling
- Static background drawing
- Dynamic field drawing
- Theme-specific previous-value/redraw state

The main dashboard drawing entry point should dispatch to the active renderer. Switching themes must clear the display and force a complete redraw.

## Settings Entry and Controls

The physical board button should have a simple role:

- Normal dashboard short press: retain the current screen on/off behavior.
- Normal dashboard long press: open Settings.
- While Settings is open, a physical-button press may act as a fallback exit to the dashboard.

Do not require users to distinguish short, medium and very long presses for different settings operations.

After Settings opens, use large touch buttons. The current Wi-Fi reset screen has already demonstrated acceptable touch accuracy on the 320 × 240 display, so its button drawing, hit-testing and debounce behavior should be reused where practical.

## Settings Screens

### Main Settings

```text
SETTINGS

[ SELECT THEME ]
[ RESET WIFI   ]
[ BACK         ]
```

### Theme Selection

```text
SELECT THEME

[ CLASSIC      ]
[ GT3       ✓  ]
[ RETRO        ]
[ BACK         ]
```

The active theme should be clearly marked. Selecting a theme should:

1. Validate the selected value.
2. Save it to persistent storage.
3. Clear the screen.
4. Invalidate renderer caches.
5. Draw the selected dashboard immediately.

No reboot should be required.

### Wi-Fi Reset

Keep a confirmation step before deleting saved Wi-Fi settings:

```text
RESET SAVED WIFI?

[ CANCEL     ]
[ RESET WIFI ]
```

The destructive action must never occur from the first tap on the main Settings screen.

## Touch UI Requirements

- Target resolution: landscape 320 × 240.
- Use large, well-separated buttons.
- Make hitboxes at least as large as the visible button bounds.
- Trigger actions on touch release, not initial contact.
- Show a pressed/highlight state before activation.
- Debounce touch input so one touch produces one action.
- Prevent the long press used to enter Settings from activating a menu item.
- Provide a Settings inactivity timeout that safely returns to the dashboard.
- Keep destructive controls separated from ordinary navigation controls.

## Theme Requirements

### GT3

- Preserve the version 1.2.7 appearance and behavior.
- Keep the adaptive RPM mapping, dynamic colors, min-alert marker and shift pulse.
- Use GT3 as the default and compatibility fallback.
- Refactoring for theme dispatch must not visually change this renderer.

### Classic

- Recover the version 1.2.5 drawing/layout implementation from Git history.
- Port only the renderer and its visual assets/helpers.
- Connect it to the current telemetry state and current infrastructure.
- Do not restore outdated parsing, Wi-Fi, input or settings code from 1.2.5.
- Match the original 1.2.5 appearance as closely as practical.

### Retro

Visual direction:

- Warm light-gray or beige-gray background.
- Dark charcoal text, borders and filled indicators.
- Thin instrument-panel dividers and framed sections.
- Condensed, squared or industrial typography where available and legible.
- Mostly monochrome presentation.
- Muted accent colors only where state meaning requires them, such as delta and shift warnings.

Functional rules:

- Present the same functions available in the current dashboard—no more and no fewer.
- Do not add reference-image fields such as clock, track name or fuel mix unless the current dashboard already supports them reliably.
- Rearrange existing BEST/LAST/CURRENT, brake/throttle and other current information to fit the Retro layout.
- Preserve the current telemetry-driven RPM scaling and alert logic even if the RPM bar is visually redesigned for the Retro theme.
- Keep critical values readable at a glance on the 2.8-inch 320 × 240 screen.

## UI State and Cache Handling

Settings is an application UI state, not a theme:

```text
Application UI
├── Dashboard
│   ├── Classic
│   ├── GT3
│   └── Retro
└── Settings
    ├── Main
    ├── Theme Selection
    └── Wi-Fi Reset Confirmation
```

On every transition between Settings and Dashboard, or between themes:

- Clear the screen.
- Reset or invalidate the relevant previous-value cache.
- Force all static and dynamic elements to redraw once.
- Reset temporary touch state.
- Avoid carrying alert animation state into unrelated UI screens.

Theme-specific caches should not prevent another renderer from drawing unchanged telemetry values.

## Implementation Phases

The work should be divided into **five stages**. Each stage has its own acceptance gate and should be tested on the physical board before beginning the next stage. This keeps input handling, renderer switching and visual work from being debugged at the same time.

### Phase 1 — Theme Infrastructure

- Add the stable theme enum and saved setting.
- Add UI state management for Dashboard and Settings.
- Add renderer dispatch.
- Preserve GT3 as the default renderer.
- Implement safe screen clearing and cache invalidation.
- Confirm that GT3 remains visually unchanged.

#### Phase 1 Acceptance

Build a temporary development version in which all three enum values can be selected internally, even if Classic and Retro still use placeholder screens.

Pass conditions:

- [ ] Both ILI9341 and ST7789 targets compile.
- [ ] The normal boot path still opens the GT3 dashboard.
- [ ] Existing users with no theme setting receive GT3.
- [ ] Invalid stored values fall back to GT3.
- [ ] Switching the internal theme value clears the old screen and completely redraws the new renderer.
- [ ] Returning to GT3 produces the same layout, colors and telemetry behavior as version 1.2.7.
- [ ] Live telemetry continues updating after repeated renderer switches.
- [ ] No obvious heap growth, crash or reboot occurs after at least 20 repeated theme switches.

Do not proceed if GT3 changes visually or if stale pixels remain after switching.

**Acceptance record:** Phase 1 was accepted after the normal GT3 build was flashed to the ST7789 board and confirmed visually unchanged with normal dashboard operation. Interactive theme switching, persistence after user selection and repeated-switch stress testing require the Settings entry point, so those checks are intentionally carried forward into Phase 2.

### Phase 2 — Touch Settings

- Replace the direct Wi-Fi-reset long-press flow with entry into Settings.
- Implement the main Settings screen.
- Implement Theme Selection with Classic, GT3 and Retro options.
- Integrate the existing Wi-Fi reset confirmation behavior.
- Add touch debounce, release activation and inactivity timeout.
- Verify persistence across reboot.

#### Phase 2 Acceptance

Test this phase on the physical 2.8-inch 320 × 240 board, using only the board button and touchscreen.

Pass conditions:

- [ ] A dashboard long press reliably enters Settings.
- [ ] The entry press cannot accidentally activate the first menu item.
- [ ] All buttons can be tapped comfortably without touchscreen calibration.
- [ ] A visual pressed state identifies the button being activated.
- [x] Tapping and releasing produces only one action.
- [ ] Theme Selection lists Classic, GT3 and Retro and marks the active choice.
- [ ] Selecting each theme saves it and returns to the corresponding renderer or placeholder.
- [ ] Power cycling restores the last selected theme.
- [ ] `BACK`, the physical-button fallback and inactivity timeout all return safely to the dashboard.
- [ ] `RESET WIFI` cannot erase settings without a second confirmation.
- [ ] Cancelling Wi-Fi reset leaves the saved network untouched.
- [ ] Confirming Wi-Fi reset retains the currently expected reboot/setup behavior.

Do not proceed until Settings can be operated repeatedly without accidental selection or lockout.

**Acceptance record:** Phase 2 was accepted after the touch Settings flow, active-theme indication, persisted enum selection, GT3/Classic/Retro switching, Back navigation and Wi-Fi reset confirmation were exercised on both ST7789 and ILI9341 boards. Full-screen button feedback flicker and incomplete Connecting-screen redraw after theme switching were found during hardware testing and corrected before acceptance.

### Phase 3 — Classic Renderer

- Identify the exact 1.2.5 renderer files/functions in Git history.
- Extract only the presentation logic.
- Adapt it to current telemetry fields.
- Verify that it does not reintroduce obsolete behavior.

#### Phase 3 Acceptance

Compare Classic on the physical display against the version 1.2.5 UI reference while feeding current GT7 telemetry.

Pass conditions:

- [x] The major layout, typography, colors and visual hierarchy match version 1.2.5.
- [x] Every current dashboard function has a defined place; no current function disappears.
- [x] No obsolete 1.2.5 parser, Wi-Fi or button behavior has been restored.
- [x] Speed, gear, RPM, timing, fuel, tyre, pedal and driver-aid fields update correctly.
- [x] RPM scaling and shift-warning behavior use the current telemetry logic.
- [x] Returning from Classic to GT3 leaves neither stale pixels nor stale cached values.
- [x] Classic remains stable for at least one normal driving session.

Minor pixel-level visual tuning may remain, but data correctness and clean switching are required before Phase 4.

**Acceptance record:** Phase 3 was accepted after physical ST7789 testing of the restored Classic layout, current telemetry fields, theme switching and the two-layer throttle/brake presentation. Pedal cells are composed off-screen to prevent flicker; driver input uses a white base layer and the filtered actual output uses the existing yellow/blue foreground. The final white-base adjustment was explicitly accepted without another visual verification cycle.

### Phase 4 — Retro Renderer

- Create a 320 × 240 layout sketch using the existing data fields.
- Implement the static panel and labels first.
- Add dynamic values using partial redraws.
- Integrate RPM colors and shift warning behavior.
- Tune contrast, font sizes and spacing on the physical display.

#### Phase 4 Acceptance

Use the supplied reference image as a style reference, not as a requirement to add unsupported data fields.

Pass conditions:

- [x] The UI clearly reads as a warm gray, dark-text vintage instrument panel.
- [x] All current dashboard functions are present, with no additional unsupported functions.
- [x] Speed and gear remain readable at a glance on the 2.8-inch display.
- [x] Small labels and timing values remain legible from the normal driving position.
- [x] Dynamic values do not flicker, jump horizontally or leave background artifacts.
- [x] Positive/negative delta and shift warnings remain distinguishable without overpowering the monochrome style.
- [x] RPM fill, min-alert position and shift alert respond correctly for cars with different alert RPM ranges.
- [x] Switching among Retro, Classic and GT3 repeatedly produces complete, clean redraws.
- [x] Retro remains stable for at least one normal driving session.

Visual approval should be based on photographs of the actual display rather than desktop mockups alone.

**Acceptance record:** Phase 4 was accepted after iterative ST7789 hardware testing. The final Retro palette uses a `#C6BA98` paper background with dark ink, restrained status accents and the existing telemetry semantics. RPM, speed, current-lap time, tyre temperatures and pedal bars use partial or buffered redraws to eliminate visible SPI clearing flicker. The supplied dashboard image was treated as a visual reference rather than a layout specification.

### Phase 5 — Integration and Release

- Test all three themes with live GT7 telemetry.
- Test repeated theme switching without rebooting.
- Test saved-theme restoration after power cycling.
- Test Settings entry/exit while telemetry is active and inactive.
- Test Wi-Fi reset confirmation and cancellation.
- Build both ILI9341 and ST7789 variants.
- Update README instructions and release notes.

#### Phase 5 Acceptance

Run a final regression pass without temporary theme overrides or test-only screens.

Pass conditions:

- [x] A clean build succeeds for `esp32` and `esp32-st7789`.
- [x] Both binaries are flashed or otherwise validated on their intended display target where hardware is available.
- [x] Fresh installation defaults to GT3.
- [x] Upgrade from version 1.2.7 preserves a usable default and does not require manual storage clearing.
- [x] Classic, GT3 and Retro all receive identical live telemetry inputs and expose the same supported functions.
- [x] Theme selection survives reboot and ordinary Wi-Fi reconnection.
- [x] Screen on/off, Settings entry, Settings timeout and Wi-Fi reset all pass regression testing.
- [x] No theme-specific crash, watchdog reset or obvious memory degradation appears during an extended session.
- [x] README documents Settings entry, theme switching, the three included themes and Wi-Fi reset.
- [x] Release notes describe the theme system and identify GT3 as the default.
- [x] Installer manifests and archived firmware packages contain the correct version and both display builds.

Release only after all five phase gates pass. A phase may be committed independently so there is always a known-good rollback point.

**Acceptance record:** Phase 5 completed with clean ILI9341 and ST7789 builds, persisted enum-based theme selection, repeated physical theme-switch testing, Settings and Wi-Fi-reset regression checks, and bilingual documentation. Release 1.3.0 was generated by the publishing helper, which validated and archived both display-controller binaries; the final ST7789 image was also flashed successfully to COM3.

## Recommended Commit Boundaries

Keep implementation history aligned with the five acceptance gates:

1. `Add dashboard theme infrastructure`
2. `Add touch settings and theme selection`
3. `Restore Classic dashboard renderer`
4. `Add Retro dashboard renderer`
5. `Finalize multi-theme release`

Small fixes discovered during physical testing can be committed within their phase, but architectural work and visual tuning should not be mixed into the same commit when avoidable.

## Acceptance Checklist

### Theme System

- [ ] GT3 is the default for existing users and invalid saved values.
- [ ] All three themes are selectable on the device.
- [ ] The selected theme survives a reboot.
- [ ] Switching themes leaves no pixels from the previous layout.
- [ ] Switching themes does not require restarting telemetry or the ESP32.

### Settings and Input

- [ ] A single long-press action reliably opens Settings.
- [ ] Settings can be operated using large touch buttons.
- [ ] Entering Settings cannot accidentally press the first menu item.
- [ ] Wi-Fi reset requires explicit confirmation.
- [ ] Settings can always return to the dashboard.

### Rendering

- [ ] GT3 remains visually and functionally equivalent to version 1.2.7.
- [ ] Classic reproduces the version 1.2.5 presentation using current data handling.
- [ ] Retro follows the gray-background, dark-text vintage direction.
- [ ] Every theme exposes the same supported dashboard functions.
- [ ] RPM mapping and shift warnings remain telemetry-driven.
- [ ] No theme causes unacceptable flicker, stale values or memory instability.

### Build Targets

- [ ] `esp32` / ILI9341 builds successfully.
- [ ] `esp32-st7789` builds successfully.
- [ ] Both targets can enter Settings and switch all themes.

## Scope Guardrails

The initial theme release should not include:

- Touchscreen calibration UI.
- User-created custom themes.
- Network-based theme management.
- New telemetry features solely because they appear in the Retro reference image.
- A general-purpose UI framework larger than needed for these three renderers.

These can be reconsidered after the three-theme implementation is stable on hardware.
