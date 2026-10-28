const assert = require('node:assert/strict');

const clayId = require.resolve('@rebble/clay');
require.cache[clayId] = {
  id: clayId, filename: clayId, loaded: true,
  exports: class Clay {
    generateUrl() { return 'way-back-settings'; }
    getSettings(response) { const values = JSON.parse(decodeURIComponent(response)); stored = JSON.stringify(values); return values; }
  }
};
let stored = null;
const events = {}, sent = [];
let opened;
global.Pebble = {
  addEventListener: (name, handler) => { events[name] = handler; },
  sendAppMessage: message => { sent.push(message); },
  openURL: url => { opened = url; }
};
global.localStorage = {getItem: () => stored, setItem: (key, value) => { stored = value; }};

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
assert.equal(old.SETTING_DECORATION, 0);
assert.equal(old.SETTING_FONT, 0);
assert.equal(old.SETTING_BACKGROUND, 0);
assert.equal(old.SETTING_ECHOES, 3);
assert.equal(old.SETTING_ECHO_REVERSE, 0);
assert.equal(old.SETTING_MOTION_MODE, 2);
assert.equal(old.SETTING_MOTION, 1);

const legacyOff = settings.settingsMessage(JSON.stringify({SETTING_MOTION: 0}));
const legacyOn = settings.settingsMessage(JSON.stringify({SETTING_MOTION: 1}));
assert.equal(legacyOff.SETTING_MOTION_MODE, 0);
assert.equal(legacyOn.SETTING_MOTION_MODE, 2);
assert.equal(JSON.parse(settings.migrateStoredSettings(JSON.stringify({SETTING_MOTION: 0}))).SETTING_MOTION_MODE, 0);
assert.equal(JSON.parse(settings.migrateStoredSettings(JSON.stringify({SETTING_MOTION: 1}))).SETTING_MOTION_MODE, 2);

const tilt = settings.settingsMessage(JSON.stringify({SETTING_MOTION: 0, SETTING_MOTION_MODE: 1}));
assert.equal(tilt.SETTING_MOTION_MODE, 1);
assert.equal(tilt.SETTING_MOTION, 1);
assert.equal(settings.settingsMessage(JSON.stringify({SETTING_MOTION_MODE: 0})).SETTING_MOTION_MODE, 0);
assert.equal(settings.settingsMessage(JSON.stringify({SETTING_MOTION_MODE: 2})).SETTING_MOTION_MODE, 2);

const custom = settings.settingsMessage(JSON.stringify({
  SETTING_MODE: 1,
  SETTING_NIGHT_START: 22,
  SETTING_DAY_START: 5,
  SETTING_NIGHT_BG: 0x010203,
  SETTING_NIGHT_FRONT: 0xEEEEEE,
  SETTING_NIGHT_DEPTH: 0x336699,
  SETTING_NIGHT_GRID: 0x111111,
  SETTING_DECORATION: 5,
  SETTING_FONT: 11,
  SETTING_BACKGROUND: 10,
  SETTING_ECHOES: 12,
  SETTING_ECHO_REVERSE: 1
}));
assert.equal(custom.SETTING_MODE, 1);
assert.equal(custom.SETTING_NIGHT_START, 22);
assert.equal(custom.SETTING_DAY_START, 5);
assert.equal(custom.SETTING_NIGHT_BG, 0x010203);
assert.equal(custom.SETTING_NIGHT_FRONT, 0xEEEEEE);
assert.equal(custom.SETTING_NIGHT_DEPTH, 0x336699);
assert.equal(custom.SETTING_NIGHT_GRID, 0x111111);
assert.equal(custom.SETTING_DECORATION, 5);
assert.equal(custom.SETTING_FONT, 11);
assert.equal(custom.SETTING_BACKGROUND, 10);
assert.equal(custom.SETTING_ECHOES, 12);
assert.equal(custom.SETTING_ECHO_REVERSE, 1);

