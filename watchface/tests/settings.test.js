const assert = require('node:assert/strict');

const clayId = require.resolve('@rebble/clay');
require.cache[clayId] = {
  id: clayId, filename: clayId, loaded: true,
  exports: class Clay {}
};
global.Pebble = {addEventListener: function () {}};
global.localStorage = {getItem: function () { return null; }};

const settings = require('../src/pkjs/index.js');
const old = settings.settingsMessage(JSON.stringify({
  SETTING_LOOK: 5,
  SETTING_BG: 0x123456,
  SETTING_DATE: 3
}));
assert.equal(old.SETTING_BG, 0x123456);
assert.equal(old.SETTING_DATE, 3);
assert.equal(old.SETTING_MODE, 2);
assert.equal(old.SETTING_NIGHT_START, 19);
assert.equal(old.SETTING_DAY_START, 7);

const custom = settings.settingsMessage(JSON.stringify({
  SETTING_MODE: 1,
  SETTING_NIGHT_START: 22,
  SETTING_DAY_START: 5,
  SETTING_NIGHT_BG: 0x010203,
  SETTING_NIGHT_FRONT: 0xEEEEEE,
  SETTING_NIGHT_DEPTH: 0x336699,
  SETTING_NIGHT_GRID: 0x111111
}));
assert.equal(custom.SETTING_MODE, 1);
assert.equal(custom.SETTING_NIGHT_START, 22);
assert.equal(custom.SETTING_DAY_START, 5);
assert.equal(custom.SETTING_NIGHT_BG, 0x010203);
assert.equal(custom.SETTING_NIGHT_FRONT, 0xEEEEEE);
assert.equal(custom.SETTING_NIGHT_DEPTH, 0x336699);
assert.equal(custom.SETTING_NIGHT_GRID, 0x111111);

const invalid = settings.settingsMessage(JSON.stringify({
  SETTING_MODE: 3,
  SETTING_NIGHT_START: 9,
  SETTING_DAY_START: 21,
  SETTING_NIGHT_BG: -1
}));
assert.equal(invalid.SETTING_MODE, 2);
assert.equal(invalid.SETTING_NIGHT_START, 19);
assert.equal(invalid.SETTING_DAY_START, 7);
assert.equal(invalid.SETTING_NIGHT_BG, 0);

const config = settings.configuration();
const keys = new Set(config.map(function (item) { return item.messageKey; }).filter(Boolean));
for (const key of Object.keys(custom)) assert(keys.has(key), `missing Clay control: ${key}`);
console.log('PASS: Gridcore settings defaults, migration, validation, and controls');
