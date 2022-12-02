import plotly.graph_objects as go
import pandas
import numpy
import pathlib
import re

# line = dict(dash = "dash", color  = "black", width = 0.3)

cannonical_file = pathlib.Path.cwd() / "GCan.csv"

with open(cannonical_file) as open_file:
    cannonical_data = pandas.read_csv(open_file, header = 0)


print(cannonical_data)

cannonical_data["Size"] = pandas.Categorical(cannonical_data["Size"])

sizes = set(cannonical_data["Size"])


# Structure Factor plots
sig = go.Figure()
for rxc in sizes:
    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata = subdata.sort_values("Temp")

    temps, sf = list(subdata["Temp"]), list(subdata["SFk10"])

    sig.add_trace(go.Scatter(x = temps, y = sf, name = f"{rxc}_S(1,0)", mode = "markers"))
    sig.add_trace(go.Scatter(x = temps, y = sf, name = f"{rxc}_S(1,0)", mode = "lines", line = dict(dash = "dash", color  = "black", width = 0.3)))

for rxc in sizes:
    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata = subdata.sort_values("Temp")
    temps, sf = list(subdata["Temp"]), list(subdata["SFk01"])

    sig.add_trace(go.Scatter(x = temps, y = sf, name = f"{rxc}_S(0,1)", mode = "markers"))
    sig.add_trace(go.Scatter(x = temps, y = sf, name = f"{rxc}_S(0,1)", mode = "lines", line = dict(dash = "dash", color  = "black", width = 0.3)))

sig.update_layout(title = "Structure Factor Plots")
sig.show()


big = go.Figure()
for rxc in sizes:
    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata = subdata.sort_values("Temp")

    temps, bp = list(subdata["Temp"]), list(subdata["BC10"])

    big.add_trace(go.Scatter(x = temps, y = bp, name = f"{rxc}_B(1,0)", mode = "markers"))
    big.add_trace(go.Scatter(x = temps, y = bp, name = f"{rxc}_B(1,0)", mode = "lines", line = dict(dash = "dash", color  = "black", width = 0.3)))



for rxc in sizes:
    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata = subdata.sort_values("Temp")
    temps, bp = list(subdata["Temp"]), list(subdata["BC01"])

    big.add_trace(go.Scatter(x = temps, y = bp, name = f"{rxc}_B(0,1)", mode = "markers"))
    big.add_trace(go.Scatter(x = temps, y = bp, name = f"{rxc}_B(0,1)", mode = "lines", line = dict(dash = "dash", color  = "black", width = 0.3)))


big.update_layout(title = "Binder Parameter Plots")
big.show()


cig = go.Figure()
for rxc in sizes:
    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata = subdata.sort_values("Temp")

    temps= list(subdata["Temp"])

    subdata["X10"] = (subdata["Sqr10"] - (subdata["SFk10"] ** 2))

    cig.add_trace(go.Scatter(x = temps, y = subdata["X10"], name = f"{rxc}_X(1,0)", mode = "markers"))
    cig.add_trace(go.Scatter(x = temps, y = subdata["X10"], name = f"{rxc}_X(1,0)", mode = "lines", line = dict(dash = "dash", color  = "black", width = 0.3)))


for rxc in sizes:
    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata = subdata.sort_values("Temp")

    temps= list(subdata["Temp"])

    subdata["X01"] = (subdata["Sqr01"] - (subdata["SFk01"] ** 2))

    cig.add_trace(go.Scatter(x = temps, y = subdata["X01"], name = f"{rxc}_X(0,1)", mode = "markers"))
    cig.add_trace(go.Scatter(x = temps, y = subdata["X01"], name = f"{rxc}_X(0,1)", mode = "lines", line = dict(dash = "dash", color  = "black", width = 0.3)))


cig.update_layout(title = "Specfic Heat Plots")
cig.show()