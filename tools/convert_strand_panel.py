#!/usr/bin/env python3
"""Crop Strand's full-panel source to the 3 HP panel aspect, preserving height."""

from pathlib import Path

from PIL import Image


ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "res/Strand/Strand-F3.png"
DESTINATION = ROOT / "res/Strand/Strand-Panel.png"


def main():
    with Image.open(SOURCE) as source:
        width = round(source.height * 15.24 / 128.5)
        if width > source.width:
            raise ValueError("Source is narrower than the Strand panel aspect")
        left = (source.width - width) // 2
        cropped = source.crop((left, 0, left + width, source.height)).convert("RGB")
        cropped.save(DESTINATION, optimize=True)
        print(f"{SOURCE.name}: {source.size}, crop x=[{left}, {left + width}) -> {cropped.size}")


if __name__ == "__main__":
    main()
