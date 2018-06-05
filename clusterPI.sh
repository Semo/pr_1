#!/bin/bash
# Do not forget to select a proper partition if the default
# one is no fit for the job!
#SBATCH --job-name=Pi
#SBATCH --output=PI.%j.out
#SBATCH --error=PI.%j.err
#SBATCH --nodes=1 # number of nodes
#SBATCH --tasks-per-node=1
#SBATCH --time=00:10:00 # walltime
#SBATCH --exclusive

#Good Idea to stop operation on first error.
set -e

# Actual work starting here.
srun ./calc.sh ${1} ${2}
# -l prepend task number to Outputline
#--multi-prog run a job with diffrend programms and args for each task
