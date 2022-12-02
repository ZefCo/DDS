from math import ceil
import pathlib
import PIL
from PIL import Image, ImageFont, ImageDraw
import os
import shutil


PIL_GRAYSCALE = 'L'
PIL_WIDTH_INDEX = 0
PIL_HEIGHT_INDEX = 1
COMMON_MONO_FONT_FILENAMES = [
    'DejaVuSansMono.ttf',  # Linux
    'Consolas Mono.ttf',   # MacOS, I think
    'Consola.ttf',         # Windows, I think
]




def textfile_to_image(textfile_path):
    """Convert text file to a grayscale image.

    arguments:
    textfile_path - the content of this file will be converted to an image
    font_path - path to a font file (for example impact.ttf)
    """
    # parse the file into lines stripped of whitespace on the right side
    with open(textfile_path) as f:
        lines = tuple(line.rstrip() for line in f.readlines())

    # choose a font (you can see more detail in the linked library on github)
    font = None
    large_font = 20  # get better resolution with larger size
    for font_filename in COMMON_MONO_FONT_FILENAMES:
        try:
            font = ImageFont.truetype(font_filename, size=large_font)
            print(f'Using font "{font_filename}".')
            break
        except IOError:
            print(f'Could not load font "{font_filename}".')
    if font is None:
        font = ImageFont.load_default()
        print('Using default font.')

    # make a sufficiently sized background image based on the combination of font and lines
    font_points_to_pixels = lambda pt: round(pt * 96.0 / 72)
    margin_pixels = 20

    # height of the background image
    tallest_line = max(lines, key=lambda line: font.getsize(line)[PIL_HEIGHT_INDEX])
    max_line_height = font_points_to_pixels(font.getsize(tallest_line)[PIL_HEIGHT_INDEX])
    realistic_line_height = max_line_height * 0.8  # apparently it measures a lot of space above visible content
    image_height = int(ceil(realistic_line_height * len(lines) + 2 * margin_pixels))

    # width of the background image
    widest_line = max(lines, key=lambda s: font.getsize(s)[PIL_WIDTH_INDEX])
    max_line_width = font_points_to_pixels(font.getsize(widest_line)[PIL_WIDTH_INDEX])
    image_width = int(ceil(max_line_width + (2 * margin_pixels)))

    # draw the background
    background_color = 255  # white
    image = Image.new(PIL_GRAYSCALE, (image_width, image_height), color=background_color)
    draw = ImageDraw.Draw(image)

    # draw each line of text
    font_color = 0  # black
    horizontal_position = margin_pixels
    for i, line in enumerate(lines):
        vertical_position = int(round(margin_pixels + (i * realistic_line_height)))
        draw.text((horizontal_position, vertical_position), line, fill=font_color, font=font)

    return image



temp_folder: pathlib.Path = pathlib.Path.cwd() / "TempImages"
if not temp_folder.is_dir():
    temp_folder.mkdir()

lattice_folder: pathlib.Path = pathlib.Path.cwd() / "LatticeImages"

specific_folder: str = input("Input folder to convert to gif: ")

specific_folder: pathlib.Path = lattice_folder / specific_folder

txt_files = list(specific_folder.rglob("*.txt"))

txt_files.sort(key = lambda x: os.path.getmtime(x))

# Open file
    # convert to ascii art or something
    # save file as png

for txt in txt_files:
    # img = Image.new('RGB', (200, 50), color = (255,255,255))
    file_name = txt.stem
    img = textfile_to_image(txt)
    # img.show()
    # break
    img.save(temp_folder / f"{file_name}.png")


png_files = list(temp_folder.rglob("*.png"))

png_files.sort(key = lambda x: os.path.getmtime(x))
# print(png_files, type(png_files))

frames = []
for png in png_files:
    new_frame = Image.open(png)
    # print(png)
    frames.append(new_frame)


frames[0].save("png2gif.gif", format="GIF", append_images=frames[1:], save_all = True, loop = 0, duration = 225) 

# delete temp image folder

shutil.rmtree(temp_folder)
