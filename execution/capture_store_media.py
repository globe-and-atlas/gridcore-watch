"""Capture native Way Back store frames and assemble clockwise-orbit GIFs.

Build a fixed-time fixture PBW before running this script. Run one platform at a
time: --capture emery, then --capture gabbro. --compose builds the final media.
"""

from __future__ import annotations

import argparse
import json
import subprocess
import time
from pathlib import Path

from PIL import Image, ImageChops


ROOT = Path(__file__).resolve().parents[1]
WATCHFACE = ROOT / "watchface"
CAPTURES = ROOT / ".tmp" / "store-media"
MEDIA = ROOT / "media"
SIZES = {"emery": (200, 228), "gabbro": (260, 260)}
FRONT_COLORS = {
    "color-slices": (255, 255, 255),
    "amber-mesh": (255, 255, 170),
    "daylight-dots": (0, 0, 85),
    "graphite-rings": (255, 255, 255),
    "green-stars": (255, 255, 255),
}

# Clockwise in screen coordinates, starting above the fixed 13:48 numerals.
ANGLES = ((0, -3), (3, -3), (3, 0), (3, 3),
          (0, 3), (-3, 3), (-3, 0), (-3, -3))
COMPOSITIONS = (
    ("color-slices", {"SETTING_LOOK": 0, "SETTING_DECORATION": 0, "SETTING_FONT": 0, "SETTING_BACKGROUND": 0, "SETTING_ECHOES": 8}),
    ("amber-mesh", {"SETTING_LOOK": 2, "SETTING_DECORATION": 2, "SETTING_FONT": 1, "SETTING_BACKGROUND": 6, "SETTING_ECHOES": 5}),
    ("daylight-dots", {"SETTING_LOOK": 3, "SETTING_DECORATION": 1, "SETTING_FONT": 2, "SETTING_BACKGROUND": 3, "SETTING_ECHOES": 7}),
    ("graphite-rings", {"SETTING_LOOK": 4, "SETTING_DECORATION": 7, "SETTING_FONT": 9, "SETTING_BACKGROUND": 8, "SETTING_ECHOES": 5}),
    (
        "green-stars",
        {
            "SETTING_LOOK": 5, "SETTING_DECORATION": 4, "SETTING_FONT": 10,
            "SETTING_BACKGROUND": 10, "SETTING_ECHOES": 12,
            "SETTING_BG": 0x061913, "SETTING_FRONT": 0xE8F5D0,
            "SETTING_DEPTH": 0x4ABE94, "SETTING_GRID": 0x316D5C,
        },
    ),
)


def run_pebble(*args: str) -> None:
    result = subprocess.run(
        ["pebble", *args], cwd=WATCHFACE, text=True,
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, check=False,
        timeout=90,
    )
    if result.returncode:
        raise RuntimeError(f"pebble {' '.join(args[:2])} failed:\n{result.stdout}")


def has_complete_date(path: Path, look: int, platform: str) -> bool:
    with Image.open(path) as image:
        image = image.convert("RGB")
        if image.size != SIZES[platform]:
            return False
        width, height = image.size
        ink = 0
        for y in range(int(height * 0.75), int(height * 0.94)):
            for x in range(int(width * 0.25), int(width * 0.75)):
                rgb = image.getpixel((x, y))
                if look == 3:
                    ink += max(rgb) < 110
                else:
                    ink += min(rgb) > 125
        return ink > 90


def foreground_mask(image: Image.Image, color: tuple[int, int, int]) -> set[tuple[int, int]]:
    pixels = image.load()
    width, height = image.size
    # Exclude edge decorations, which can be occluded by a moving trail.
    return {
        (x, y)
        for y in range(int(height * 0.25), int(height * 0.74))
        for x in range(int(width * 0.22), int(width * 0.78))
        if pixels[x, y] == color
    }


