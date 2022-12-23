#!/bin/bash

IP="0.0.0.0"
NDATA=100

if [ $1 == "killall" ]; then
  killall publisher
  killall subscriber
  killall broker
  exit 0
fi

if [ $1 == "generate" ]; then
  ./generate_latency_data.sh $IP 8090 secuencial 50 $NDATA & >/dev/null
  ./generate_latency_data.sh $IP 8090 paralelo 50 $NDATA & >/dev/null
  ./generate_latency_data.sh $IP 8090 justo 50 $NDATA & >/dev/null
  ./generate_latency_data.sh $IP 8090 secuencial 500 $NDATA & >/dev/null
  ./generate_latency_data.sh $IP 8090 paralelo 500 $NDATA & >/dev/null
  ./generate_latency_data.sh $IP 8090 justo 500 $NDATA & >/dev/null
  ./generate_latency_data.sh $IP 8090 secuencial 900 $NDATA & >/dev/null
  ./generate_latency_data.sh $IP 8090 paralelo 900 $NDATA & >/dev/null
  ./generate_latency_data.sh $IP 8090 justo 900 $NDATA & >/dev/null
  exit 0
fi

if [ $1 == "process" ]; then
  echo "---------------------" >> latency/results_latency.csv
  ./generate_latency_file.sh secuencial 50 &
  ./generate_latency_file.sh paralelo 50 &
  ./generate_latency_file.sh justo 50 &
  ./generate_latency_file.sh secuencial 500 &
  ./generate_latency_file.sh paralelo 500 &
  ./generate_latency_file.sh justo 500 &
  ./generate_latency_file.sh secuencial 900 &
  ./generate_latency_file.sh paralelo 900 &
  ./generate_latency_file.sh justo 900 &
  exit 0
fi

if [ $1 == "pcount" ]; then
  echo brokers > latency/pcount.txt
  ps aux | grep -v grep| grep broker | wc -l >> latency/pcount.txt
  echo subscribers >> latency/pcount.txt
  ps aux | grep -v grep| grep subscriber | wc -l >> latency/pcount.txt
  echo publishers >> latency/pcount.txt
  ps aux | grep -v grep| grep publisher | wc -l >> latency/pcount.txt
  exit 0
fi

if [ $1 == "plot" ]; then
  python3 generate_latency_plot.py results/latency/latency_justo_50.txt &
  python3 generate_latency_plot.py results/latency/latency_justo_500.txt &
  python3 generate_latency_plot.py results/latency/latency_justo_900.txt &
  python3 generate_latency_plot.py results/latency/latency_paralelo_50.txt &
  python3 generate_latency_plot.py results/latency/latency_paralelo_500.txt &
  python3 generate_latency_plot.py results/latency/latency_paralelo_900.txt &
  python3 generate_latency_plot.py results/latency/latency_secuencial_50.txt &
  python3 generate_latency_plot.py results/latency/latency_secuencial_500.txt &
  python3 generate_latency_plot.py results/latency/latency_secuencial_900.txt &
  exit 0
fi

if [ $1 == "close_plot" ]; then
  killall python3
  exit 0
fi

if [ $1 == "csv" ]; then
  python3 generate_latency_csv.py results/latency/latency_secuencial_50.txt results/latency/results_latency.csv secuencial 50 
  python3 generate_latency_csv.py results/latency/latency_secuencial_500.txt results/latency/results_latency.csv secuencial 500 
  python3 generate_latency_csv.py results/latency/latency_secuencial_900.txt results/latency/results_latency.csv secuencial 900 
  python3 generate_latency_csv.py results/latency/latency_paralelo_50.txt results/latency/results_latency.csv paralelo 50 
  python3 generate_latency_csv.py results/latency/latency_paralelo_500.txt results/latency/results_latency.csv paralelo 500 
  python3 generate_latency_csv.py results/latency/latency_paralelo_900.txt results/latency/results_latency.csv paralelo 900 
  python3 generate_latency_csv.py results/latency/latency_justo_50.txt results/latency/results_latency.csv justo 50 
  python3 generate_latency_csv.py results/latency/latency_justo_500.txt results/latency/results_latency.csv justo 500 
  python3 generate_latency_csv.py results/latency/latency_justo_900.txt results/latency/results_latency.csv justo 900 
  exit 0
fi