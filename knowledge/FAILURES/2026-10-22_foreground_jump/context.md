---
generated_by: "OpenAI Codex (GPT-6.1-sol)"
timestamp: "2026-10-22T15:40:28-05:00"
---
# Foreground jumped when echo direction changed

## Attempt

Way Back 0.6.0 reversed the trail and shifted the foreground by half the direction-dependent depth to keep the combined silhouette centered. The initial paired store GIFs used that renderer.

## Why it failed

The main time moved when the reverse toggle changed. The user expected only the receding echoes to change direction. The foreground shift was visible across GIF pairs despite both frames using the same time and simulated wrist angle.

## Correct path

Center the foreground time without using echo direction, reach, or tilt. Apply the reverse sign only to echo offsets. Version 0.6.1 uses this layout and regenerated native Emery and Gabbro store media. The media workflow now rejects pairs with different foreground numeral masks, including after GIF encoding. A live Gabbro tilt comparison changed 3,521 pixels while the foreground mask changed zero.

## Release boundary

The corrected bundle and media are local. Physical-watch checks and RePebble upload remain open.
