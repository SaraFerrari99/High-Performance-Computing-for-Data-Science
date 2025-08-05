#!/bin/bash
#PBS -l select=1:ncpus=4:mem=2gb

#set max execution time
#PBS -l walltime=0:05:00

#set the queue
#PBS -q short_cpuQ

echo "hostname:" && hostname
