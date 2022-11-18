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

    ac10_file = cwd / "AC10_Proj" / "AutoCorrs_10_Safe.csv"

    with open(ac10_file) as file:
        auto_corr = pandas.read_csv(file, header=0, index_col="Unnamed: 0")
    
    auto_corr["S10tau"] = auto_corr["S10tau"].astype(int) * 2
    auto_corr["S10n"] = auto_corr["S10n"].astype(int)

    print(auto_corr)

    rows, _ = auto_corr.shape

    sizes, temps, sfk10, sfk01 = [], [], [], []

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

        subfolder = cwd / "Data" / size_settings / temp_setting
        files = list(subfolder.rglob("*.csv"))

        orders = []
        seeds = []

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

        sfk10_local = []
        sfk01_local = []

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

            sfk10_local.append(ensemble["SFk10"].mean())
            sfk01_local.append(ensemble["SFk01"].mean())

            # print(sub_frame)

        sfk10_local = numpy.array(sfk10_local).mean()
        sfk01_local = numpy.array(sfk01_local).mean()

        sizes.append(size_settings), temps.append(temp_setting), sfk10.append(sfk10_local), sfk01.append(sfk01_local)

        # master = pandas.concat([master, pandas.Series()])

    master = pandas.DataFrame(data={"Size": sizes, "Temp": temps, "SFk10": sfk10, "SFk01": sfk01})

    print(master)

    master.to_csv(cwd / "GCan.csv", index = None, header = True)






if __name__ in "__main__":
    main()