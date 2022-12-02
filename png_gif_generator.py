from math import ceil
import pathlib
import PIL
from PIL import Image, ImageFont, ImageDraw
import os
import shutil


temp_folder: pathlib.Path = pathlib.Path.cwd() / "TempPNGImages"
if not temp_folder.is_dir():
    temp_folder.mkdir()


specific_folder: pathlib.Path = pathlib.Path(input("Input folder to convert to gif: "))

# specific_folder: pathlib.Path = lattice_folder / specific_folder

png_files = list(specific_folder.rglob("*.png"))
png_files.sort(key = lambda x: os.path.getmtime(x))

frames = []
for png in png_files:
    new_frame = Image.open(png)
    # print(png)
    frames.append(new_frame)


frames[0].save("png2gif.gif", format="GIF", append_images=frames[1:], save_all = True, loop = 0, duration = 225) 
