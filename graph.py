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

difference = 60

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
            if (abs(ref_colour[0] - colours[0]) < difference) and (abs(ref_colour[1] - colours[1]) < difference) and (abs(ref_colour[2] - colours[2]) < difference):
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

plots = {}

num = 300

for k in groups.keys():
    steps = list(groups[k].keys())

    populations = []

    for s in range(max_steps):
        if s in groups[k].keys():
            populations.append(groups[k][s])
        else:
            populations.append(0)
    
    averages = []

    averages.append(populations[0])

    # Calculate moving average of populations.
    for i in range(num-1, max_steps):
        total = 0
        for j in range(num):
            total += populations[i-j]

        averages.append(total / float(num))

    plots[k] = averages

cumulative = []
for i in range(num-2, max_steps):
    cumulative.append(0)

for k in plots.keys():
    population_color = literal_eval(k)
    line_color = (float(population_color[0])/255, float(population_color[1]/255), float(population_color[2])/255)

    this_line = []

    for i in range(len(cumulative)):
        this_line.append(cumulative[i] + plots[k][i])
        cumulative[i] += plots[k][i]

    plt.plot([0] + list(range(num-1, max_steps)), this_line, color=line_color)

plt.show()

file.close()
