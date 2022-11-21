import plotly.graph_objects as go
import pandas
import numpy
import pathlib

cannonical_file = pathlib.Path.cwd() / "GCan.csv"

with open(cannonical_file) as open_file:
    cannonical_data = pandas.read_csv(open_file, header = 0)

print(cannonical_data)

cannonical_data["Size"] = pandas.Categorical(cannonical_data["Size"])

sizes = set(cannonical_data["Size"])


sf10 = go.Figure()
sf10.update_layout(title = "Structure Factor S(1,0)")
for rxc in sizes:
    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata.sort_values("Temp")

    temps, sf = list(subdata["Temp"]), list(subdata["SFk10"])

    sf10.add_trace(go.Scatter(x = temps, y = sf, name = rxc))
sf10.show()


sf01 = go.Figure()
sf01.update_layout(title = "Structure Factor S(0,1)")
for rxc in sizes:
    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata.sort_values("Temp")
    temps, sf = list(subdata["Temp"]), list(subdata["SFk01"])

    sf01.add_trace(go.Scatter(x = temps, y = sf, name = rxc))
sf01.show()


bc10 = go.Figure()
bc10.update_layout(title = "Binder Parameter 1, 0")
for rxc in sizes:
    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata.sort_values("Temp")
    temps, sf = list(subdata["Temp"]), list(subdata["BC10"])

    bc10.add_trace(go.Scatter(x = temps, y = sf, name = rxc))
bc10.show()


bc01 = go.Figure()
bc01.update_layout(title = "Binder Parameter 0, 1")
for rxc in sizes:
    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata.sort_values("Temp")
    temps, sf = list(subdata["Temp"]), list(subdata["BC01"])

    bc01.add_trace(go.Scatter(x = temps, y = sf, name = rxc))
bc01.show()