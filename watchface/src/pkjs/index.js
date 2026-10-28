var Clay = require('@rebble/clay');

var DEFAULTS = {
  SETTING_ORBIT_SPEED: 1,
  SETTING_SENSITIVITY: 1,
  SETTING_LOOK: 0,
  SETTING_DATE: 1,
  SETTING_MOTION: 1,
  SETTING_MOTION_MODE: 2,
  SETTING_BG: 0x0A1020,
  SETTING_FRONT: 0xF4F4DB,
  SETTING_DEPTH: 0x49B8D0,
  SETTING_GRID: 0x273759,
  SETTING_MODE: 2,
  SETTING_NIGHT_START: 19,
  SETTING_DAY_START: 7,
  SETTING_NIGHT_BG: 0x000000,
  SETTING_NIGHT_FRONT: 0xC0C8D0,
  SETTING_NIGHT_DEPTH: 0x336688,
  SETTING_NIGHT_GRID: 0x273759,
  SETTING_DECORATION: 0,
  SETTING_FONT: 0,
  SETTING_BACKGROUND: 0,
  SETTING_ECHOES: 3,
  SETTING_ECHO_REVERSE: 0
};

function integer(value, min, max, fallback) {
  var n = Number(value);
  return isFinite(n) && Math.floor(n) === n && n >= min && n <= max ? n : fallback;
}

function motionMode(saved) {
  if (Object.prototype.hasOwnProperty.call(saved, 'SETTING_MOTION_MODE')) {
    return integer(saved.SETTING_MOTION_MODE, 0, 2, DEFAULTS.SETTING_MOTION_MODE);
  }
  return integer(saved.SETTING_MOTION, 0, 1, DEFAULTS.SETTING_MOTION) === 0 ? 0 : 2;
}

function migrateStoredSettings(raw) {
  if (!raw) return raw;
  var saved;
  try { saved = JSON.parse(raw); } catch (error) { return raw; }
  if (!saved || typeof saved !== 'object' ||
      !Object.prototype.hasOwnProperty.call(saved, 'SETTING_MOTION') ||
      Object.prototype.hasOwnProperty.call(saved, 'SETTING_MOTION_MODE')) return raw;
  saved.SETTING_MOTION_MODE = motionMode(saved);
  return JSON.stringify(saved);
}

function settingsMessage(raw) {
  var saved;
  try { saved = JSON.parse(raw) || {}; } catch (error) { saved = {}; }
  var selectedMotion = motionMode(saved);
  return {
    SETTING_ORBIT_SPEED: integer(saved.SETTING_ORBIT_SPEED, 0, 2, DEFAULTS.SETTING_ORBIT_SPEED),
    SETTING_SENSITIVITY: integer(saved.SETTING_SENSITIVITY, 0, 1, DEFAULTS.SETTING_SENSITIVITY),
    SETTING_LOOK: integer(saved.SETTING_LOOK, 0, 5, DEFAULTS.SETTING_LOOK),
    SETTING_DATE: integer(saved.SETTING_DATE, 0, 3, DEFAULTS.SETTING_DATE),
    SETTING_MOTION: selectedMotion === 0 ? 0 : 1,
    SETTING_MOTION_MODE: selectedMotion,
    SETTING_BG: integer(saved.SETTING_BG, 0, 0xFFFFFF, DEFAULTS.SETTING_BG),
    SETTING_FRONT: integer(saved.SETTING_FRONT, 0, 0xFFFFFF, DEFAULTS.SETTING_FRONT),
    SETTING_DEPTH: integer(saved.SETTING_DEPTH, 0, 0xFFFFFF, DEFAULTS.SETTING_DEPTH),
    SETTING_GRID: integer(saved.SETTING_GRID, 0, 0xFFFFFF, DEFAULTS.SETTING_GRID),
    SETTING_MODE: integer(saved.SETTING_MODE, 0, 2, DEFAULTS.SETTING_MODE),
    SETTING_NIGHT_START: integer(saved.SETTING_NIGHT_START, 18, 22, DEFAULTS.SETTING_NIGHT_START),
    SETTING_DAY_START: integer(saved.SETTING_DAY_START, 5, 9, DEFAULTS.SETTING_DAY_START),
    SETTING_NIGHT_BG: integer(saved.SETTING_NIGHT_BG, 0, 0xFFFFFF, DEFAULTS.SETTING_NIGHT_BG),
    SETTING_NIGHT_FRONT: integer(saved.SETTING_NIGHT_FRONT, 0, 0xFFFFFF, DEFAULTS.SETTING_NIGHT_FRONT),
    SETTING_NIGHT_DEPTH: integer(saved.SETTING_NIGHT_DEPTH, 0, 0xFFFFFF, DEFAULTS.SETTING_NIGHT_DEPTH),
    SETTING_NIGHT_GRID: integer(saved.SETTING_NIGHT_GRID, 0, 0xFFFFFF, DEFAULTS.SETTING_NIGHT_GRID),
    SETTING_DECORATION: integer(saved.SETTING_DECORATION, 0, 7, DEFAULTS.SETTING_DECORATION),
    SETTING_FONT: integer(saved.SETTING_FONT, 0, 11, DEFAULTS.SETTING_FONT),
    SETTING_BACKGROUND: integer(saved.SETTING_BACKGROUND, 0, 11, DEFAULTS.SETTING_BACKGROUND),
    SETTING_ECHOES: integer(saved.SETTING_ECHOES, 1, 12, DEFAULTS.SETTING_ECHOES),
    SETTING_ECHO_REVERSE: integer(saved.SETTING_ECHO_REVERSE, 0, 1, DEFAULTS.SETTING_ECHO_REVERSE)
  };
}

