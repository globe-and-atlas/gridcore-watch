# Decisions

## Initial Decision

- Template profile: `workflow-python`
- Deploy target: `local-only`
- Runtime: `pebble`

## 2026-10-22 — Native vector font and optional motion

- Use seven solid chamfered segments for the clock and draw their colored depth with Pebble graphics primitives. This matches the selected digital font reference while keeping the geometry adjustable for both screen shapes.
- The original reveal was opt-in. A five-second cooldown bounds repeated wrist triggers, and disabling motion unsubscribes accelerometer sampling.
- Keep a single PBW targeting Emery and Gabbro; account for the round viewport in the common renderer.

## 2026-10-22 — Live tilt parallax

- Make the tilt setting on by default for new installs because the depth motion is central to this face; retain a static setting for people who prefer it. Existing saved choices remain in effect.
- Quantize filtered accelerometer x/y into seven positions per axis, with 50 mG hysteresis on each side of a boundary and redraw only when a position changes. This limits jitter and redraw activity at rest.
- Keep the digit front fixed and move the rear extrusion up to three pixels with three stepped colors. The accelerometer measures wrist orientation relative to gravity, not the viewer's eye position.

## 2026-10-22 — Day and night settings

- Preserve the existing four custom color values as the day palette, then add four independent night values. This keeps earlier saved color choices intact.
- Offer Day, Night, and Auto. Auto uses the watch's local hour, with configurable night start (6–10 p.m.) and day start (5–9 a.m.); defaults are 7 p.m. and 7 a.m. The minute tick already redraws the face at those boundaries.
- Give each built-in look a night palette so the mode works outside Custom colors. Keep the night grid visible after quantization to Pebble's 64-color palette.

## 2026-10-22 — Numerals and decorations

- Keep the existing segmented clock as the default while offering eleven alternate numeral styles. Pebble system fonts cover several digital choices; four SIL Open Font License assets add larger geometric and classic silhouettes. Their embedded resources contain only digits and colon.
- Draw the depth trail before a fixed foreground for every numeral choice. Text faces use two offset color bands to balance depth with legibility at 200 pixels wide.
- Keep the measured grid independent from eight selectable decorations. Use a circular ring for the full-frame option on Gabbro.
