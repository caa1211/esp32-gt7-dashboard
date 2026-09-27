import { readFileSync } from 'node:fs';
import { test } from 'node:test';
import assert from 'node:assert/strict';

const formula = new Function('$prop', 'root', readFileSync(new URL('../simhub/custom-protocol.txt', import.meta.url), 'utf8'));
const props = {
  'DataCorePlugin.GameRunning': true,
  SpeedKmh: 140, Gear: '4', Rpms: 5600,
  CarSettings_CurrentDisplayedRPMPercent: 68, CarSettings_RPMRedLineSetting: 90,
  CurrentLapTime: '00:01:24.6310000', LastLapTime: '00:01:25.104', BestLapTime: '01:24.382',
  Fuel: 30, 'DataCorePlugin.Computed.Fuel_LitersPerLap': 2.5, FuelPercent: 80,
  Position: 5, CurrentLap: 3, TotalLaps: 10, Throttle: 68, Brake: 36,
  TCActive: false, ABSActive: true, LapInvalidated: false,
  TyreTemperatureFrontLeft: 82, TyreTemperatureFrontRight: 79,
  TyreTemperatureRearLeft: 76, TyreTemperatureRearRight: 78,
};
const run = (values, root = {}) => formula(k => values[k] ?? null, root).split(';');

test('formula emits the versioned 25-field contract with units and normalized times', () => {
  const result = run(props);
  assert.deepEqual(result, ['DSH1', '1', '1', '140', '4', '5600', '68', '90',
    '01:24.631', '01:25.104', '01:24.382', '12.0', '5', '3', '10', '80',
    '68', '36', '0', '1', '0', '82.0', '79.0', '76.0', '78.0']);
  assert.ok(result.join(';').length < 384);
});
test('unchanging/paused telemetry still emits a new sequence; wraps safely', () => {
  const root = {};
  assert.equal(run(props, root)[1], '1');
  assert.equal(run(props, root)[1], '2');
  root.dashSequence = 4294967295;
  assert.equal(run(props, root)[1], '0');
});
test('unsupported properties remain missing, not fabricated zero readings', () => {
  const result = run({});
  assert.deepEqual(result.slice(0, 3), ['DSH1', '1', '0']);
  assert.ok(result.slice(3).every(v => v === '--'));
  assert.equal(formula(() => { throw Error('missing'); }, {}).split(';').length, 25);
});
test('zero inputs are real readings, invalid/out-of-range values are missing', () => {
  const result = run({ ...props, SpeedKmh: 0, Gear: -1, Throttle: 0, Brake: 101,
    FuelPercent: NaN, Rpms: Infinity, TCActive: 'unknown' });
  assert.equal(result[3], '0'); assert.equal(result[4], 'R');
  assert.equal(result[5], '--'); assert.equal(result[15], '--');
  assert.equal(result[16], '0'); assert.equal(result[17], '--'); assert.equal(result[18], '--');
});
test('null fuel cannot produce zero remaining laps and times cannot inject fields', () => {
  const result = run({ ...props, Fuel: null, CurrentLapTime: '00:00:01;BAD', LastLapTime: '01:99.000' });
  assert.equal(result[11], '--'); assert.equal(result[8], '--'); assert.equal(result[9], '--');
});
test('full NewData properties work without short aliases', () => {
  const result = run({ 'DataCorePlugin.GameRunning': true,
    'DataCorePlugin.GameData.NewData.SpeedKmh': 123,
    'DataCorePlugin.GameData.NewData.Gear': 'N',
    'DataCorePlugin.GameData.NewData.CurrentLapTime': '00:03:02.050' });
  assert.equal(result[3], '123'); assert.equal(result[4], 'N'); assert.equal(result[8], '03:02.050');
});
test('GameData properties used by the historical configuration remain supported', () => {
  const result = run({ 'DataCorePlugin.GameRunning': true,
    'DataCorePlugin.GameData.CurrentLap': 3,
    'DataCorePlugin.GameData.TotalLaps': 10,
    'DataCorePlugin.GameData.Throttle': 50 });
  assert.equal(result[13], '3'); assert.equal(result[14], '10'); assert.equal(result[16], '50');
});
test('live short fuel percent overrides a stale full property when capacity is unavailable', () => {
  const result = run({ FuelPercent: 47,
    'DataCorePlugin.GameData.NewData.FuelPercent': 72 });
  assert.equal(result[15], '47');
});
test('fuel percent falls back to fuel divided by tank capacity', () => {
  const result = run({ Fuel: 30, FuelCapacity: 60 });
  assert.equal(result[15], '50');
});
test('calculated fuel percent overrides a stale reported percentage', () => {
  const result = run({ Fuel: 24, FuelCapacity: 60,
    'DataCorePlugin.GameData.NewData.FuelPercent': 47 });
  assert.equal(result[15], '40');
});
test('formula forwards neutral for the firmware gear filter', () => {
  assert.equal(run({ Gear: 0 })[4], 'N');
});
