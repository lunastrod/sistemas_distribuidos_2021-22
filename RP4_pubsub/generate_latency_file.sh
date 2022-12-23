#!/bin/bash

MODE=$1
NUM_SUBSCRIBERS=$2
FILENAME="latency/latency_${MODE}_${NUM_SUBSCRIBERS}"

# get the data from the files and save it in 1 file only with the latency data
rm -f results/$FILENAME.txt
echo "generating $FILENAME.txt"
for i in $(seq 1 $NUM_SUBSCRIBERS); do
    #[1671798023.952543] Recibido mensaje topic: cpu-usage - mensaje: "cpu load:  1m: 7.32  5m: 2.51  15: 1.48" - Generó: 1671798023.951140 - Recibido: 1671798023.952543 - Latencia: 0.001403.
    #sed -n 's/.*Latencia: \([0-9.]*\).*Generó: \([0-9.]*\).*/\1 \2/p'
    #genero=$(cat $FILENAME"_"$i.txt | grep -o 'Generó: [0-9.]*' | grep -o '[0-9.]*')
    #latencia=$(cat $FILENAME"_"$i.txt | grep -o 'Latencia: [0-9.]*' | grep -o '[0-9.]*')
    echo "SUBSCRIBER $i" >> results/$FILENAME.txt
    cat $FILENAME"_"$i.txt | grep 'Latencia: [0-9.]*' | grep -oP 'Generó: \K\d+.\d+|Latencia: \K\d+.\d+' | xargs -n 2 >> results/$FILENAME.txt
    #echo "$i $genero $latencia" >> results/$FILENAME.txt
    #cat $FILENAME"_"$i.txt | grep -o "Latencia: [0-9.]*" | sed -E 's/Latencia: ([0-9.]*)./\1/g' > $FILENAME"_"$i.txt
done

echo "generated $FILENAME.txt"
# append to the results file
#python3 latency_csv.py $FILENAME.txt latency/results_latency.csv $MODE $NUM_SUBSCRIBERS

exit 0