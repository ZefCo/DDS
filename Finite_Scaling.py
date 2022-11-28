import pandas
import numpy
import pathlib
import re
import plotly.graph_objects as go

tc = 1.4*2.27

# nu_p = 0.5
# nu_o = 0.2

# spacing = 0.05

# nu_p = [((2 / 3) - 0.05) + (i*0.01) for i in range(10)]
# nu_o = [((1 / 3) - 0.05) + (i*0.01) for i in range(10)]

nu_p = [2 / 3]
nu_o = [1 / 3]


# print(nu_p)
# print(nu_o)


cannonical_file = pathlib.Path.cwd() / "GCan_5.csv"

with open(cannonical_file) as open_file:
    cannonical_data = pandas.read_csv(open_file, header = 0)

cannonical_data["Size"] = pandas.Categorical(cannonical_data["Size"])

sizes = set(cannonical_data["Size"])

cannonical_data["t"] = abs(cannonical_data["Temp"] - tc) / tc



for np in nu_p:
    for no in nu_o:
        fig = go.Figure()

        for rxc in sizes:

            subdata = cannonical_data[cannonical_data["Size"] == rxc].copy()

            lattice = re.split("x", rxc)
            parallel, orthongonal = int(lattice[0]), int(lattice[1])

            # subdata["SFk10"] = ((0.5 * parallel * orthongonal)**(-1)) * subdata["SFk10"]
            # subdata["SFk01"] = ((0.5 * parallel * orthongonal)**(-1)) * subdata["SFk01"]


            # subdata = cannonical_data[cannonical_data["Size"] == rxc]
            subdata["tLp"] = (subdata["t"] * (parallel**(np)))
            subdata["PL"] = (parallel**(no) * (0.5 * numpy.sin(numpy.pi / parallel) * subdata["SFk01"]))
            # subdata["tLo"] = subdata["t"] * (orthongonal**(2/3))

            fig.add_trace(go.Scatter(x = subdata["tLp"], y = subdata["PL"], name = f"{rxc}_01", mode = "markers"))
            fig.add_trace(go.Scatter(x = subdata["tLp"], y = subdata["PL"], mode = "lines", line = dict(dash = "dash", color  = "black", width = 0.3)))
            fig.update_layout(title = f"Nu P = {np}\tNu O = {no}")
            # fig.add_trace(go.Scatter(x = subdata["tLp"], y = subdata["SFk10"], name = f"{rxc}_10"))


        fig.show()