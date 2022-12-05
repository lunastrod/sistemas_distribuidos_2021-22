#!/bin/bash

# Script para calcular la latencia entre publicador y subscriptores

BROKER_IP="0.0.0.0"
BROKER_PORT="8080"
TOPIC="cpu-usage"

echo "Generando archivo de latencia"

mode=$1


./broker --port $BROKER_PORT --mode $2

# Ejecuta N suscriptores asociados a un mismo TOPIC. Asegúrate que guardas su
# salida estándar en un fichero para su posterior análisis. Puedes utilizar el siguiente
# comando para ejecutar un comando, ver su salida estándar y además guardarla a fichero
for i in $(seq 1 $N); do
    ./subscriber --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC &
done

# Ejecuta 1 publicador asociado al mismo TOPIC y deja que genere al menos 100
# datos nuevos para publicar.
./publisher --ip $BROKER_IP --port $BROKER_PORT --topic $TOPIC

# deja que genere al menos 100 datos nuevos para publicar. (300s)
n_data=1
# multiply by 3 to get the total time
sleep $((n_data*3+1))
# stop all subscribers
killall subscriber -s SIGINT
# stop publisher
killall publisher -s SIGINT


generateLatencyFile 50 &
generateLatencyFile 500 &
generateLatencyFile 900 &
