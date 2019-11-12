#!/bin/bash
#
#SBATCH --job-name=FilterModel
#
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=8260
#SBATCH --time=1-00:00:00
#SBATCH --qos=batch

srun c++/build/bin/JointDistributionLearner -i data/downsampled_to_1000.csv -o output/downsampled_to_1000.out --iterations 6000 &> output/downsampled_to_1000.log
