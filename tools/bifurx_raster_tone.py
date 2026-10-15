"""Deterministic color treatment shared by the Bifurx raster generators."""

from __future__ import annotations

import numpy as np
from PIL import Image
from pathlib import Path
import shutil
import subprocess
import tempfile


def _srgb_to_linear(value: np.ndarray) -> np.ndarray:
    return np.where(
        value <= 0.04045,
        value / 12.92,
        np.power((value + 0.055) / 1.055, 2.4),
    )


def _linear_to_srgb(value: np.ndarray) -> np.ndarray:
    return np.where(
        value <= 0.0031308,
        value * 12.92,
        1.055 * np.power(value, 1.0 / 2.4) - 0.055,
    )


def neutralize_light_panel(image: Image.Image, strength: float = 0.70) -> Image.Image:
    """Move the low-chroma panel material toward neutral gray.

    Relative luminance is retained. Saturated cyan, violet, and orange light
    accents are progressively protected between 0.12 and 0.32 sRGB chroma.
    Neutral black/white typography is unchanged by construction.
    """
    if not 0.0 <= strength <= 1.0:
        raise ValueError("neutral strength must be between 0 and 1")
    source = image.convert("RGB")
    if strength == 0.0:
        return source

    srgb = np.asarray(source, dtype=np.float32) / 255.0
    chroma = np.max(srgb, axis=2) - np.min(srgb, axis=2)
    protection = np.clip((chroma - 0.12) / 0.20, 0.0, 1.0)
    protection = protection * protection * (3.0 - 2.0 * protection)
    amount = strength * (1.0 - protection)

    linear = _srgb_to_linear(srgb)
    luminance = (
        0.2126 * linear[:, :, 0]
        + 0.7152 * linear[:, :, 1]
        + 0.0722 * linear[:, :, 2]
    )
    neutral = _linear_to_srgb(np.clip(luminance, 0.0, 1.0))
    result = srgb + amount[:, :, None] * (neutral[:, :, None] - srgb)
    encoded = np.clip(np.rint(result * 255.0), 0, 255).astype(np.uint8)
    return Image.fromarray(encoded, mode="RGB")


def save_runtime_png(image: Image.Image, destination: Path, colors: int) -> None:
    """Save a runtime PNG, using pngquant for perceptual palette selection.

    ``colors == 0`` deliberately preserves full color. Palette generation is
    never silently delegated to Pillow's median-cut fallback because that loses
    important Bifurx glow gradients at practical palette sizes.
    """
    destination.parent.mkdir(parents=True, exist_ok=True)
    if colors == 0:
        image.save(destination, format="PNG", optimize=True)
        return

    pngquant = shutil.which("pngquant")
    if not pngquant:
        raise RuntimeError(
            "pngquant is required for optimized Bifurx runtime assets; "
            "install it or pass --colors 0 for full-color output"
        )
    optipng = shutil.which("optipng")
    if not optipng:
        raise RuntimeError(
            "optipng is required for losslessly optimized Bifurx runtime assets; "
            "install it or pass --colors 0 for full-color output"
        )
    with tempfile.TemporaryDirectory(prefix="bifurx-png-") as temp_dir:
        source = Path(temp_dir) / "source.png"
        image.save(source, format="PNG", optimize=True)
        subprocess.run(
            [
                pngquant,
                "--force",
                "--speed",
                "1",
                "--output",
                str(destination),
                str(colors),
                str(source),
            ],
            check=True,
        )
        subprocess.run(
            [optipng, "-quiet", "-o2", str(destination)],
            check=True,
        )
