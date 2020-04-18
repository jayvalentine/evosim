from ast import literal_eval

import matplotlib.pyplot as plt

import os

if (os.path.exists('population.log')):
    os.remove('population.log')

os.system('./evosim.exe')

# Get the number of lines in the file.

file = open('population.log', 'r')

lines = 0
while True:
    line = file.readline()

    if not line:
        break

    lines += 1

file.close()

print(f"Found {lines} lines. Processing...")

groups = {}

step = 0

file = open('population.log', 'r')

while True:
    line = file.readline()

    if not line:
        break

    vals = line.split(":")

    population = vals[0]

    creatures = vals[1:-2]

    for c in creatures:
        colours = literal_eval(c)

        found_key = None

        for k in groups.keys():
            ref_colour = literal_eval(k)
            if (abs(ref_colour[0] - colours[0]) < 20) and (abs(ref_colour[1] - colours[1]) < 20) and (abs(ref_colour[2] - colours[2]) < 20):
                found_key = k

        if found_key:
            if step not in groups[found_key].keys():
                groups[found_key][step] = 0

            groups[found_key][step] += 1
        else:
            groups[c] = {step: 1}

    step += 1

    print(f"{step}/{lines}...")

max_steps = step

first = True
last_populations = []

for k in groups.keys():
    steps = list(groups[k].keys())

    population_color = literal_eval(k)

    line_color = (float(population_color[0])/255, float(population_color[1]/255), float(population_color[2])/255)

    populations = []

    for s in range(max_steps):
        if first:
            if s in groups[k].keys():
                populations.append(groups[k][s])
            else:
                populations.append(0)
        else:
            if s in groups[k].keys():
                populations.append(last_populations[s] + groups[k][s])
            else:
                populations.append(0)

    if first:
        last_populations = populations
    else:
        for i in range(len(last_populations)):
            if populations[i] > 0:
                last_populations[i] = populations[i]

    plt.plot(range(max_steps), populations, color=line_color)

    first = False

plt.show()

file.close()
