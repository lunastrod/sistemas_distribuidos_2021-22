#!/bin/bash

for i in `seq 1 1000`; do
WAIT=`printf '0.%06d\n' $RANDOM`;
(sleep $WAIT; echo "Lanzando cliente $i ..."; ./client $i) &
done

exit 0