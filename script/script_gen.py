algos = ["0","1","2","3"]
nodes = ["1","2","4","8"]
paralisms = ["32","64","128","256"]
for algo in algos:
    for i in range(len(nodes)):
        node = nodes[i]
        paralism = paralisms[i]
        query = '''#!/bin/bash
#SBATCH -o sieve{AG}_{N}.o
#SBATCH -e sieve{AG}_{N}.err
#SBATCH -J sieve{AG}_{N}
#SBATCH -N {N}
#SBATCH -t 00:03:00

module load mpich-3.2.1/gcc-4.8.5

mpirun -np {P} ./../build/sieve{AG} 10000000000 &> ../result/sieve{AG}_np_{P}_output.txt

rm *.err *.o

'''
        with open("sieve" + algo + "_" + node + ".sh", "w") as f:
            f.write(query.format( AG = algo, N = node, P = paralism  ))