function configuration() {
  return [
    {type: 'heading', defaultValue: 'Way Back'},
    {type: 'text', defaultValue: 'Choose a static echo, one that follows your wrist angle, or a full clockwise orbit on a deliberate tilt or tap. The orbit settles at your current tilt. The time digits stay fixed.'},
    {type: 'select', messageKey: 'SETTING_LOOK', label: 'Look', defaultValue: '0', options: [
      {label: 'Color Slices', value: '0'},
      {label: 'Lift', value: '1'},
      {label: 'Tunnel', value: '2'},
      {label: 'Daylight Blueprint', value: '3'},
      {label: 'Graphite', value: '4'},
      {label: 'Custom colors', value: '5'}
    ]},
    {type: 'select', messageKey: 'SETTING_BACKGROUND', label: 'Background pattern', defaultValue: '0', options: [
      {label: 'Blueprint grid', value: '0'},
      {label: 'Fine graph', value: '1'},
      {label: 'Wide graph', value: '2'},
      {label: 'Dot matrix', value: '3'},
      {label: 'Crosshair field', value: '4'},
      {label: 'Diagonal lines', value: '5'},
      {label: 'Diamond mesh', value: '6'},
      {label: 'Scanlines', value: '7'},
      {label: 'Radar rings', value: '8'},
      {label: 'Contour frames', value: '9'},
      {label: 'Starfield', value: '10'},
      {label: 'Solid', value: '11'}
    ]},
    {type: 'select', messageKey: 'SETTING_DATE', label: 'Date', defaultValue: '1', options: [
      {label: 'Off', value: '0'},
      {label: 'YYYY-MM-DD', value: '1'},
      {label: 'MM/DD/YYYY', value: '2'},
      {label: 'DD/MM/YYYY', value: '3'}
    ]},
    {type: 'select', messageKey: 'SETTING_DECORATION', label: 'Corner decoration', defaultValue: '0', options: [
      {label: 'Registration corners', value: '0'},
      {label: 'Short brackets', value: '1'},
      {label: 'Double brackets', value: '2'},
      {label: 'Crosshairs', value: '3'},
      {label: 'Edge ticks', value: '4'},
      {label: 'Dots', value: '5'},
      {label: 'Full frame', value: '6'},
      {label: 'None', value: '7'}
    ]},
    {type: 'select', messageKey: 'SETTING_FONT', label: 'Time numerals', defaultValue: '0', options: [
      {label: 'Prism segments', value: '0'},
      {label: 'Open wire segments', value: '1'},
      {label: 'Leco bold', value: '2'},
      {label: 'Leco regular', value: '3'},
      {label: 'Leco compact', value: '4'},
      {label: 'Roboto bold', value: '5'},
      {label: 'Bitham bold', value: '6'},
      {label: 'Bitham light', value: '7'},
      {label: 'Cormorant Garamond', value: '8'},
      {label: 'Bodoni Moda', value: '9'},
      {label: 'Space Grotesk', value: '10'},
      {label: 'Book Serif', value: '11'}
    ]},
    {type: 'select', messageKey: 'SETTING_ECHOES', label: 'Number echoes', defaultValue: '3', options: [
      {label: '1', value: '1'}, {label: '2', value: '2'},
      {label: '3', value: '3'}, {label: '4', value: '4'},
      {label: '5', value: '5'}, {label: '6', value: '6'},
      {label: '7', value: '7'}, {label: '8', value: '8'},
      {label: '9', value: '9'}, {label: '10', value: '10'},
      {label: '11', value: '11'}, {label: '12', value: '12'}
    ]},
    {type: 'toggle', messageKey: 'SETTING_ECHO_REVERSE', label: 'Reverse echo direction', defaultValue: false},
    {type: 'select', messageKey: 'SETTING_MODE', label: 'Day / night', defaultValue: '2', options: [
      {label: 'Auto by time', value: '2'},
      {label: 'Always day', value: '0'},
      {label: 'Always night', value: '1'}
    ]},
    {type: 'select', messageKey: 'SETTING_NIGHT_START', label: 'Night starts', defaultValue: '19', options: [
      {label: '6 PM', value: '18'}, {label: '7 PM', value: '19'},
      {label: '8 PM', value: '20'}, {label: '9 PM', value: '21'},
      {label: '10 PM', value: '22'}
    ]},
    {type: 'select', messageKey: 'SETTING_DAY_START', label: 'Day starts', defaultValue: '7', options: [
      {label: '5 AM', value: '5'}, {label: '6 AM', value: '6'},
      {label: '7 AM', value: '7'}, {label: '8 AM', value: '8'},
      {label: '9 AM', value: '9'}
    ]},
    {type: 'select', messageKey: 'SETTING_MOTION_MODE', label: 'Echo movement', defaultValue: '2', options: [
      {label: 'None', value: '0'},
      {label: 'Follow tilt', value: '1'},
      {label: 'Full clockwise orbit', value: '2'}
    ]},
    {type: 'select', messageKey: 'SETTING_ORBIT_SPEED', label: 'Orbit speed', defaultValue: '1', options: [
      {label: 'Quick', value: '0'}, {label: 'Classic', value: '1'}, {label: 'Slow', value: '2'}
    ]},
    {type: 'select', messageKey: 'SETTING_SENSITIVITY', label: 'Gesture sensitivity', defaultValue: '1', options: [
      {label: 'Gentle', value: '0'}, {label: 'Deliberate', value: '1'}
    ]},
    {type: 'text', defaultValue: 'Save previews one orbit when Full clockwise orbit is selected. Follow tilt responds to your wrist; None stays still. Sensitivity controls the tilt trigger, while a tap triggers an orbit in Full clockwise orbit mode.'},
    {type: 'heading', defaultValue: 'Custom day colors'},
    {type: 'text', defaultValue: 'Choose Custom colors above to use these colors. Day and night picks are independent.'},
    {type: 'color', messageKey: 'SETTING_BG', label: 'Day background', defaultValue: '0A1020', sunlight: false, allowGray: true},
    {type: 'color', messageKey: 'SETTING_FRONT', label: 'Day digit front', defaultValue: 'F4F4DB', sunlight: false, allowGray: true},
    {type: 'color', messageKey: 'SETTING_DEPTH', label: 'Day digit depth', defaultValue: '49B8D0', sunlight: false, allowGray: true},
    {type: 'color', messageKey: 'SETTING_GRID', label: 'Day grid', defaultValue: '273759', sunlight: false, allowGray: true},
    {type: 'heading', defaultValue: 'Custom night colors'},
    {type: 'color', messageKey: 'SETTING_NIGHT_BG', label: 'Night background', defaultValue: '000000', sunlight: false, allowGray: true},
    {type: 'color', messageKey: 'SETTING_NIGHT_FRONT', label: 'Night digit front', defaultValue: 'C0C8D0', sunlight: false, allowGray: true},
    {type: 'color', messageKey: 'SETTING_NIGHT_DEPTH', label: 'Night digit depth', defaultValue: '336688', sunlight: false, allowGray: true},
    {type: 'color', messageKey: 'SETTING_NIGHT_GRID', label: 'Night grid', defaultValue: '273759', sunlight: false, allowGray: true},
    {type: 'submit', defaultValue: 'Save to watch'}
  ];
}

var savedBeforeClay = localStorage.getItem('clay-settings');
var migratedBeforeClay = migrateStoredSettings(savedBeforeClay);
if (migratedBeforeClay !== savedBeforeClay) {
  localStorage.setItem('clay-settings', migratedBeforeClay);
}
var clay = new Clay(configuration(), null, {autoHandleEvents: false});

function send(raw, preview) {
  var message = settingsMessage(raw);
  if (preview) message.SETTING_PREVIEW = 1;
  Pebble.sendAppMessage(message, null, function (error) {
    console.log('Way Back settings send failed: ' + JSON.stringify(error));
  });
}

Pebble.addEventListener('ready', function () {
  var saved = localStorage.getItem('clay-settings');
  if (saved) send(saved);
});

Pebble.addEventListener('showConfiguration', function () {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function (event) {
  if (!event || !event.response || event.response === 'CANCELLED') return;
  try { clay.getSettings(event.response); }
  catch (error) { console.log('Way Back settings rejected: ' + error); return; }
  send(localStorage.getItem('clay-settings'), true);
});

if (typeof module !== 'undefined') {
  module.exports = {settingsMessage: settingsMessage, configuration: configuration,
    migrateStoredSettings: migrateStoredSettings};
}
