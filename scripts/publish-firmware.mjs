#!/usr/bin/env node

import { createHash } from "node:crypto";
import { copyFile, mkdir, readFile, rm, stat, writeFile } from "node:fs/promises";
import { homedir } from "node:os";
import { dirname, join, relative, resolve } from "node:path";
import { spawnSync } from "node:child_process";
import { fileURLToPath } from "node:url";

const scriptDirectory = dirname(fileURLToPath(import.meta.url));
const projectDirectory = resolve(scriptDirectory, "..");
const variants = [
  {
    id: "ili9341",
    environment: process.env.PLATFORMIO_ENV ?? "esp32",
    firmwareName: "firmware-ili9341.bin",
  },
  {
    id: "st7789",
    environment: process.env.PLATFORMIO_ST7789_ENV ?? "esp32-st7789",
    firmwareName: "firmware-st7789.bin",
  },
];
const firmwareDirectory = join(projectDirectory, "installer", "firmware");
const versionPath = join(projectDirectory, "VERSION");
const versionHeaderPath = join(projectDirectory, "include", "version.h");
const manifestPath = join(projectDirectory, "installer", "manifest.json");
const manifestPaths = [
  manifestPath,
  join(projectDirectory, "installer", "manifest-st7789.json"),
];
const releasesPath = join(projectDirectory, "installer", "releases.json");
const releaseNotesPath = join(projectDirectory, "installer", "release-notes.json");
const versionsDirectory = join(projectDirectory, "installer", "versions");
const retainedReleaseCount = 10;
const versionPattern = /^\d+\.\d+\.\d+$/;

function displayPath(path) {
  return relative(projectDirectory, path).replaceAll("\\", "/");
}

async function fileContents(path) {
  try {
    return await readFile(path, "utf8");
  } catch (error) {
    if (error.code === "ENOENT") {
      return null;
    }
    throw error;
  }
}

async function writeIfChanged(path, contents, modifiedFiles) {
  if ((await fileContents(path)) === contents) {
    return;
  }

  await mkdir(dirname(path), { recursive: true });
  await writeFile(path, contents, "utf8");
  modifiedFiles.push(displayPath(path));
}

function parseArguments() {
  const argumentsList = process.argv.slice(2);
  const skipBuild = argumentsList.includes("--skip-build");
  const positional = argumentsList.filter((argument) => argument !== "--skip-build");

  if (positional.length > 1 || argumentsList.some((argument) => argument.startsWith("--") && argument !== "--skip-build")) {
    throw new Error("Usage: node scripts/publish-firmware.mjs [x.y.z] [--skip-build]");
  }

  return { suppliedVersion: positional[0], skipBuild };
}

async function resolveVersion(suppliedVersion) {
  const version = suppliedVersion ?? (await readFile(versionPath, "utf8")).trim();

  if (!versionPattern.test(version)) {
    throw new Error(`Invalid version "${version}". Expected x.y.z using digits only.`);
  }

  return version;
}

async function synchronizeVersion(version) {
  const modifiedFiles = [];

  await writeIfChanged(versionPath, `${version}\n`, modifiedFiles);
  await writeIfChanged(
    versionHeaderPath,
    `#pragma once\n\ninline constexpr char GT7_DASH_VERSION[] = "${version}";\n`,
    modifiedFiles,
  );

  for (const currentManifestPath of manifestPaths) {
    const manifest = JSON.parse(await readFile(currentManifestPath, "utf8"));
    manifest.version = version;
    await writeIfChanged(
      currentManifestPath,
      `${JSON.stringify(manifest, null, 2)}\n`,
      modifiedFiles,
    );
  }

  return modifiedFiles;
}

async function verifySynchronizedVersion(expectedVersion) {
  const version = (await readFile(versionPath, "utf8")).trim();
  const header = await readFile(versionHeaderPath, "utf8");
  const headerMatch = header.match(/inline constexpr char GT7_DASH_VERSION\[\] = "(\d+\.\d+\.\d+)";/);
  const manifests = [];
  for (const currentManifestPath of manifestPaths) {
    const manifest = JSON.parse(await readFile(currentManifestPath, "utf8"));
    if (manifest.version !== version) {
      throw new Error(`Version verification failed for ${displayPath(currentManifestPath)}.`);
    }
    manifests.push({ path: currentManifestPath, manifest });
  }

  if (version !== expectedVersion || headerMatch?.[1] !== version) {
    throw new Error("Version verification failed for VERSION or include/version.h.");
  }

  return manifests;
}

