#!/usr/bin/env python3
"""
Generate a perfect 8x8 chessboard PNG.

Requirements:
    pip install pillow
"""

from pathlib import Path
from PIL import Image, ImageDraw


def generate_chessboard(
    output_path: str = "chessboard.png",
    square_size: int = 128,
    light_color: tuple[int, int, int] = (240, 217, 181),
    dark_color: tuple[int, int, int] = (181, 136, 99),
) -> None:
    """
    Generate an 8x8 chessboard image with exact square dimensions.

    Args:
        output_path: Output PNG filename.
        square_size: Width/height of each square in pixels.
        light_color: RGB color for light squares.
        dark_color: RGB color for dark squares.
    """
    board_size = 8
    image_size = board_size * square_size

    image = Image.new("RGB", (image_size, image_size), light_color)
    draw = ImageDraw.Draw(image)

    for row in range(board_size):
        for col in range(board_size):
            color = light_color if (row + col) % 2 == 0 else dark_color
            x0 = col * square_size
            y0 = row * square_size
            x1 = x0 + square_size
            y1 = y0 + square_size
            draw.rectangle([x0, y0, x1, y1], fill=color)

    image.save(output_path, "PNG")
    print(f"Saved chessboard to: {Path(output_path).resolve()}")


if __name__ == "__main__":
    generate_chessboard(
        output_path="chessboard.png",
        square_size=128,  # 8 * 128 = 1024x1024 final image
        light_color=(240, 217, 181),
        dark_color=(181, 136, 99),
    )