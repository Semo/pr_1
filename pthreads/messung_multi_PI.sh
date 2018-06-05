#!/bin/bash
declare -a threads=(1 2 4 8 16 24 32 48 96)
for i in "${threads[@]}"
  do
  sbatch ./clusterPI.sh $i 1000000000
done


