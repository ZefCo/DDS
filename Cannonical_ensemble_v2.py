import numpy
import pandas
import pathlib
import os
import re


temp_length = 8
efield_length = 9


def main():
    cwd = pathlib.Path.cwd()

    # print(cwd)

    ac10_file = cwd / "AC_Manual.csv"

    with open(ac10_file) as file:
        auto_corr = pandas.read_csv(file, header=0, index_col="Unnamed: 0")
    
    auto_corr["S10tau"] = auto_corr["S10tau"].astype(int) * 2
    auto_corr["S10n"] = auto_corr["S10n"].astype(int)

    print(auto_corr)

    rows, _ = auto_corr.shape

    sizes, temps, sfk10, sfk01, zen10, zen01, sqr01, sqr10, rad01, rad10 = [], [], [], [], [], [], [], [], [], []

    for row in range(rows):
        row_of_interest = auto_corr.iloc[row, :].copy()

        temp_setting = str(row_of_interest["Temp"])
        if len(temp_setting) < temp_length:
            for _ in range(temp_length - len(temp_setting)):
                temp_setting = f"{temp_setting}0"

        efield_setting = str(float(row_of_interest["E"]))
        if len(efield_setting) < efield_length:
            for _ in range(efield_length - len(efield_setting)):
                efield_setting = f"{efield_setting}0"

        size_settings = row_of_interest["Size"]
        size_dump = re.split("x", size_settings)
        parralel, orthogonal = int(size_dump[0]), int(size_dump[1])
        sites = parralel * orthogonal

        subfolder = cwd / "Data" / size_settings / temp_setting
        files = list(subfolder.rglob("*.csv"))

        orders = []
        seeds = []

        prefactor01 = (numpy.sin(numpy.pi / orthogonal) * (2*parralel)**-1)**2
        prefactor10 = (numpy.sin(numpy.pi / parralel) * (2*orthogonal)**-1)**2

        for file in files:

            file_parts = re.split("_", str(file.name))
            orders.append(int(re.split("x", file_parts[1])[0]))
            seeds.append(int(file_parts[7]))

        file_frame = pandas.DataFrame({"files": files, "order": orders, "seed": seeds})

        seed = set(file_frame["seed"])
        # print(seed)

        measure_index = [i * row_of_interest["S10tau"] for i in range(row_of_interest["S10n"])]
        # print(measure_index)
        # break

        sfk10_local, sfk01_local, sqr01_local, sqr10_local, zen01_local, zen10_local, rad10_local, rad01_local = [], [], [], [], [], [], [], []

        print(f"Working on subfolder:\n{subfolder}")

        for s in seed:
            sub_frame = file_frame[file_frame["seed"] == s]
            sub_frame = sub_frame.sort_values(by="order")

            ordered_files = list(sub_frame["files"])
            # print(ordered_files)
            ensemble = pandas.DataFrame()
            for f in ordered_files:
                with open (f) as local_file:
                    try:
                        local_data = pandas.read_csv(local_file, header = 0)
                    except Exception as e:
                        print(f"Error reading file\n{f}")
                        print(e)
                        print(type(e))
                    ensemble = pandas.concat([ensemble, local_data])

            ensemble = ensemble.sort_values(by="sweep")

            # ensemble = ensemble.set_index("sweep")

            ensemble = ensemble[ensemble["sweep"].isin(measure_index)]
            # print(ensemble)
            ensemble["SFk01"] = prefactor01 * ensemble["SFk01"]
            ensemble["SFk10"] = prefactor10 * ensemble["SFk10"]

            ensemble["SFk01**2"] = ensemble["SFk01"]**2
            ensemble["SFk10**2"] = ensemble["SFk10"]**2

            ensemble["SFk01**4"] = ensemble["SFk01"]**4
            ensemble["SFk10**4"] = ensemble["SFk10"]**4

            ensemble["Sqrt01"] = numpy.sqrt(ensemble["SFk01"])
            ensemble["Sqrt10"] = numpy.sqrt(ensemble["SFk10"])

            sfk01_local.append(ensemble["SFk01"].mean())
            sfk10_local.append(ensemble["SFk10"].mean())

            sqr01_local.append(ensemble["SFk01**2"].mean())
            sqr10_local.append(ensemble["SFk10**2"].mean())

            zen01_local.append(ensemble["SFk01**4"].mean()) 
            zen10_local.append(ensemble["SFk10**4"].mean())

            rad01_local.append(ensemble["Sqrt01"].mean())
            rad10_local.append(ensemble["Sqrt10"].mean())

            # print(sub_frame)

        sfk10_local = numpy.array(sfk10_local).mean()
        sfk01_local = numpy.array(sfk01_local).mean()

        sqr10_local = numpy.array(sqr10_local).mean()
        sqr01_local = numpy.array(sqr01_local).mean()

        zen10_local = numpy.array(zen10_local).mean()
        zen01_local = numpy.array(zen01_local).mean()
       
        # bc10_local = 1 - (sqr10_local / (3*(sfk10_local**2)))
        # bc01_local = 1 - (sqr01_local / (3*(sfk01_local**2)))

        sizes.append(size_settings), temps.append(temp_setting), sfk10.append(sfk10_local), sfk01.append(sfk01_local), sqr10.append(sqr10_local), sqr01.append(sqr01_local), zen10.append(zen10_local), zen01.append(zen01_local)

        # master = pandas.concat([master, pandas.Series()])

    master = pandas.DataFrame(data={"Size": sizes, "Temp": temps, "SFk10": sfk10, "SFk01": sfk01, "Sqr10": sqr10, "Sqr01": sqr01, "Zen10": zen10, "Zen01": zen01})

    # print(master)

    master.to_csv(cwd / "GCan_8.csv", index = None, header = True)






if __name__ in "__main__":
    main()