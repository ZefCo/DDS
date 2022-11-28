import pandas
import numpy
import pathlib
import re
import plotly.graph_objects as go

tc = 1.4*2.27

cannonical_file = pathlib.Path.cwd() / "GCan_5.csv"

with open(cannonical_file) as open_file:
    cannonical_data = pandas.read_csv(open_file, header = 0)

cannonical_data["Size"] = pandas.Categorical(cannonical_data["Size"])

sizes = set(cannonical_data["Size"])

cannonical_data["t"] = abs((cannonical_data["Temp"] - tc) / tc)


fig = go.Figure()

for rxc in sizes:

    subdata = cannonical_data[cannonical_data["Size"] == rxc]

    lattice = re.split("x", rxc)
    parallel, orthongonal = int(lattice[0]), int(lattice[1])

    # subdata["SFk10"] = ((0.5 * parallel * orthongonal)**(-1)) * subdata["SFk10"]
    # subdata["SFk01"] = ((0.5 * parallel * orthongonal)**(-1)) * subdata["SFk01"]


    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata["tLp"] = numpy.log2(subdata["t"] * (parallel**(2/3)))
    subdata["PL"] = numpy.log2(parallel**(1/3) * subdata["SFk01"])
    # subdata["tLo"] = subdata["t"] * (orthongonal**(2/3))

    fig.add_trace(go.Scatter(x = subdata["tLp"], y = subdata["PL"], name = f"{rxc}_01"))
    # fig.add_trace(go.Scatter(x = subdata["tLp"], y = subdata["SFk10"], name = f"{rxc}_10"))


fig.show()