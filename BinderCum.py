import pandas
import numpy
import pathlib
import re
import plotly.graph_objects as go


cannonical_file = pathlib.Path.cwd() / "GCan_5.csv"

with open(cannonical_file) as open_file:
    cannonical_data = pandas.read_csv(open_file, header = 0)

cannonical_data["Size"] = pandas.Categorical(cannonical_data["Size"])

sizes = set(cannonical_data["Size"])

cannonical_data["BC10"] = 1 - ((cannonical_data["SFk10"]**2) / (3 * cannonical_data["SFk10"]))
cannonical_data["BC01"] = 1 - ((cannonical_data["SFk01"]**2) / (3 * cannonical_data["SFk01"]))


print(cannonical_data)

# cannonical_data["t"] = abs(cannonical_data["Temp"] - tc) / tc
fig = go.Figure()

for rxc in sizes:

    subdata = cannonical_data[cannonical_data["Size"] == rxc].copy()

    lattice = re.split("x", rxc)
    parallel, orthongonal = int(lattice[0]), int(lattice[1])

    # subdata["SFk10"] = ((0.5 * parallel * orthongonal)**(-1)) * subdata["SFk10"]
    # subdata["SFk01"] = ((0.5 * parallel * orthongonal)**(-1)) * subdata["SFk01"]


    # subdata = cannonical_data[cannonical_data["Size"] == rxc]
    # subdata["Lp"] = (subdata["t"] * (parallel**(2 / 3)))
    # subdata["PL"] = (parallel**(no) * (0.5 * numpy.sin(numpy.pi / orthongonal) * subdata["SFk01"]))
    # subdata["tLo"] = subdata["t"] * (orthongonal**(2/3))

    fig.add_trace(go.Scatter(x = subdata["Temp"], y = subdata["BC10"], name = f"{rxc}_10", mode = "markers"))
    fig.add_trace(go.Scatter(x = subdata["Temp"], y = subdata["BC10"], mode = "lines", line = dict(dash = "dash", color  = "black", width = 0.3)))

    fig.add_trace(go.Scatter(x = subdata["Temp"], y = subdata["BC01"], name = f"{rxc}_01", mode = "markers"))
    fig.add_trace(go.Scatter(x = subdata["Temp"], y = subdata["BC01"], mode = "lines", line = dict(dash = "dash", color  = "black", width = 0.3)))

    # fig.update_layout(title = f"Nu P = {np}\tNu O = {no}")
    # fig.add_trace(go.Scatter(x = subdata["tLp"], y = subdata["SFk10"], name = f"{rxc}_10"))


fig.show()