# Publishing firmware for the Web Installer

The GitHub Pages workflow publishes only the contents of `installer/`. It does not install
PlatformIO and does not compile the firmware. Build and test every release locally before
committing its binary images.

`VERSION` is the only manually controlled version source. The publish helper synchronizes
that value into the generated C++ header and both ESP Web Tools manifests.

## Flash layout

The project uses the `esp32` and `esp32-st7789` PlatformIO environments, the classic
`esp32doit-devkit-v1` board, Arduino framework, and `huge_app.csv` partition table.
PlatformIO's resolved upload metadata defines this flash layout:

| Offset | Installer file | Local source |
| --- | --- | --- |
| `0x1000` | `installer/firmware/bootloader.bin` | `.pio/build/esp32/bootloader.bin` |
| `0x8000` | `installer/firmware/partitions.bin` | `.pio/build/esp32/partitions.bin` |
| `0xE000` | `installer/firmware/boot_app0.bin` | PlatformIO's Arduino ESP32 package, `tools/partitions/boot_app0.bin` |
| `0x10000` | `installer/firmware/firmware-ili9341.bin` | `.pio/build/esp32/firmware.bin` |
| `0x10000` | `installer/firmware/firmware-st7789.bin` | `.pio/build/esp32-st7789/firmware.bin` |

These offsets are not arbitrary: the ESP32 PlatformIO integration places the bootloader at
`0x1000`, the partition table at `0x8000`, the OTA data initializer at `0xE000`, and the
application at `0x10000`. The selected `huge_app.csv` confirms that the OTA data partition
starts at `0xE000` and the first application partition starts at `0x10000`.

Re-check PlatformIO's verbose upload command or resolved project metadata if the board,
framework platform, or partition table changes. Update both this document and
`installer/manifest.json` and `installer/manifest-st7789.json` if the flash map changes.

## Build and test locally

1. Install PlatformIO locally and check out the revision to release.
2. Choose a release version and run the single local publishing command. Supply a numeric
   `major.minor.patch` version:

   ```bash
   node scripts/publish-firmware.mjs 1.2.3
   ```

   The script validates the version; updates `VERSION`, `include/version.h`, and both
   installer manifests; then builds the `esp32` ILI9341 and `esp32-st7789` environments.
   After successful builds it validates and copies the three shared boot files plus both
   application images into `installer/firmware/`, verifies every manifest binary, and
   prints sizes, SHA-256 hashes, and a release summary.

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

Create `installer/firmware/` if necessary, then copy the five real binary files listed in
the flash-layout table into it. The two application images come from their respective
PlatformIO environment build directories. Locate `boot_app0.bin` in the installed Arduino
ESP32 framework package reported by PlatformIO; on typical systems it is under:

```text
<PlatformIO home>/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin
```

Do not commit `.pio/`. Do not add empty or placeholder binary files. Before committing,
verify that all five installer binaries exist and have non-zero sizes:

```bash
git status --short
git check-ignore -v installer/firmware/firmware-ili9341.bin
git check-ignore -v installer/firmware/firmware-st7789.bin
```

The `git check-ignore` commands should produce no output. Review the files that will be
committed, then commit the five binaries together with both manifest version changes.

## Verify the installer

1. Serve `installer/` from a local HTTP server or push the tested binaries and allow the
   GitHub Pages workflow to deploy them. Web Serial requires a secure context; the deployed
   HTTPS Pages site is the most representative test.
2. Open the installer in Chrome, Edge, or another desktop browser with Web Serial support.
3. Connect a test ESP32 using a data-capable USB cable and close any serial monitor.
4. Select the correct display controller, select **Install firmware**, choose the correct
   serial port, and complete the flash.
5. Power-cycle the device and repeat the functional checks above.
6. In repository **Settings > Pages**, ensure the source is set to **GitHub Actions**. Confirm
   that the deployment succeeded and that the public installer loads both manifests and all
   five binary URLs without 404 errors.

The Pages workflow is intentionally path-filtered and uploads only `installer/`; source code,
documentation, `.pio/`, and local build tooling are not included in the published artifact.

## Release and GitHub Pages deployment checklist

Use this sequence for every public firmware release:

1. Publish the firmware locally with the intended numeric version:

   ```bash
   npm run publish:firmware -- 1.2.3
   ```

   This runs `scripts/publish-firmware.mjs`; it synchronizes the version, builds with the
   local PlatformIO installation, copies the required binaries into `installer/firmware/`,
   and validates the installer files.
2. Flash and test the generated firmware on a physical ESP32-2432S028.
3. Serve the local installer without caching:

   ```bash
   http-server installer -p 8080 -c-1
   ```

4. Open <http://localhost:8080> in a Web Serial-compatible desktop browser.
5. Confirm that the Web Installer can install the locally committed firmware and that the
   flashed device works correctly.
6. Review all release changes:

   ```bash
   git diff
   git status
   ```

7. Commit the updated source, version files, manifest, and firmware binaries.
8. Push or merge the changes into `main`.
9. GitHub Actions automatically validates and deploys the committed `installer/` directory
   to GitHub Pages.
10. Verify <https://caa1211.github.io/esp32-gt7-dashboard/> and perform another installer
    test from the deployed site.
11. After the deployed release has been verified, create an annotated Git tag on the
    accepted release commit. Use the `vX.Y.Z` naming convention and keep the tag version
    identical to `VERSION`:

    ```bash
    git tag -a v1.2.3 -m "Release 1.2.3"
    ```

    If the release commit is not currently checked out, specify it explicitly:

    ```bash
    git tag -a v1.2.3 <release-commit> -m "Release 1.2.3"
    ```

12. Push the release tag and verify that it points to the intended commit on GitHub:

    ```bash
    git push origin v1.2.3
    git show --no-patch v1.2.3
    ```

    Pushing the branch does not push a newly created tag automatically. A GitHub Release
    may optionally be created from this tag, but it is separate from the firmware and
    GitHub Pages publishing process.

GitHub Actions does **not** install PlatformIO, build firmware, run the local publishing
script, or generate binaries. It deploys the committed files exactly as they exist under
`installer/`.

### One-time GitHub Pages setting

Configure the repository once at **Repository > Settings > Pages > Build and deployment >
Source > GitHub Actions**. The workflow uses the official Pages artifact deployment flow;
it does not create or maintain a `gh-pages` branch. The contents of `installer/` become the
site root, so the public URL has no additional `/installer/` segment.
