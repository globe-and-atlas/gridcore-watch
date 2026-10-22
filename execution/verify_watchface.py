"""Check the release bundle against Way Back's two-watch package contract."""

from __future__ import annotations

import json
from pathlib import Path
from zipfile import ZipFile


ROOT = Path(__file__).resolve().parents[1]
PACKAGE = ROOT / "watchface" / "package.json"
BUNDLE = ROOT / "watchface" / "build" / "watchface.pbw"
TARGETS = ("emery", "gabbro")
MESSAGE_KEYS = (
    "SETTING_LOOK",
    "SETTING_DATE",
    "SETTING_MOTION",
    "SETTING_BG",
    "SETTING_FRONT",
    "SETTING_DEPTH",
    "SETTING_GRID",
    "SETTING_MODE",
    "SETTING_NIGHT_START",
    "SETTING_DAY_START",
    "SETTING_NIGHT_BG",
    "SETTING_NIGHT_FRONT",
    "SETTING_NIGHT_DEPTH",
    "SETTING_NIGHT_GRID",
    "SETTING_DECORATION",
    "SETTING_FONT",
    "SETTING_BACKGROUND",
    "SETTING_ECHOES",
    "SETTING_ECHO_REVERSE",
    "SETTING_MOTION_MODE",
)


def verify() -> None:
    package = json.loads(PACKAGE.read_text(encoding="utf-8"))
    expected = package["pebble"]
    assert expected["displayName"] == "Way Back"
    assert expected["uuid"] == "2062ace2-4528-4975-95b6-fc2cbf11de0d"
    assert tuple(expected["targetPlatforms"]) == TARGETS
    assert expected["watchapp"]["watchface"] is True
    assert tuple(expected["messageKeys"]) == MESSAGE_KEYS
    assert BUNDLE.is_file(), f"Missing bundle: {BUNDLE}"

    with ZipFile(BUNDLE) as archive:
        names = set(archive.namelist())
        appinfo = json.loads(archive.read("appinfo.json"))
        assert appinfo["name"] == "way-back-watch"
        assert appinfo["shortName"] == "Way Back"
        assert appinfo["longName"] == "Way Back"
        assert appinfo["versionLabel"] == package["version"]
        assert appinfo["uuid"] == expected["uuid"]
        assert tuple(appinfo["targetPlatforms"]) == TARGETS
        assert set(appinfo["messageKeys"]) == set(MESSAGE_KEYS)
        assert appinfo["watchapp"]["watchface"] is True
        assert "pebble-js-app.js" in names
        for target in TARGETS:
            binary = f"{target}/pebble-app.bin"
            manifest = f"{target}/manifest.json"
            assert binary in names, f"Missing {binary}"
            assert manifest in names, f"Missing {manifest}"
            assert len(archive.read(binary)) > 1024, f"Empty {binary}"
            json.loads(archive.read(manifest))

    print(f"PASS: Way Back {appinfo['versionLabel']} bundle for Emery and Gabbro")


if __name__ == "__main__":
    verify()
