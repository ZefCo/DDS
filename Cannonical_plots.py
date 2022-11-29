import plotly.graph_objects as go
import pandas
import numpy
import pathlib
import re

cannonical_file = pathlib.Path.cwd() / "GCan_7.csv"

with open(cannonical_file) as open_file:
    cannonical_data = pandas.read_csv(open_file, header = 0)

# cannonical_data = cannonical_data[cannonical_data["Size"] == "46x18"]

# cannonical_data["SFk10"] = (1/(46*18)) * cannonical_data["SFk10"]
# cannonical_data["SFk01"] = (1/(46*18)) * cannonical_data["SFk01"]


print(cannonical_data)

cannonical_data["Size"] = pandas.Categorical(cannonical_data["Size"])

sizes = set(cannonical_data["Size"])


fig = go.Figure()
# fig.update_layout(title = "Structure Factor S(1,0)")
for rxc in sizes:
    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata = subdata.sort_values("Temp")

    temps, sf = list(subdata["Temp"]), list(subdata["SFk10"])

    fig.add_trace(go.Scatter(x = temps, y = sf, name = f"{rxc}_S(1,0)"))


# fig.update_layout(title = "Structure Factor S(0,1)")
for rxc in sizes:
    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata = subdata.sort_values("Temp")
    temps, sf = list(subdata["Temp"]), list(subdata["SFk01"])

    fig.add_trace(go.Scatter(x = temps, y = sf, name = f"{rxc}_S(0,1)"))


# # fig.update_layout(title = "Binder Parameter 1, 0")
# for rxc in sizes:
#     subdata = cannonical_data[cannonical_data["Size"] == rxc]
#     subdata.sort_values("Temp")
#     temps, sf = list(subdata["Temp"]), list(subdata["BC10"])

#     fig.add_trace(go.Scatter(x = temps, y = sf, name = f"{rxc}_B(1,0)"))


# # fig.update_layout(title = "Binder Parameter 0, 1")
# for rxc in sizes:
#     subdata = cannonical_data[cannonical_data["Size"] == rxc]
#     subdata.sort_values("Temp")
#     temps, sf = list(subdata["Temp"]), list(subdata["BC01"])

#     fig.add_trace(go.Scatter(x = temps, y = sf, name = f"{rxc}_B(0,1)"))
fig.show()


big = go.Figure()
# fig.update_layout(title = "Structure Factor S(1,0)")
for rxc in sizes:
    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata = subdata.sort_values("Temp")

    temps, sf = list(subdata["Temp"]), list(subdata["BC10"])

    big.add_trace(go.Scatter(x = temps, y = sf, name = f"{rxc}_B(1,0)"))


# fig.update_layout(title = "Structure Factor S(0,1)")
for rxc in sizes:
    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata = subdata.sort_values("Temp")
    temps, sf = list(subdata["Temp"]), list(subdata["BC01"])

    big.add_trace(go.Scatter(x = temps, y = sf, name = f"{rxc}_B(0,1)"))


# # fig.update_layout(title = "Binder Parameter 1, 0")
# for rxc in sizes:
#     subdata = cannonical_data[cannonical_data["Size"] == rxc]
#     subdata.sort_values("Temp")
#     temps, sf = list(subdata["Temp"]), list(subdata["BC10"])

#     fig.add_trace(go.Scatter(x = temps, y = sf, name = f"{rxc}_B(1,0)"))


# # fig.update_layout(title = "Binder Parameter 0, 1")
# for rxc in sizes:
#     subdata = cannonical_data[cannonical_data["Size"] == rxc]
#     subdata.sort_values("Temp")
#     temps, sf = list(subdata["Temp"]), list(subdata["BC01"])

#     fig.add_trace(go.Scatter(x = temps, y = sf, name = f"{rxc}_B(0,1)"))
big.show()


cig = go.Figure()

for rxc in sizes:
    dim = re.split("x", rxc)
    p, o = int(dim[0]), int(dim[1])
    subdata = cannonical_data[cannonical_data["Size"] == rxc]
    subdata = subdata.sort_values("Temp")

    temps= list(subdata["Temp"])

    consant = p / numpy.sin(numpy.pi / o)

    subdata["X"] = consant * (subdata["Sqr01"] - (subdata["SFk01"] ** 2))

    cig.add_trace(go.Scatter(x = temps, y = subdata["X"], name = f"{rxc}"))

cig.show()