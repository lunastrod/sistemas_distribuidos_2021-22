#!/bin/bash

IP="0.0.0.0"

make fresh

echo "running secuencial 50"
./generate_latency_file.sh $IP 8080 secuencial 50

exit 0
echo "running secuencial 500"
./generate_latency_file.sh $IP 8081 secuencial 500
echo "running secuencial 900"
./generate_latency_file.sh $IP 8082 secuencial 900

echo "running paralelo 50"
./generate_latency_file.sh $IP 8083 paralelo 50
echo "running paralelo 500"
./generate_latency_file.sh $IP 8084 paralelo 500
echo "running paralelo 900"
./generate_latency_file.sh $IP 8085 paralelo 900

echo "running justo 50"
./generate_latency_file.sh $IP 8086 justo 50
echo "running justo 500"
./generate_latency_file.sh $IP 8087 justo 500
echo "running justo 900"
./generate_latency_file.sh $IP 8088 justo 900
