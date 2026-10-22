---
generated_by: "OpenAI Codex (GPT-6.1-sol)"
timestamp: "2026-10-22T10:01:05-05:00"
---
# Dependencies

- Pebble SDK 4.33.1 compiles the C watchface for both Emery and Gabbro.
- `@rebble/clay` is pinned to 1.1.0 in `package.json` and `package-lock.json` for the phone configuration page.
- Clay's package expects generated `tmp/config-page.html` during its build path. Direct Node `require('@rebble/clay')` without that generated file fails; the Pebble build succeeds. To unit-check Gridcore's settings parser in Node, replace the Clay import in `require.cache` with a stub before loading `src/pkjs/index.js`.
