---
generated_by: "OpenAI Codex (GPT-6.1-sol)"
timestamp: "2026-10-19T22:11:00-05:00"
---
# Way Back watchface

## Clockwise echo orbit — 2026-10-22

- [x] Wrist tilt can place the echo above the digits.
- [x] Wrist tilt can place the echo to the right of the digits.
- [x] Wrist tilt can place the echo below the digits.
- [x] Wrist tilt can place the echo to the left of the digits.
- [x] The reverse setting reverses the tilt displacement.
- [x] Foreground numeral pixels remain fixed through eight echo angles within each style.
- [x] Each platform GIF visits eight echo angles clockwise within each style.
- [x] Each platform GIF cycles through five font and background compositions.
- [x] Each platform has four native still screenshots.
- [x] The GIF includes a style with no corner decorations.
- [x] The production PBW contains Emery and Gabbro binaries.
- [x] Native production tilt captures demonstrate echo movement.
- [x] A fresh verifier reviews the final PBW and media.

## Contract
- Target directive: `directives/build_watchface.md`.
- Intended execution script: `execution/verify_watchface.py`.
- Expected artifact: `watchface/build/watchface.pbw`.
- Expected captures: `.tmp/emery.png` and `.tmp/gabbro.png`.
- Safety: keep `.env`, tokens, waf locks, and `.tmp/` out of Git.
- Safety: do not publish a new listing without a separate release check.

## Validation Contract
- [x] The PBW contains an Emery binary.
- [x] The PBW contains a Gabbro binary.
- [x] The default face shows Color Slices at 200×228.
- [x] The Round 2 face keeps the time inside its circular screen.
- [x] The time follows the watch's 12-hour setting.
- [x] The time follows the watch's 24-hour setting.
- [x] Lift is selectable.
- [x] Tunnel is selectable.
- [x] Color Slices is selectable.
- [x] Daylight Blueprint is selectable.
- [x] A grayscale look is selectable.
- [x] Background color is configurable.
- [x] Front digit color is configurable.
- [x] Depth color is configurable.
- [x] Grid color is configurable.
- [x] Date Off hides the date.
- [x] Date YYYY-MM-DD renders in that order.
- [x] Date MM/DD/YYYY renders in that order.
- [x] Date DD/MM/YYYY renders in that order.
- [x] Settings persist after watch restart.
- [x] The motion effect rests after a short reveal.
- [x] `python3 execution/verify_watchface.py` passes.
- [x] A fresh verifier reviews the final PBW against this contract.

## Progress
- [x] Create the project with the workspace scaffold.
- [x] Implement the watchface renderer.
- [x] Implement phone configuration.
- [x] Build both platforms.
- [x] Capture both native emulators.
- [x] Run independent verification.
- [x] Promote procedural knowledge.

## Tilt parallax update — 2026-10-22

- [x] Motion Off leaves the clock static when accelerometer samples arrive.
- [x] Motion On shifts only the digit depth in response to horizontal wrist tilt.
- [x] Motion On shifts only the digit depth in response to vertical wrist tilt.
- [x] Motion On changes the depth shade order in response to wrist tilt.
- [x] The foreground digit silhouette remains fixed during tilt.
- [x] A steady tilt stops triggering redraws.
- [x] The tilt effect stays inside the Emery screen.
- [x] The tilt effect stays inside the Gabbro circle.
- [x] A tap reveal returns to the current tilt pose within three seconds.
- [x] The production PBW contains Emery and Gabbro binaries.
- [x] A fresh verifier reviews the tilt implementation and native captures.

## Day and night settings — 2026-10-22

