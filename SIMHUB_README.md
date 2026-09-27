# SimHub USB setup

The dashboard supports **Direct GT7** over Wi-Fi and **SimHub USB** for PC games. SimHub USB does not require Wi-Fi. All dashboard themes use the same Custom Protocol formula.

## 1. Prepare the dashboard

1. Connect the dashboard to the PC with a data-capable USB cable.
2. Select **SIMHUB USB** on the dashboard. If it is currently in GT7 mode, open **Settings → Device Settings → Change Connection → SIMHUB USB**.
3. Close Arduino Serial Monitor, PlatformIO Serial Monitor, and any other application using the same COM port.

## 2. Let SimHub detect the dashboard

1. Open SimHub and go to **Arduino**.
2. Open the **Multiple Arduino** or **Multiple USB** devices page. The label varies slightly between SimHub versions; use this page even when only one dashboard is connected.
3. Enable Arduino support so SimHub scans the USB devices.
4. Find the dashboard's COM port, then add or enable that device. Its firmware identification is `GT7 SimHub Dash`.
5. Wait for SimHub to show the device as connected. Detecting the COM port only confirms the USB connection; telemetry requires the Custom Protocol in the next section.

The initial connection uses 19200 baud and SimHub negotiates the later rate automatically. No virtual COM port, TCP bridge, or extra plugin is required.

## 3. Add the Custom Protocol

1. Open **Custom Protocol** or its formula editor for the Arduino device you just added.
2. Enable **Use JavaScript**.
3. Open [simhub/custom-protocol.txt](simhub/custom-protocol.txt), copy its **entire contents**, and paste them into the formula field.
4. Select Apply or Save and verify that the protocol is enabled for the correct COM device.
5. Do not append `\n`; SimHub adds the line ending automatically.

This uses SimHub's **Arduino Custom Protocol**, not the Custom Serial Devices plugin. Do not use SimHub's generic Arduino sketch upload because it would overwrite the dashboard firmware.

## 4. Start the game

1. Start or select a supported PC game in SimHub.
2. Confirm that SimHub is receiving game data.
3. The dashboard changes from **Waiting for SimHub** to the selected theme.

If SimHub detects the device but the dashboard remains on Waiting:

- **USB linked: set Custom Protocol**: USB and SimHub are linked, but no valid formula data has arrived. Enable **Use JavaScript**, paste the complete formula, and select Apply or Save.
- **Check Custom Protocol (DSH1)**: the formula has the wrong format or version. Paste [simhub/custom-protocol.txt](simhub/custom-protocol.txt) again.
- **Waiting for SimHub** remains: verify the data cable, selected COM port, serial-port ownership, and that SimHub is receiving live game data.
- Individual values show `--`: the current game may not expose those properties; other supported values still work.
- Fuel never changes: paste the latest [simhub/custom-protocol.txt](simhub/custom-protocol.txt) again. The formula prefers SimHub's live `FuelPercent` alias and calculates from fuel capacity when available.
- The firmware filters a brief neutral value during a gear change for 750 ms. A sustained or initial neutral still displays `N`.

## Notes

- All seven themes use the same formula. Changing themes does not require another SimHub setup.
- The current connection choice is saved. Use **Change Connection** to switch between Direct GT7 and SimHub USB.
- **Reset to Default** clears Wi-Fi and all dashboard preferences, then restarts first-time setup.
- Historical formulas elsewhere in the repository use an older packet format. For this firmware, use only [simhub/custom-protocol.txt](simhub/custom-protocol.txt).

## References

- [Traditional Chinese guide](SIMHUB_README.zh-TW.md)
- [SimHub Custom Arduino Hardware Support](https://github.com/SHWotever/SimHub/wiki/Custom-Arduino-hardware-support)
- [SimHub JavaScript Formula Engine](https://github.com/SHWotever/SimHub/wiki/Javascript-Formula-Engine)
- [Telemetry protocol and validation](docs/TELEMETRY_PROTOCOL.md)
