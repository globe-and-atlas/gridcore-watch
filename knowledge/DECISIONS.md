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

## 2026-10-22 — Background patterns

- Keep the existing 16-pixel Blueprint grid as the default so upgrades preserve the face's current appearance.
- Render 12 backgrounds from native Pebble drawing primitives. Reuse the active day, night, grayscale, or custom palette's grid color so pattern and color choices remain independent.
- Keep the pattern behind the time and date. Do not include a separate colored bitmap resource for each combination.

## 2026-10-22 — Segmented numeral optical balance

- Extend vertical strokes nearly to the cell edge when the digit has no cap at that edge. This gives `1` and `4` an apparent height comparable to capped digits such as `3` and `8`.
- Center the narrow `1` within its cell. Shorten and slightly thin the top and bottom cap segments so capped numerals carry less horizontal mass. Keep the 12 font choices and clock spacing unchanged.

## 2026-10-22 — Visible clock centering and selectable echoes

- Historical choice: center the visible clock, including its nominal extrusion, instead of only centering equal-width digit cells. Account for the narrow `1` at either end of the time. The depth-dependent part of this choice was superseded by the fixed-foreground decision below.
- Offer 1–12 echoes, defaulting to three for existing installations. Grow the maximum reach gradually to 21 pixels at twelve echoes, and use progressively quieter colors for distant layers above three echoes. Preserve a fixed foreground for both segmented and text numeral styles.

## 2026-10-22 — Product name

- Name the watchface Way Back. Keep its Pebble UUID so existing local installs receive an update under the new name. Retain the `gridcore-watch` repository path and internal font resource identifiers to avoid renaming build fixtures or altering the licensed font subset during a product copy change.

## 2026-10-22 — Echo direction and store media

- Add a persisted reverse echo direction toggle, off by default. Multiply both the static depth and wrist-driven offsets by the selected direction. The original depth-dependent group recentering was superseded by the fixed-foreground decision below.
- Use native fixed-time emulator captures for store media. Pair normal and reversed trails at the same simulated wrist angle, then vary numeral style, palette, and background between pairs. Keep the GIFs at native target resolution so every frame represents the watch renderer.

## 2026-10-22 — Fixed foreground during direction changes

- Center the foreground numerals without a depth offset. Reverse direction changes only the echo vector, so the front has the same pixels in paired frames. Keep optical compensation for a narrow `1`. Check identical front-color pixel masks across every matched store-media pair before accepting GIFs.

## 2026-10-22 — Clockwise echo orbit

- Historical choice: blend the resting diagonal echo toward the measured two-axis wrist angle. This can leave a left-wrist gesture limited to a small arc. The full-orbit decision below supersedes the claim that a physical tilt visits all directions.
- Replace the paired-direction store animation with eight clockwise native angles per style. Cycle through five styles, including a graphite style with decoration set to None. Require identical foreground masks and visible echo changes for every adjacent frame.

## 2026-10-22 — Full orbit on deliberate tilt

- A deliberate change on any accelerometer axis, including Z, starts one eight-step clockwise echo orbit. The user selected a complete sweep for each trigger after a physical left-wrist test exposed the limited angle range.
- Keep the foreground digits fixed. After the 1.44-second orbit, return the echoes to the filtered live tilt direction and set the gesture anchor to that position so a held wrist does not retrigger.
- Keep Motion Off and reverse direction. The former unsubscribes motion and cancels an orbit; the latter negates the echo vector.
- Drive listing frames through the fixture's actual orbit-step path so the GIF demonstrates the same eight positions as the watch.

## 2026-10-22 — Three echo movement choices

- Expose None, Follow tilt, and Full clockwise orbit as separate settings. None unsubscribes the accelerometer; Follow tilt continuously follows its filtered X/Y angle without starting an orbit; Full clockwise orbit retains the eight-step sweep and then follows the measured angle.
- Keep Full clockwise orbit as the default. Migrate existing Motion Off to None and Motion On to Full clockwise orbit in both Clay settings and watch persistence.
- Keep the old message key as a compatibility alias and append a new motion-mode key, preserving all existing AppMessage IDs.

## 2026-10-27 — Motion controls and Save preview

0.8.0 adds Quick 90 ms, Classic 180 ms, and Slow 300 ms orbit steps. Deliberate retains the 360 mG summed-axis trigger; Gentle uses 220. Persist both settings. Save carries an ephemeral preview marker, cancels an active orbit, and restarts only in Full clockwise orbit mode. Startup and Cancel omit the marker. None and Follow tilt keep their existing semantics. Store GIFs use three contrasting native looks with 1.8-second initial and 2.2-second settled holds; stills remain unchanged.