- [x] Day mode uses the day palette at 02:15.
- [x] Night mode uses the night palette at 14:15.
- [x] Auto mode uses the day palette after the selected day start.
- [x] Auto mode uses the night palette after the selected night start.
- [x] Custom day background is configurable.
- [x] Custom day digit front is configurable.
- [x] Custom day digit depth is configurable.
- [x] Custom day grid is configurable.
- [x] Custom night background is configurable.
- [x] Custom night digit front is configurable.
- [x] Custom night digit depth is configurable.
- [x] Custom night grid is configurable.
- [x] A saved day palette survives watch restart.
- [x] A saved night palette survives watch restart.
- [x] Date Off hides the date in day mode.
- [x] Date Off hides the date in night mode.
- [x] YYYY-MM-DD renders in night mode.
- [x] MM/DD/YYYY renders in night mode.
- [x] DD/MM/YYYY renders in night mode.
- [x] The dual-platform production PBW passes `python3 execution/verify_watchface.py`.
- [x] Native Emery day and night captures are reviewed.
- [x] Native Gabbro day and night captures are reviewed.
- [x] A fresh verifier reviews the final settings and native captures.

## Corner decorations — 2026-10-22

- [x] Registration corners render at native resolution.
- [x] Short brackets render at native resolution.
- [x] Double brackets render at native resolution.
- [x] Crosshairs render at native resolution.
- [x] Edge ticks render at native resolution.
- [x] Dots render at native resolution.
- [x] A full frame renders at native resolution.
- [x] None removes the decoration.
- [x] The decoration choice persists after restart.
- [x] The decoration choice works on Emery.
- [x] The decoration choice works on Gabbro.
- [x] Native screenshots show the eight choices.
- [x] The production PBW passes `python3 execution/verify_watchface.py`.
- [x] A fresh verifier reviews the final decoration choices.

## Numeral styles — 2026-10-22

- [x] Twelve numeral choices appear in phone settings.
- [x] Each numeral choice displays a readable 12:15 on Emery.
- [x] Each numeral choice displays a readable 12:15 on Gabbro.
- [x] Each numeral choice retains a fixed foreground during tilt.
- [x] Each numeral choice has a moving depth trail when motion is on.
- [x] The numeral choice persists after restart.
- [x] Native previews show the numeral choices.

## Background patterns — 2026-10-22

- [x] Blueprint grid renders at native resolution.
- [x] Fine graph renders at native resolution.
- [x] Wide graph renders at native resolution.
- [x] Dot matrix renders at native resolution.
- [x] Crosshair field renders at native resolution.
- [x] Diagonal lines render at native resolution.
- [x] Diamond mesh renders at native resolution.
- [x] Scanlines render at native resolution.
- [x] Radar rings render at native resolution.
- [x] Contour frames render at native resolution.
- [x] Starfield renders at native resolution.
- [x] Solid renders without a pattern.
- [x] Background pattern is selectable independently of the color look.
- [x] The selected background pattern persists after restart.
- [x] Each background renders at 200×228 on Emery.
- [x] Each background renders at 260×260 on Gabbro.
- [x] Twelve native previews show each background on Emery.
- [x] Twelve native previews show each background on Gabbro.
- [x] The production PBW passes `python3 execution/verify_watchface.py`.
- [x] A fresh verifier reviews the final PBW and previews.

## Segmented numeral optical balance — 2026-10-22

- [x] The digit 1 reaches the same visual top as 3 at 13:48.
- [x] The digit 4 reaches the same visual bottom as 8 at 13:48.
- [x] Top horizontal bars are shorter than the previous renderer.
- [x] Bottom horizontal bars are shorter than the previous renderer.
- [x] The digit 1 is centered within its numeral cell.
- [x] The time 13:48 is readable at 200×228 on Emery.
- [x] The time 13:48 is readable at 260×260 on Gabbro.
- [x] The alternate text font choices remain unaffected.
- [x] The production PBW passes `python3 execution/verify_watchface.py`.
- [x] A fresh verifier reviews the native before/after captures.

## Visible time centering — 2026-10-22

- [x] The visible 13:48 clock is horizontally centered on Emery.
- [x] The visible 13:48 clock is horizontally centered on Gabbro.
- [x] A time without a leading 1 remains centered on Emery.
- [x] A time without a leading 1 remains centered on Gabbro.
- [x] The date remains visible in the final Emery capture.
- [x] The date remains visible in the final Gabbro capture.
- [x] Six current looks render on Emery.
- [x] Six current looks render on Gabbro.
- [x] The production PBW passes the dual-target verifier.
- [x] A fresh verifier reviews the final captures.

## Selectable number echoes — 2026-10-22

