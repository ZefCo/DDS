import pathlib
import re

cwd = pathlib.Path.cwd()

dataFolder = cwd / "Data"

for folder in dataFolder.iterdir():
    strfolder = str(folder.name)
    if re.match("Temp", strfolder):
        settings = re.split("_", strfolder)
        # print(settings)
        temp_setting = settings[1]
        size_setting = settings[5]

        size_folder = folder.parent / size_setting
        temp_folder = folder.parent / size_setting / temp_setting

        final_folder = temp_folder / folder.name

        # print(final_folder)
        if not final_folder.exists():
            pathlib.Path(final_folder).mkdir(parents=True, exist_ok=True)


        pathlib.Path(folder).rename(final_folder)


