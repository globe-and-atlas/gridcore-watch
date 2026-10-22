---
generated_by: "OpenAI Codex (GPT-6.1-sol)"
timestamp: "2026-10-22T10:01:05-05:00"
---
# Dependencies

- Pebble SDK 4.33.1 compiles the C watchface for both Emery and Gabbro.
- `@rebble/clay` is pinned to 1.1.0 in `package.json` and `package-lock.json` for the phone configuration page.
- Clay's package expects generated `tmp/config-page.html` during its build path. Direct Node `require('@rebble/clay')` without that generated file fails; the Pebble build succeeds. To unit-check Way Back's settings parser in Node, replace the Clay import in `require.cache` with a stub before loading `src/pkjs/index.js`.
- Cormorant Garamond, Bodoni Moda, Space Grotesk, and Libre Baskerville were sourced from the Google Fonts repository on 2026-10-22 under their bundled SIL Open Font Licenses. Static 600/700 weight instances were subset to clock characters using fontTools 4.62.1 for the Pebble resource budget. Libre Baskerville reserves its font name; its modified subset retains the internal family Gridcore Book and appears as Book Serif in Way Back's settings. The watch app loads the resulting 54-point resources at startup.