- [x] Phone settings offer each count from 1 through 12.
- [x] An old saved configuration defaults to 3 echoes.
- [x] Invalid echo counts revert to 3.
- [x] The selected echo count persists after a watch restart.
- [x] One echo has a visibly shorter trail than three echoes on Emery.
- [x] Twelve echoes have a visibly deeper trail than three echoes on Emery.
- [x] Twelve echoes preserve a readable foreground on Emery.
- [x] Twelve echoes preserve a readable foreground on Gabbro.
- [x] Echo count affects the alternate text fonts.
- [x] The release PBW contains the echo setting on both targets.
- [x] A fresh verifier reviews the final echo captures.

## Way Back store publication and follow-up — 2026-10-22

- [ ] Check 12-echo redraw and wrist motion on a physical Pebble Time 2 after release.
- [ ] Check 12-echo redraw and wrist motion on a physical Pebble Round 2 after release.
- [ ] Check watch battery behavior with 12 echoes enabled after release.
- [x] Create an Emery GIF showing Color Slices, Tunnel, Daylight Blueprint, Graphite, and custom green.
- [x] Create a Gabbro GIF showing Color Slices, Tunnel, Daylight Blueprint, Graphite, and custom green.
- [x] Keep each GIF under the project's 4.4 MB media budget.
- [x] Choose no more than five image assets per target.
- [x] Prepare the Way Back listing description.
- [x] Upload the production PBW to the Pebble Developer Dashboard.
- [x] Confirm the saved Emery media on the public listing.
- [x] Confirm the saved Gabbro media on the public listing.
- [x] Confirm the public listing serves version 0.7.0 to both targets.
- [x] The public PBW matches the local production PBW byte for byte.
- [x] The public Emery GIF matches the local GIF byte for byte.
- [x] The public Gabbro GIF matches the local GIF byte for byte.
- [x] Each public still matches its local image pixels.
- [x] A fresh verifier reviews the public release.

## Way Back product rename — 2026-10-22

- [x] The PBW display name is Way Back.
- [x] The PBW UUID remains 2062ace2-4528-4975-95b6-fc2cbf11de0d.
- [x] The phone settings title is Way Back.
- [x] The project README names the watchface Way Back.
- [x] The release checklist names Way Back.
- [x] The package version identifies the renamed build.
- [x] The production PBW contains Emery and Gabbro binaries.
- [x] A fresh verifier reviews the final package identity.

## Reversible echo direction — 2026-10-22

- [x] Phone settings offer a reverse echo direction toggle.
- [x] Existing saved settings default to the current echo direction.
- [x] Reversing the setting flips the echo response to the same wrist tilt.
- [x] The foreground time stays fixed while the wrist tilts.
- [x] The selected direction persists after restart.
- [x] The setting works on Emery.
- [x] The setting works on Gabbro.
- [x] The final production PBW passes the package verifier.
- [x] A fresh verifier reviews the completed direction behavior.

## Way Back store media — 2026-10-22 (0.6.1 historical criteria)

The paired-direction GIF was replaced by the clockwise orbit GIF specified above.

- [x] The 0.6.1 Emery GIF showed both echo directions at a fixed wrist angle.
- [x] The 0.6.1 Gabbro GIF showed both echo directions at a fixed wrist angle.
- [x] Each GIF shows at least three numeral styles.
- [x] Each GIF shows at least three background patterns.
- [x] Each GIF remains below 4.4 MB.
- [x] Emery has four selected still screenshots from the GIF compositions.
- [x] Gabbro has four selected still screenshots from the GIF compositions.
- [x] Each final screenshot retains legible time and date at native resolution.
- [x] A fresh verifier reviews the final media.

## Fixed foreground during echo transitions — 2026-10-22

- [x] Foreground numeral coordinates are independent of echo direction.
- [x] Foreground numeral coordinates are independent of wrist tilt.
- [x] Echoes move to opposite sides at the same simulated tilt when direction changes.
- [x] Matched GIF frames retain the same foreground pixel mask.
- [x] Emery store media uses the corrected renderer.
- [x] Gabbro store media uses the corrected renderer.
- [x] The final production PBW passes the dual-target verifier.
- [x] A fresh verifier reviews the corrected animation.
