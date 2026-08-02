# Publishing firmware for the Web Installer

The GitHub Pages workflow publishes only the contents of `installer/`. It does not install
PlatformIO and does not compile the firmware. Build and test every release locally before
committing its binary images.

`VERSION` is the only manually controlled version source. The publish helper synchronizes
that value into the generated C++ header and ESP Web Tools manifest.

## Flash layout

The project uses the `esp32` PlatformIO environment, the classic
`esp32doit-devkit-v1` board, Arduino framework, and `huge_app.csv` partition table.
PlatformIO's resolved upload metadata defines this flash layout:

| Offset | Installer file | Local source |
| --- | --- | --- |
| `0x1000` | `installer/firmware/bootloader.bin` | `.pio/build/esp32/bootloader.bin` |
| `0x8000` | `installer/firmware/partitions.bin` | `.pio/build/esp32/partitions.bin` |
| `0xE000` | `installer/firmware/boot_app0.bin` | PlatformIO's Arduino ESP32 package, `tools/partitions/boot_app0.bin` |
| `0x10000` | `installer/firmware/firmware.bin` | `.pio/build/esp32/firmware.bin` |

These offsets are not arbitrary: the ESP32 PlatformIO integration places the bootloader at
`0x1000`, the partition table at `0x8000`, the OTA data initializer at `0xE000`, and the
application at `0x10000`. The selected `huge_app.csv` confirms that the OTA data partition
starts at `0xE000` and the first application partition starts at `0x10000`.

Re-check PlatformIO's verbose upload command or resolved project metadata if the board,
framework platform, or partition table changes. Update both this document and
`installer/manifest.json` if the flash map changes.

## Build and test locally

1. Install PlatformIO locally and check out the revision to release.
2. Choose a release version and run the single local publishing command. Supply a numeric
   `major.minor.patch` version:

   ```bash
   node scripts/publish-firmware.mjs 1.2.3
   ```

   The script validates the version, updates `VERSION`, `include/version.h`, and
   `installer/manifest.json`, then runs `pio run -e esp32`. After a successful build it
   validates and copies all four non-empty binaries into `installer/firmware/`, verifies
   every manifest binary, and prints sizes, SHA-256 hashes, and a release summary.

   Running the command without a version preserves the existing behavior by reading the
   current value from `VERSION`:

   ```bash
   node scripts/publish-firmware.mjs
   ```

   To synchronize the version files without building or copying binaries, use:

   ```bash
   node scripts/publish-firmware.mjs 1.2.3 --skip-build
   ```

   Set `PLATFORMIO_CMD` if PlatformIO is not on `PATH`, or `PLATFORMIO_ENV` if a future
   release uses another environment. The helper never commits, tags, pushes, creates a
   GitHub Release, or deploys Pages.

   To build without staging installer files, run PlatformIO directly:

   ```bash
   pio run -e esp32
   ```

3. Upload it to the target hardware and test the dashboard:

   ```bash
   pio run -e esp32 -t upload
   ```

4. Confirm that the display, touch input, Wi-Fi setup, and GT7 telemetry work as expected.

## Stage the installer binaries

Create `installer/firmware/` if necessary, then copy the four real binary files listed in
the flash-layout table into it. The first, second, and fourth files come from
`.pio/build/esp32/`. Locate `boot_app0.bin` in the installed Arduino ESP32 framework package
reported by PlatformIO; on typical systems it is under:

```text
<PlatformIO home>/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin
```

Do not commit `.pio/`. Do not add empty or placeholder binary files. Before committing,
verify that all four installer binaries exist and have non-zero sizes:

```bash
git status --short
git check-ignore -v installer/firmware/firmware.bin
```

The second command should produce no output. Review the files that will be committed, then
commit the four binaries together with any intentional manifest version change.

## Verify the installer

1. Serve `installer/` from a local HTTP server or push the tested binaries and allow the
   GitHub Pages workflow to deploy them. Web Serial requires a secure context; the deployed
   HTTPS Pages site is the most representative test.
2. Open the installer in Chrome, Edge, or another desktop browser with Web Serial support.
3. Connect a test ESP32 using a data-capable USB cable and close any serial monitor.
4. Select **Install firmware**, choose the correct serial port, and complete the flash.
5. Power-cycle the device and repeat the functional checks above.
6. In repository **Settings > Pages**, ensure the source is set to **GitHub Actions**. Confirm
   that the deployment succeeded and that the public installer loads its manifest and all
   four binary URLs without 404 errors.

The Pages workflow is intentionally path-filtered and uploads only `installer/`; source code,
documentation, `.pio/`, and local build tooling are not included in the published artifact.
