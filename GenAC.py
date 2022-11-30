import pandas
import re
import pathlib

cwd = pathlib.Path.cwd()
dataFolder = cwd / "Data"

tau = 1000
tmax = 10000000


ac_data = pandas.DataFrame()
for folder in dataFolder.iterdir():
    # print(folder.name)
    # print()
    if not re.match("Temp", folder.name):
        # print(f"dont do a thing with {folder}")
        for subfolder in folder.iterdir():
            # print(subfolder)

            settings = re.split("/", str(subfolder))
            temp = settings[8]
            size = settings[7]

            # print(temp, size)

            data = {"Temp": float(temp), "E": 50, "Size": size, "S10tau": tau, "S10tmax": tmax, "S10n": int(tmax/(2*tau))}

            new_row = pandas.Series(data = data)

            ac_data = pandas.concat([ac_data, new_row.to_frame()], axis = 1)


ac_data.T.to_csv(cwd / "AC_Manual.csv", header = True)

