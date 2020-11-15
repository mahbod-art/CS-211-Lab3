#!/bin/bash
python script_gen.py
sbatch sieve0_1.sh
sbatch sieve0_2.sh
sbatch sieve0_4.sh
sbatch sieve0_8.sh
sbatch sieve1_1.sh
sbatch sieve1_2.sh
sbatch sieve1_4.sh
sbatch sieve1_8.sh
sbatch sieve2_1.sh
sbatch sieve2_2.sh
sbatch sieve2_4.sh
sbatch sieve2_8.sh
sbatch sieve3_1.sh
sbatch sieve3_2.sh
sbatch sieve3_4.sh
sbatch sieve3_8.sh

rm sieve*.sh
cp ../*.c ~
