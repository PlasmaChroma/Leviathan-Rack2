from PIL import Image
import numpy as np

def crop_and_square(input_path, output_path, bg_threshold=245, target_size=None):
    """
    Crops white borders and ensures 1:1 aspect ratio.

    Args:
        input_path (str): Input image path
        output_path (str): Output image path
        bg_threshold (int): Threshold for detecting "white" (0-255)
        target_size (int or None): If set, resize to (target_size x target_size)
    """

    img = Image.open(input_path).convert("RGB")
    arr = np.array(img)

    # Detect non-white pixels
    mask = np.any(arr < bg_threshold, axis=2)

    coords = np.argwhere(mask)

    if coords.size == 0:
        raise ValueError("Image appears fully white")

    y_min, x_min = coords.min(axis=0)
    y_max, x_max = coords.max(axis=0)

    # Crop to content
    cropped = img.crop((x_min, y_min, x_max + 1, y_max + 1))

    w, h = cropped.size

    # Make square via padding
    if w != h:
        size = max(w, h)
        square = Image.new("RGB", (size, size), (255, 255, 255))

        offset_x = (size - w) // 2
        offset_y = (size - h) // 2

        square.paste(cropped, (offset_x, offset_y))
    else:
        square = cropped

    # Optional resize
    if target_size:
        square = square.resize((target_size, target_size), Image.LANCZOS)

    square.save(output_path)
    print(f"Saved: {output_path}")


# Example usage
crop_and_square(
    input_path="Wood.png",
    output_path="output.png",
    bg_threshold=245,
    target_size=1024  # optional, remove if not needed
)