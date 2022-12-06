#!/bin/bash

IP="0.0.0.0"
NDATA=100

rm -f latency/latency_*.txt
make fresh
killall publisher
killall subscriber
killall broker

echo "running secuencial 50"
./generate_latency_file.sh $IP 8080 secuencial 50 $NDATA &
sleep 0.25
echo "running secuencial 500"
./generate_latency_file.sh $IP 8081 secuencial 500 $NDATA &
sleep 0.25
echo "running secuencial 900"
./generate_latency_file.sh $IP 8082 secuencial 900 $NDATA &
sleep 0.25

echo "running paralelo 50"
./generate_latency_file.sh $IP 8083 paralelo 50 $NDATA &
sleep 0.25
echo "running paralelo 500"
./generate_latency_file.sh $IP 8084 paralelo 500 $NDATA &
sleep 0.25
echo "running paralelo 900"
./generate_latency_file.sh $IP 8085 paralelo 900 $NDATA &
sleep 0.25

echo "running justo 50"
./generate_latency_file.sh $IP 8086 justo 50 $NDATA &
sleep 0.25
echo "running justo 500"
./generate_latency_file.sh $IP 8087 justo 500 $NDATA &
sleep 0.25
echo "running justo 900"
./generate_latency_file.sh $IP 8088 justo 900 $NDATA &
sleep 0.25

sleep 2

echo brokers
ps aux | grep -v grep| grep broker | wc -l
echo subscribers
ps aux | grep -v grep| grep subscriber | wc -l
echo publishers
ps aux | grep -v grep| grep publisher | wc -l