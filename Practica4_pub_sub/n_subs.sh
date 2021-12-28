#!/bin/bash

for i in `seq 1 500`; do
	echo "Lanzando cliente $i ...";
	(./"$@"|tee -a data.txt)&
done
