#!/bin/bash

BROKER_IP=$1
BROKER_PORT=$2
MODE=$3
NUM_SUBSCRIBERS=$4
FILENAME="latency/latency_${MODE}_${NUM_SUBSCRIBERS}"
NDATA=2 #100

TOPIC="cpu-usage"

./broker --port $BROKER_PORT --mode $MODE &

# Ejecuta 1 publicador asociado al mismo TOPIC y deja que genere al menos 100
# datos nuevos para publicar.
./publisher --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC &

sleep 2

# Ejecuta N suscriptores asociados a un mismo TOPIC. Asegúrate que guardas su
# salida estándar en un fichero para su posterior análisis. Puedes utilizar el siguiente
# comando para ejecutar un comando, ver su salida estándar y además guardarla a fichero
for i in $(seq 1 $NUM_SUBSCRIBERS); do
    ./subscriber --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC | tee $FILENAME"_"$i.txt &
done

# deja que genere al menos 100 datos nuevos para publicar. (300s)
# multiply by 3 to get the total time
sleep $((NDATA*3))
# stop all subscribers
killall subscriber -s SIGINT
# stop publisher
killall publisher -s SIGINT
# stop broker
killall broker -s SIGINT

sleep 1
#cat latency/latency_secuencial_50_50.txt | grep -o "Latencia: [0-9.]*" | sed -E 's/Latencia: ([0-9.]*)./\1/g'
# get the data from the files and save it in 1 file only with the latency data
rm -f $FILENAME.txt
for i in $(seq 1 $NUM_SUBSCRIBERS); do
    cat $FILENAME"_"$i.txt | grep -o "Latencia: [0-9.]*" | sed -E 's/Latencia: ([0-9.]*)./\1/g' >> $FILENAME.txt
done

# delete the files with the latency data
for i in $(seq 1 $NUM_SUBSCRIBERS); do
    rm $FILENAME"_"$i.txt
done

# append to the results file
python3 latency_csv.py $FILENAME.txt latency/results_latency.csv $MODE $NUM_SUBSCRIBERS

# remove the file with the latency data
rm $FILENAME.txt