import numpy as np

array=[]
with open('latencias.txt') as f:
    for line in f: # read rest of lines
        array.append(float(line))

print(str(np.max(array))+",",str(np.min(array))+",",str(np.mean(array))+",",str(np.std(array)))

