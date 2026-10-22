var Clay = require('@rebble/clay');

var DEFAULTS = {
  SETTING_LOOK: 0,
  SETTING_DATE: 1,
  SETTING_MOTION: 1,
  SETTING_BG: 0x0A1020,
  SETTING_FRONT: 0xF4F4DB,
  SETTING_DEPTH: 0x49B8D0,
  SETTING_GRID: 0x273759
};

function integer(value, min, max, fallback) {
  var n = Number(value);
  return isFinite(n) && Math.floor(n) === n && n >= min && n <= max ? n : fallback;
}

function settingsMessage(raw) {
  var saved;
  try { saved = JSON.parse(raw) || {}; } catch (error) { saved = {}; }
  return {
    SETTING_LOOK: integer(saved.SETTING_LOOK, 0, 5, DEFAULTS.SETTING_LOOK),
    SETTING_DATE: integer(saved.SETTING_DATE, 0, 3, DEFAULTS.SETTING_DATE),
    SETTING_MOTION: integer(saved.SETTING_MOTION, 0, 1, DEFAULTS.SETTING_MOTION),
    SETTING_BG: integer(saved.SETTING_BG, 0, 0xFFFFFF, DEFAULTS.SETTING_BG),
    SETTING_FRONT: integer(saved.SETTING_FRONT, 0, 0xFFFFFF, DEFAULTS.SETTING_FRONT),
    SETTING_DEPTH: integer(saved.SETTING_DEPTH, 0, 0xFFFFFF, DEFAULTS.SETTING_DEPTH),
    SETTING_GRID: integer(saved.SETTING_GRID, 0, 0xFFFFFF, DEFAULTS.SETTING_GRID)
  };
}

function configuration() {
  return [
    {type: 'heading', defaultValue: 'Gridcore'},
    {type: 'text', defaultValue: 'Wrist tilt shifts the depth and its three-color shading. Turn Live tilt parallax off for a static face; a tap briefly reveals the structure when it is on.'},
    {type: 'select', messageKey: 'SETTING_LOOK', label: 'Look', defaultValue: '0', options: [
      {label: 'Color Slices', value: '0'},
      {label: 'Lift', value: '1'},
      {label: 'Tunnel', value: '2'},
      {label: 'Daylight Blueprint', value: '3'},
      {label: 'Graphite', value: '4'},
      {label: 'Custom colors', value: '5'}
    ]},
    {type: 'select', messageKey: 'SETTING_DATE', label: 'Date', defaultValue: '1', options: [
      {label: 'Off', value: '0'},
      {label: 'YYYY-MM-DD', value: '1'},
      {label: 'MM/DD/YYYY', value: '2'},
      {label: 'DD/MM/YYYY', value: '3'}
    ]},
    {type: 'toggle', messageKey: 'SETTING_MOTION', label: 'Live tilt parallax', defaultValue: true},
    {type: 'heading', defaultValue: 'Custom colors'},
    {type: 'text', defaultValue: 'Choose Custom colors above to use these four picks.'},
    {type: 'color', messageKey: 'SETTING_BG', label: 'Background', defaultValue: '0A1020', sunlight: false, allowGray: true},
    {type: 'color', messageKey: 'SETTING_FRONT', label: 'Digit front', defaultValue: 'F4F4DB', sunlight: false, allowGray: true},
    {type: 'color', messageKey: 'SETTING_DEPTH', label: 'Digit depth', defaultValue: '49B8D0', sunlight: false, allowGray: true},
    {type: 'color', messageKey: 'SETTING_GRID', label: 'Grid', defaultValue: '273759', sunlight: false, allowGray: true},
    {type: 'submit', defaultValue: 'Save to watch'}
  ];
}

var clay = new Clay(configuration(), null, {autoHandleEvents: false});

function send(raw) {
  Pebble.sendAppMessage(settingsMessage(raw), null, function (error) {
    console.log('Gridcore settings send failed: ' + JSON.stringify(error));
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
  catch (error) { console.log('Gridcore settings rejected: ' + error); return; }
  send(localStorage.getItem('clay-settings'));
});

if (typeof module !== 'undefined') {
  module.exports = {settingsMessage: settingsMessage, configuration: configuration};
}
