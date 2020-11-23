#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define BLOCK_LOW(id, p, n) ((unsigned long long)(id) * (unsigned long long)(n) / (unsigned long long)(p))
int main(int argc, char *argv[])
{
   unsigned long int count;            /* Local prime count */
   double elapsed_time;                /* Parallel execution time */
   unsigned long int first;            /* Index of first multiple */
   unsigned long int global_count = 0; /* Global prime count */
   unsigned long long int HighVal;  /* Highest value on this proc */
   unsigned long int i;
   int id;                           /* Process ID number */
   unsigned long int index;          /* Index of current prime */
   unsigned long long int LowVal; /* Lowest value on this proc */
   char *m;                     /* Portion of 2,...,'n' */
   unsigned long long int n;         /* Sieving from 2, ..., 'n' */
   int p;                            /* Number of processes */
   unsigned long int proc0_size;     /* Size of proc 0's subarray */
   unsigned long int prime;          /* Current prime */
   unsigned long int size;           /* Elements in 'm' */

   MPI_Init(&argc, &argv);

   /* Start the timer */

   MPI_Comm_rank(MPI_COMM_WORLD, &id);
   MPI_Comm_size(MPI_COMM_WORLD, &p);
   MPI_Barrier(MPI_COMM_WORLD);
   elapsed_time = -MPI_Wtime();
   if (argc != 2)
   {
      if (!id)
         printf("Command line: %s <m>\n", argv[0]);
      MPI_Finalize();
      exit(1);
   }

   n = atoll(argv[1]);
   /* Add you code here  */

   proc0_size = (n - 1) / p;
   if ((2 + proc0_size) < (int)sqrt((double)n))
   {
      if (!id)
         printf("Too many processes\n");
      MPI_Finalize();
      exit(1);
   }

   LowVal = 2 + BLOCK_LOW(id, p, n - 1);

   HighVal = 2 + ((long int)(id + 1) * (long int)(n - 1) / (long int)p) - 1;

   if (LowVal % 2 == 0)
      LowVal = LowVal + 1;
   if (HighVal % 2 == 0)
      HighVal = HighVal - 1;
   size = ((HighVal - LowVal) / 2) + 1;

   prime = 3;

   m = (char *)malloc(size);

   if (m == NULL)
   {
      printf("Cannot allocate enough memory\n");
      MPI_Finalize();
      exit(1);
   }

   for (i = 0; i < size; i++)
      m[i] = 0;
   if (!id)
      index = 0;

   do
   {
      if (prime * prime > LowVal)
         first = ((prime * prime - 3) - (LowVal - 3)) / 2;
      else
      {
         if (!(LowVal % prime))
            first = 0;
         if (prime > LowVal % (2 * prime))
            first = (prime - (LowVal % prime)) / 2;
         if (prime < LowVal % (2 * prime))
            first = prime - (LowVal % prime) / 2;
      }
      for (i = first; i < size; i += prime)
         m[i] = 1;
      if (!id)
      {
         while (m[++index]);
         prime = index * 2 + 3;
      }
      MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD);
   } while (prime * prime <= n);

   count = 0;
   for (i = 0; i < size; i++)
   {
      if (!m[i])
         count++;
   }

   MPI_Reduce(&count, &global_count, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
   global_count = global_count + 1;

   elapsed_time += MPI_Wtime();

   /* Print the results */

   if (!id)
   {
      printf("The total number of prime: %ld, total time: %10.6f, total node %d\n", global_count, elapsed_time, p);
   }
   MPI_Finalize();
   return 0;
}
