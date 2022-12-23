#python script to generate an csv file with the latency data
#usage: python latency_csv.py <latency_file> <csv_file> <mode> <n>
#the csv file has the format:
# Modo, N, min, max, avg, std (no incluyas esta línea)
# 0, 50, $MIN, $MAX, $AVG, $STD
# 0, 500, $MIN, $MAX, $AVG, $STD
# 0, 900, $MIN, $MAX, $AVG, $STD

#the input file has one number per line
# use numpy to calculate the mean and std

import sys
import numpy as np

#read the file
f = open(sys.argv[1], 'r')
lines = f.readlines()
f.close()

#calculate the mean and std
data = []
for line in lines:
    if(line[0] == 'S'):
        continue
    data.append(float(line.split()[1]))

np.array(data)

mean = np.mean(data)
std = np.std(data)

#write the csv file
f = open(sys.argv[2], 'a')
#write one of the lines
f.write(sys.argv[3] + ',' + sys.argv[4] + ',' + str(np.min(data)) + ',' + str(np.max(data)) + ',' + str(mean) + ',' + str(std) + '\n')
f.close()
