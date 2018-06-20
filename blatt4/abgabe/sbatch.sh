#!/bin/bash
# Do not forget to select a proper partition if the default
# one is no fit for the job!
#SBATCH --job-name=CASEQ
#SBATCH --output=CASEQ.%j.out
#SBATCH --error=CASEQ.%j.err
#SBATCH --nodes=2 # number of nodes
#SBATCH --tasks-per-node=4 # number of tasks per node
#SBATCH --time=00:10:00 # walltime

#Good Idea to stop operation on first error.
set -e

# Actual work starting here.
mpirun campi 100 100
