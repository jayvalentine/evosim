from ast import literal_eval

import matplotlib.pyplot as plt

file = open('population.log', 'r')

groups = {}

step = 0

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
            if (abs(ref_colour[0] - colours[0]) < 10) and (abs(ref_colour[1] - colours[1]) < 10) and (abs(ref_colour[2] - colours[2]) < 10):
                found_key = k

        if found_key:
            if step not in groups[found_key].keys():
                groups[found_key][step] = 0

            groups[found_key][step] += 1
        else:
            groups[c] = {step: 1}

    step += 1
    print(".", end="", flush=True)

for k in groups.keys():
    steps = list(groups[k].keys())

    population_color = literal_eval(k)

    line_color = (float(population_color[0])/255, float(population_color[1]/255), float(population_color[2])/255)

    populations = []

    for s in steps:
        populations.append(groups[k][s])

    plt.plot(steps, populations, color=line_color)

plt.show()

file.close()
