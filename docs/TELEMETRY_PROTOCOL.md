# Telemetry sources and DSH1

User-facing installation steps, including Multiple Arduino/Multiple USB device discovery and the Custom Protocol editor, are available in the [English](../SIMHUB_README.md) and [Traditional Chinese](../SIMHUB_README.zh-TW.md) SimHub USB guides.

The firmware has two inputs: validated GT7 UDP and SimHub Arduino Custom Protocol over physical USB serial. Transport adapters retain separate snapshots. `TelemetrySelector` chooses one snapshot; every theme renders the same `DashboardState`. Renderer animation caches are preserved between updates and invalidated on source changes. GT7 derived metrics reset on source changes or a stale GT7 stream.

## USB framing

Keep the existing SimHub Arduino ARQ framing and command handshake. Command `P` begins a bounded, incremental line receiver. SimHub appends LF; CRLF is accepted. The custom-command `0x15` acknowledgement follows the complete line (or abort), not its first fragment. Max payload is 383 ASCII characters excluding LF; the line has exactly 25 semicolon-separated fields, with **no trailing semicolon**. Idle fragments time out after 300 ms, or 1500 ms total. Invalid/overflow/incomplete lines never update the snapshot or liveness. Unsupported versions are rejected, not guessed. The lower ARQ layer accepts at most 32 bytes per CRC-validated frame and never overwrites unread payload.

| Index | Field | Unit / format |
|---|---|---|
| 0 | Version | literal `DSH1` |
| 1 | Sequence | uint32, changes every formula evaluation |
| 2 | Game running | required `0` / `1` |
| 3 | Speed | 0–1500 km/h |
| 4 | Gear | `N`, `R`, `1`–`9` |
| 5 | Engine RPM | 0–30000 |
| 6 | Displayed RPM | 0–100 % |
| 7 | Redline | 0–100 %; 0 disables the alert |
| 8–10 | Current / last / best lap | `mm:ss.fff`; up to 999 minutes |
| 11 | Remaining fuel laps | 0–9999 laps |
| 12 | Position | integer 0–9999 |
| 13–14 | Current / total laps | integer 0–9999 |
| 15 | Fuel | 0–100 % |
| 16–17 | Throttle / brake | 0–100 % |
| 18–20 | TC active / ABS active / invalid lap | `0` / `1` |
| 21–24 | Tyre temperature FL / FR / RL / RR | −100–1000 °C |

Every field from index 3 onward may be `--`. Empty strings, NaN, infinity, malformed numbers, unknown gear strings and out-of-range values are invalid. Optional fields may become available/unavailable each packet. Boolean strings are normalized by the sender to `0` / `1`. Sequence repetition is ignored for liveness; sequence changes (including wraparound and SimHub restart) are accepted. No ordering assumption across PC restarts.

Example:

```text
DSH1;42;1;140;4;5600;68;90;01:24.631;01:25.104;01:24.382;4.5;5;3;10;80;68;36;0;1;0;82;79;76;78
```

## Connection selection and persistence

- First setup runs touch orientation before presenting explicit **Direct GT7** and **SimHub USB** choices. The connection choice is persisted.
- Direct GT7 enables Wi-Fi and GT7 UDP only. With no saved network it opens the setup portal; Wi-Fi cannot be skipped in this mode.
- SimHub USB keeps the Wi-Fi radio off and accepts only valid DSH1 frames over physical USB serial.
- Switching connection from the Waiting screen or Device Settings preserves other preferences. Reset to Default clears the connection, Wi-Fi credentials, touch orientation, theme and brightness, then returns to Touch Setup.
- Handshake/USB presence alone never affects telemetry freshness. Continued idle frames maintain an existing lock; disconnect the idle sender to release it immediately.
- Wi-Fi work runs on a separate FreeRTOS task because portal scans/saves can wait internally. Task-owned WiFiManager communicates with the main USB/render loop using bounded command/status queues. GT7 UDP is owned by the main loop.
- Device Settings exposes direct GT7/SimHub choices and Reset to Default. Reset does not require an MCU restart.

## Validation

Run formula tests with:

```sh
node --test tests/simhub-formula.test.mjs
```

Run the actual C++ parser and selector tests on a host with g++:

```sh
g++ -std=c++11 -Wall -Wextra -pedantic -fsanitize=address,undefined tests/telemetry.cpp -o telemetry-tests
./telemetry-tests
```

The `Telemetry protocol tests` CI workflow runs both. Formula tests use mocked property values, not an installed SimHub session. C++ tests cover malformed/missing fields, range validation, idle sources, source contention, manual selection, expiry and timer wraparound. Build both panels with `pio run -e esp32 -e esp32-st7789`.

### Hardware acceptance (still required before release)

- Fresh NVS: verify Touch Setup precedes connection selection. Direct GT7 opens Wi-Fi Setup when needed; SimHub USB leaves Wi-Fi off.
- Upgrade without a saved connection choice: preserve existing preferences and show connection selection.
- Confirm SimHub discovery at 19200, baud negotiation, Custom Protocol evaluation, all 25 property mappings, and continuous stationary/paused frames.
- Run GT7 only, USB only, both, wrong Wi-Fi credentials, no formula, invalid formula, unplug/reconnect, and SimHub restart.
- While USB telemetry runs, enter and cancel GT7 Wi-Fi Setup; verify responsive rendering and no USB reconnect, protocol loss or mixed-source values.
- Exercise both display controllers and every theme with valid/missing fuel, temperatures, RPM, pedals and aids; verify sleep/wake, preview, source change and memory stability.
- Confirm settings are reachable while waiting and during play; verify portal cancellation, retries and Reset to Default.

This protocol is intentionally incompatible with the historical unversioned `custProtocol.txt` and upstream tyre-pressure mapping. Install the new formula together with the new firmware.