function platformioCandidates() {
  const candidates = [];

  if (process.env.PLATFORMIO_CMD) {
    candidates.push(process.env.PLATFORMIO_CMD);
  }

  candidates.push("pio", "platformio");

  if (process.platform === "win32") {
    candidates.push(
      join(homedir(), ".platformio", "penv", "Scripts", "pio.exe"),
      join(homedir(), ".platformio", "penv", "Scripts", "platformio.exe"),
    );
  } else {
    candidates.push(
      join(homedir(), ".platformio", "penv", "bin", "pio"),
      join(homedir(), ".platformio", "penv", "bin", "platformio"),
    );
  }

  return [...new Set(candidates)];
}

function runBuild(environment) {
  for (const command of platformioCandidates()) {
    const result = spawnSync(command, ["run", "-e", environment], {
      cwd: projectDirectory,
      encoding: "utf8",
      stdio: "inherit",
      shell: false,
    });

    if (result.error?.code === "ENOENT") {
      continue;
    }

    if (result.error) {
      throw result.error;
    }

    if (result.status !== 0) {
      throw new Error(`PlatformIO exited with status ${result.status}.`);
    }

    return command;
  }

  throw new Error(
    "PlatformIO was not found. Install PlatformIO, add pio to PATH, or set PLATFORMIO_CMD.",
  );
}

function frameworkBootAppPath() {
  const coreDirectory = process.env.PLATFORMIO_CORE_DIR
    ? resolve(process.env.PLATFORMIO_CORE_DIR)
    : join(homedir(), ".platformio");

  return join(
    coreDirectory,
    "packages",
    "framework-arduinoespressif32",
    "tools",
    "partitions",
    "boot_app0.bin",
  );
}

async function verifyNonEmpty(path) {
  const details = await stat(path);

  if (!details.isFile() || details.size === 0) {
    throw new Error(`Expected a non-empty binary file: ${path}`);
  }

  return details.size;
}

async function sha256(path) {
  const contents = await readFile(path);
  return createHash("sha256").update(contents).digest("hex");
}

async function verifyManifestBinaries(manifests) {
  for (const { path: currentManifestPath, manifest } of manifests) {
    const parts = manifest.builds?.flatMap((build) => build.parts ?? []) ?? [];

    if (parts.length === 0) {
      throw new Error(`${displayPath(currentManifestPath)} does not reference any binary files.`);
    }

    for (const part of parts) {
      const binaryPath = resolve(dirname(currentManifestPath), part.path);
      await verifyNonEmpty(binaryPath);
    }
  }
}

function printSummary({ version, skipBuild, copiedFiles, modifiedFiles }) {
  console.log("\nRelease summary");
  console.log(`  Version: ${version}`);
  console.log(`  PlatformIO environments: ${variants.map((variant) => variant.environment).join(", ")}`);
  console.log(`  Manifests: ${manifestPaths.map(displayPath).join(", ")}`);
  console.log(`  Build: ${skipBuild ? "skipped" : "successful"}`);
  console.log(`  Copied binaries: ${copiedFiles.length ? copiedFiles.join(", ") : "none (--skip-build)"}`);
  console.log(`  Modified files: ${modifiedFiles.length ? modifiedFiles.join(", ") : "none"}`);
}

async function copyVerifiedBinary(source, destinationName, copiedFiles) {
  await verifyNonEmpty(source);
  const destination = join(firmwareDirectory, destinationName);
  await copyFile(source, destination);
  const size = await verifyNonEmpty(destination);
  copiedFiles.push(displayPath(destination));
  console.log(`${destinationName}: ${size} bytes, SHA-256 ${await sha256(destination)}`);
}

