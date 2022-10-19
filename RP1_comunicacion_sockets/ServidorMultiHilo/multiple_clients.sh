#!/bin/bash

for i in `seq 1 300`; do
WAIT=`printf '0.%06d\n' $RANDOM`;
(sleep $WAIT; echo "Lanzando cliente $i ..."; ./client $i "0.0.0.0" "8081") &
done

exit 0