def capture(platform: str, style: str | None = None) -> None:
    CAPTURES.mkdir(parents=True, exist_ok=True)
    message_keys = json.loads((WATCHFACE / "build" / "js" / "message_keys.json").read_text())
    for name, settings in COMPOSITIONS:
        if style and name != style:
            continue
        for index, (angle_x, angle_y) in enumerate(ANGLES):
            selected = {**settings, "SETTING_ECHO_REVERSE": 0, "SETTING_DATE": 1,
                        "SETTING_MODE": 0, 11000: angle_x, 11001: angle_y}
            pairs = [f"{message_keys[key] if isinstance(key, str) else key}={value}"
                     for key, value in selected.items()]
            run_pebble("send-app-message", "--emulator", platform, "--int", *pairs)
            output = CAPTURES / f"{platform}-{name}-orbit-{index:02d}.png"
            for attempt in range(8):
                time.sleep(0.35)
                run_pebble("screenshot", "--emulator", platform, "--no-open", "--no-correction", str(output))
                if has_complete_date(output, settings["SETTING_LOOK"], platform):
                    break
            else:
                raise RuntimeError(f"No complete native frame after 8 tries: {output}")
            print(f"Captured {output.name} (attempt {attempt + 1})")


def compose(platform: str) -> None:
    MEDIA.mkdir(parents=True, exist_ok=True)
    frames: list[Image.Image] = []
    for name, _ in COMPOSITIONS:
        orbit = []
        for index in range(len(ANGLES)):
            source = CAPTURES / f"{platform}-{name}-orbit-{index:02d}.png"
            if not source.is_file():
                raise FileNotFoundError(source)
            image = Image.open(source).convert("RGB")
            if image.size != SIZES[platform]:
                raise ValueError(f"Wrong capture size: {source}: {image.size}")
            orbit.append(image)
            frames.append(image)
        first_front = foreground_mask(orbit[0], FRONT_COLORS[name])
        if len(first_front) < 100:
            raise ValueError(f"Foreground missing: {platform}-{name}")
        for index, image in enumerate(orbit[1:], start=1):
            changed = ImageChops.difference(orbit[index - 1], image)
            changed_pixels = changed.width * changed.height - changed.convert("L").histogram()[0]
            if changed_pixels < 200:
                raise ValueError(f"Orbit step invisible: {platform}-{name}-{index}")
            if foreground_mask(image, FRONT_COLORS[name]) != first_front:
                raise ValueError(f"Foreground moved during orbit: {platform}-{name}-{index}")
        print(f"{platform}-{name}: eight visible orbit positions, fixed front")

    gif = MEDIA / f"way-back-{platform}-options.gif"
    frames[0].save(
        gif, save_all=True, append_images=frames[1:], duration=240,
        loop=0, optimize=True, disposal=2,
    )
    if gif.stat().st_size >= 4_400_000:
        raise ValueError(f"GIF exceeds 4.4 MB: {gif.stat().st_size}")
    with Image.open(gif) as encoded:
        if encoded.n_frames != len(frames):
            raise ValueError(f"GIF lost frames: {gif}")
        for style_index, (name, _) in enumerate(COMPOSITIONS):
            front = None
            for angle_index in range(len(ANGLES)):
                encoded.seek(style_index * len(ANGLES) + angle_index)
                current = foreground_mask(encoded.convert("RGB"), FRONT_COLORS[name])
                if front is None:
                    front = current
                elif front != current:
                    raise ValueError(f"Encoded GIF shifted foreground: {platform}-{name}-{angle_index}")
    print(f"Saved {gif.name}: {gif.stat().st_size} bytes")

    # Four stills plus the GIF fill the five listing slots for each platform.
    selections = (
        ("color-slices", 1), ("daylight-dots", 3),
        ("graphite-rings", 5), ("green-stars", 7),
    )
    for index, (name, angle_index) in enumerate(selections, start=1):
        source = CAPTURES / f"{platform}-{name}-orbit-{angle_index:02d}.png"
        output = MEDIA / f"way-back-{platform}-store-{index:02d}-{name}.png"
        output.write_bytes(source.read_bytes())
        print(f"Saved {output.name}")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--capture", choices=SIZES)
    parser.add_argument("--style", choices=[name for name, _ in COMPOSITIONS])
    parser.add_argument("--compose", action="store_true")
    args = parser.parse_args()
    if not args.capture and not args.compose:
        parser.error("choose --capture or --compose")
    if args.capture:
        capture(args.capture, args.style)
    if args.compose:
        for platform in SIZES:
            compose(platform)


if __name__ == "__main__":
    main()