async function archiveRelease(version, manifests, copiedFiles, modifiedFiles) {
  const notesByVersion = JSON.parse(await readFile(releaseNotesPath, "utf8"));
  const notes = notesByVersion[version];

  if (!Array.isArray(notes) || notes.length === 0) {
    throw new Error(
      `Add at least one short entry for ${version} to installer/release-notes.json.`,
    );
  }

  const versionDirectory = join(versionsDirectory, version);
  await mkdir(versionDirectory, { recursive: true });

  for (const binaryName of [
    "bootloader.bin",
    "partitions.bin",
    "boot_app0.bin",
    ...variants.map((variant) => variant.firmwareName),
  ]) {
    const source = join(firmwareDirectory, binaryName);
    const destination = join(versionDirectory, binaryName);
    await copyFile(source, destination);
    await verifyNonEmpty(destination);
    copiedFiles.push(displayPath(destination));
  }

  const archivedManifests = {};
  for (const { manifest } of manifests) {
    const variant = manifest.name.includes("ST7789") ? variants[1] : variants[0];
    const archivedManifestName = `manifest-${variant.id}.json`;
    const archivedManifest = structuredClone(manifest);

    for (const build of archivedManifest.builds ?? []) {
      for (const part of build.parts ?? []) {
        part.path = part.path.replace(/^firmware\//, "");
      }
    }

    await writeIfChanged(
      join(versionDirectory, archivedManifestName),
      `${JSON.stringify(archivedManifest, null, 2)}\n`,
      modifiedFiles,
    );
    archivedManifests[variant.id] = `versions/${version}/${archivedManifestName}`;
  }

  const existingReleases = JSON.parse(
    (await fileContents(releasesPath)) ?? '{"versions":[]}',
  );
  const releaseEntry = {
    version,
    notes,
    manifests: archivedManifests,
  };
  const versions = [
    releaseEntry,
    ...(existingReleases.versions ?? []).filter((entry) => entry.version !== version),
  ].slice(0, retainedReleaseCount);

  await writeIfChanged(
    releasesPath,
    `${JSON.stringify({ versions }, null, 2)}\n`,
    modifiedFiles,
  );

  const retainedVersions = new Set(versions.map((entry) => entry.version));
  for (const previousRelease of existingReleases.versions ?? []) {
    if (!retainedVersions.has(previousRelease.version)) {
      await rm(join(versionsDirectory, previousRelease.version), {
        recursive: true,
        force: true,
      });
    }
  }
}

async function main() {
  const { suppliedVersion, skipBuild } = parseArguments();
  const version = await resolveVersion(suppliedVersion);
  const modifiedFiles = await synchronizeVersion(version);
  let manifests = await verifySynchronizedVersion(version);

  if (skipBuild) {
    printSummary({ version, skipBuild, copiedFiles: [], modifiedFiles });
    return;
  }

  for (const variant of variants) {
    console.log(`Building ${variant.id} with PlatformIO environment: ${variant.environment}`);
    const command = runBuild(variant.environment);
    console.log(`Build completed with: ${command}`);
  }

  await mkdir(firmwareDirectory, { recursive: true });

  const copiedFiles = [];
  console.log(`Copying firmware to: ${firmwareDirectory}`);

  const standardBuildDirectory = join(
    projectDirectory,
    ".pio",
    "build",
    variants[0].environment,
  );
  for (const sharedBinary of ["bootloader.bin", "partitions.bin"]) {
    await copyVerifiedBinary(
      join(standardBuildDirectory, sharedBinary),
      sharedBinary,
      copiedFiles,
    );
  }
  await copyVerifiedBinary(frameworkBootAppPath(), "boot_app0.bin", copiedFiles);

  for (const variant of variants) {
    const buildDirectory = join(projectDirectory, ".pio", "build", variant.environment);
    await copyVerifiedBinary(
      join(buildDirectory, "firmware.bin"),
      variant.firmwareName,
      copiedFiles,
    );
  }

  manifests = await verifySynchronizedVersion(version);
  await verifyManifestBinaries(manifests);
  await archiveRelease(version, manifests, copiedFiles, modifiedFiles);
  printSummary({ version, skipBuild, copiedFiles, modifiedFiles });
}

main().catch((error) => {
  console.error(`publish-firmware: ${error.message}`);
  process.exitCode = 1;
});
