#!/bin/bash
#PBS -l select=2:ncpus=5:mem=2gb

#set max execution time
#PBS -l walltime=0:05:00

#set the queue
#PBS -q short_cpuQ
# Vai nella directory da cui è stato inviato il job
cd $PBS_O_WORKDIR

module load mpich-3.2

mpirun.actual -n 10 ./test1
