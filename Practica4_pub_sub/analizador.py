import numpy as np
# cat data.txt |grep -o 'Latencia: [^,]*$'| cut -d'[' -f 2 | cut -d']' -f 1 | tee latencias.txt
array=[]
with open('latencias.txt') as f:
    for line in f: # read rest of lines
        array.append(float(line))

print(str(np.max(array))+",",str(np.min(array))+",",str(np.mean(array))+",",str(np.std(array)))

