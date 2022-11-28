#!/bin/bash

if [ $# -ne 3 ]; then
    echo "Usage: $0 <port> <n_readers> <n_writers>"
    exit 1
fi

./client --ip 0.0.0.0 --port $1 --mode writer --threads $3 &
./client --ip 0.0.0.0 --port $1 --mode reader --threads $2 &

exit 0