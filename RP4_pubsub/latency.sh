#!/bin/bash

IP="0.0.0.0"
NDATA=100

rm -f latency/latency_*.txt
make fresh
killall publisher
killall subscriber
killall broker
killall generate_latency_file.sh

echo "--------------------------------------" >> latency/results_latency.csv

# Define an array of modes (secuencial, paralelo, and justo)
modes=("secuencial" "paralelo" "justo")
# Define an array of numbers (50, 500, and 900)
numbers=(50 500 900)

port=8090

for mode in ${modes[@]}; do
  # Loop through each number
  for number in ${numbers[@]}; do
    # Run the generate_latency_file.sh program with the current mode and number
    set -x 
    ./generate_latency_file.sh $IP $port $mode $number $NDATA & >/dev/null
    { set +x; } 2>/dev/null 
    ((port++))
    sleep 0.25
  done
done

sleep 2

echo brokers > latency/pcount.txt
ps aux | grep -v grep| grep broker | wc -l >> latency/pcount.txt
echo subscribers >> latency/pcount.txt
ps aux | grep -v grep| grep subscriber | wc -l >> latency/pcount.txt
echo publishers >> latency/pcount.txt
ps aux | grep -v grep| grep publisher | wc -l >> latency/pcount.txt