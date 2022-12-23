import matplotlib.pyplot as plt
import numpy as np
import sys

f = open(sys.argv[1], 'r')
lines = f.readlines()
f.close()

time = []
latency = []
for line in lines:
    if(line[0] == 'S'):
        continue
    time.append(float(line.split()[0]))
    latency.append(float(line.split()[1]))


# Use the plot() function to plot the points
plt.scatter(time, latency, s=1)

#plt.ylim(0, 0.15)
plt.title(sys.argv[1])

# Show the plot
plt.show()