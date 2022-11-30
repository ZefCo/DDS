import pandas
import re
import pathlib

cwd = pathlib.Path.cwd()
dataFolder = cwd / "Data"


ac_data = pandas.DataFrame()
for folder in dataFolder.iterdir():
    for subfolder in folder.iterdir():
        settings = re.split("/", str(subfolder))
        temp = settings[8]
        size = settings[7]

        # print(temp, size)

        data = {"Temp": float(temp), "E": 50, "Size": size, "S10tau": 1000, "S10tmax": 10000000, "S10n": int(10000000/(2*1000))}

        new_row = pandas.Series(data = data)

        ac_data = pandas.concat([ac_data, new_row.to_frame()], axis = 1)


ac_data.T.to_csv(cwd / "AC_Manual.csv", header = True)

