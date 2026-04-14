from PIL import Image
import sys
import os

def crop_top_left_half(input_path, output_path=None):
    # Open image
    img = Image.open(input_path)
    width, height = img.size

    # Compute half dimensions
    new_width = width // 2
    new_height = height // 2

    # Define crop box: (left, upper, right, lower)
    crop_box = (0, 0, new_width, new_height)

    # Perform crop
    cropped_img = img.crop(crop_box)

    # Determine output path
    if output_path is None:
        base, ext = os.path.splitext(input_path)
        output_path = f"{base}_cropped{ext}"

    # Save result
    cropped_img.save(output_path)
    print(f"Cropped image saved to: {output_path}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python crop_half.py <input_image> [output_image]")
        sys.exit(1)

    input_image = sys.argv[1]
    output_image = sys.argv[2] if len(sys.argv) > 2 else None

    crop_top_left_half(input_image, output_image)