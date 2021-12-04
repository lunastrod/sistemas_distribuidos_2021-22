#!/bin/sh

make

for i in `seq 1 1000`; do
	WAIT=`printf '0.%06d\n' $RANDOM`;
	(sleep $WAIT; echo "Lanzando cliente $i ..."; ./publisher $i) &
done

for i in `seq 1 1`; do
	WAIT=`printf '0.%06d\n' $RANDOM`;
	(sleep $WAIT; echo "Lanzando cliente $i ..."; ./subscriber $i) &
done