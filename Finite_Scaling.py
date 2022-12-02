import pandas
import numpy
import pathlib
import re
import plotly.graph_objects as go
# import pathlib

tc = 3.17

cwd = pathlib.Path.cwd()
scaling_folder = cwd / "ScalingImages"

if not scaling_folder.is_dir():
    scaling_folder.mkdir()

# nu_p = 0.5
# nu_o = 0.2

# spacing = 0.05
b0 = 1/2
n0 = 3/2
epsilon = 0.3
steps = 0.05

nu_p = [(n0 - epsilon) + (i*steps) for i in range(int(2*epsilon/steps) + 1)]
beta = [(b0 - epsilon) + (i*steps) for i in range(int(2*epsilon/steps) + 1)]
# nu_p = [n0]
# beta = [b0]

# print(beta)
# print(nu_p)


# # print(nu_p)
# # print(nu_o)


cannonical_file = cwd / "GCan.csv"

with open(cannonical_file) as open_file:
    cannonical_data = pandas.read_csv(open_file, header = 0)

cannonical_data["Size"] = pandas.Categorical(cannonical_data["Size"])

sizes = set(cannonical_data["Size"])

cannonical_data["t"] = (cannonical_data["Temp"] - tc) / tc
cannonical_data["abst"] = abs(cannonical_data["t"])

# cannonical_data = cannonical_data[cannonical_data["abst"] <= 2]

# print(cannonical_data)



for b in beta:
    for n in nu_p:
        fig = go.Figure()

        figName = f"B_{b}_n_{n}.png"

        for rxc in sizes:

            subdata = cannonical_data[cannonical_data["Size"] == rxc].copy()
            subdata = subdata.sort_values("t")
            subdata = subdata[subdata["abst"] <= 2]

            lattice = re.split("x", rxc)
            parallel, orthongonal = int(lattice[0]), int(lattice[1])

            subdata["x"] = (subdata["abst"] * (parallel**(1/n)))
            # subdata["y"] = parallel**(-(b/n)) * ((2*parallel / numpy.sin(numpy.pi / orthongonal)) * subdata["SFk01"])
            subdata["y"] = parallel**(-(b/n)) * ((numpy.sqrt(parallel*orthongonal)) * subdata["SFk01"])

            # subdata["tLo"] = subdata["t"] * (orthongonal**(2/3))
            # subdata = subdata.sort_values("tLp")

            fig.add_trace(go.Scatter(x = subdata["x"], y = subdata["y"], name = f"{rxc}_01", mode = "markers"))
            fig.add_trace(go.Scatter(x = subdata["x"], y = subdata["y"], mode = "lines", line = dict(dash = "dash", color  = "black", width = 0.3)))
            fig.update_layout(title = f"B = {b}\tNu = {n}")
            # fig.add_trace(go.Scatter(x = subdata["tLp"], y = subdata["SFk10"], name = f"{rxc}_10"))


        # fig.show()
        fig.write_image(str(scaling_folder / figName))