const invalid = settings.settingsMessage(JSON.stringify({
  SETTING_MODE: 3,
  SETTING_NIGHT_START: 9,
  SETTING_DAY_START: 21,
  SETTING_NIGHT_BG: -1,
  SETTING_DECORATION: 8,
  SETTING_FONT: 12,
  SETTING_BACKGROUND: 12,
  SETTING_ECHOES: 13,
  SETTING_ECHO_REVERSE: 2,
  SETTING_MOTION_MODE: 3
}));
assert.equal(invalid.SETTING_MODE, 2);
assert.equal(invalid.SETTING_NIGHT_START, 19);
assert.equal(invalid.SETTING_DAY_START, 7);
assert.equal(invalid.SETTING_NIGHT_BG, 0);
assert.equal(invalid.SETTING_DECORATION, 0);
assert.equal(invalid.SETTING_FONT, 0);
assert.equal(invalid.SETTING_BACKGROUND, 0);
assert.equal(invalid.SETTING_ECHOES, 3);
assert.equal(invalid.SETTING_ECHO_REVERSE, 0);
assert.equal(invalid.SETTING_MOTION_MODE, 2);

const config = settings.configuration();
assert.equal(config[0].defaultValue, 'Way Back');
const keys = new Set(config.map(function (item) { return item.messageKey; }).filter(Boolean));
for (const key of Object.keys(custom)) {
  if (key !== 'SETTING_MOTION') assert(keys.has(key), `missing Clay control: ${key}`);
}
assert(!keys.has('SETTING_MOTION'));
const motionControl = config.find((item) => item.messageKey === 'SETTING_MOTION_MODE');
assert.equal(motionControl.type, 'select');
assert.equal(motionControl.defaultValue, '2');
assert.deepEqual(motionControl.options.map((option) => option.value), ['0', '1', '2']);
assert.equal(config.find((item) => item.messageKey === 'SETTING_DECORATION').options.length, 8);
assert.equal(config.find((item) => item.messageKey === 'SETTING_FONT').options.length, 12);
assert.equal(config.find((item) => item.messageKey === 'SETTING_BACKGROUND').options.length, 12);
assert.equal(config.find((item) => item.messageKey === 'SETTING_ECHOES').options.length, 12);
assert.equal(config.find((item) => item.messageKey === 'SETTING_ECHO_REVERSE').defaultValue, false);
console.log('PASS: Way Back settings defaults, migration, validation, and controls');

assert.equal(old.SETTING_ORBIT_SPEED, 1);
assert.equal(old.SETTING_SENSITIVITY, 1);
for (const speed of [0, 1, 2]) {
  for (const sensitivity of [0, 1]) {
    const value = settings.settingsMessage(JSON.stringify({SETTING_ORBIT_SPEED: String(speed), SETTING_SENSITIVITY: String(sensitivity)}));
    assert.equal(value.SETTING_ORBIT_SPEED, speed);
    assert.equal(value.SETTING_SENSITIVITY, sensitivity);
  }
}
assert.equal(settings.settingsMessage(JSON.stringify({SETTING_ORBIT_SPEED: 3})).SETTING_ORBIT_SPEED, 1);
assert.equal(settings.settingsMessage(JSON.stringify({SETTING_SENSITIVITY: -1})).SETTING_SENSITIVITY, 1);
events.ready();
assert.equal(sent.length, 0);
stored = JSON.stringify({SETTING_ORBIT_SPEED: 2});
events.ready();
assert.equal(sent.length, 1);
assert.equal(sent[0].SETTING_PREVIEW, undefined);
events.showConfiguration();
assert.equal(opened, 'way-back-settings');
for (const event of [null, {}, {response: ''}, {response: 'CANCELLED'}, {response: 'invalid'}]) events.webviewclosed(event);
assert.equal(sent.length, 1);
events.webviewclosed({response: encodeURIComponent(JSON.stringify({SETTING_ORBIT_SPEED: '0', SETTING_SENSITIVITY: '0', SETTING_MOTION_MODE: '2'}))});
assert.equal(sent.length, 2);
assert.equal(sent[1].SETTING_PREVIEW, 1);
assert.equal(sent[1].SETTING_ORBIT_SPEED, 0);
assert.equal(sent[1].SETTING_SENSITIVITY, 0);
assert.equal(JSON.parse(stored).SETTING_PREVIEW, undefined);
events.ready();
assert.equal(sent[2].SETTING_PREVIEW, undefined);
console.log('PASS: speed, sensitivity, Save preview, Cancel, and startup events');
