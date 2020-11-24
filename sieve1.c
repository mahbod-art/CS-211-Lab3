#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main(int argc, char *argv[])
{
   unsigned long int count;            /* Local prime count */
   double elapsed_time;                /* Parallel execution time */
   unsigned long int first;            /* Index of first multiple */
   unsigned long int global_count = 0; /* Global prime count */
   unsigned long long int high_value;  /* Highest value on this proc */
   unsigned long int i;
   int id;                           /* Process ID number */
   unsigned long int index;          /* Index of current prime */
   unsigned long long int low_value; /* Lowest value on this proc */
   char *marked;                     /* Portion of 2,...,'n' */
   unsigned long long int n;         /* Sieving from 2, ..., 'n' */
   int p;                            /* Number of processes */
   unsigned long int proc0_size;     /* Size of proc 0's subarray */
   unsigned long int prime;          /* Current prime */
   unsigned long int size;           /* Elements in 'marked' */

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
   /* Stop the timer */

   /* Add you code here  */

   proc0_size = (n - 1) / p;
   if ((2 + proc0_size) < (int)sqrt((double)n))
   {
      if (!id)
         printf("Too many processes\n");
      MPI_Finalize();
      exit(1);
   }

   low_value = 2 + BLOCK_LOW(id, p, n - 1);
   high_value = 2 + ((long int)(id + 1) * (long int)(n - 1) / (long int)p) - 1;

   if (low_value % 2 == 0)
   {
      low_value = low_value + 1;
   }
   if (high_value % 2 == 0)
   {
      high_value = high_value - 1;
   }
   size = ((high_value - low_value) / 2) + 1;

   prime = 3;

   marked = (char *)malloc(size);

   if (marked == NULL)
   {
      printf("Cannot allocate enough memory\n");
      MPI_Finalize();
      exit(1);
   }

   for (i = 0; i < size; i++)
   {
      marked[i] = 0;
   }
   if (!id)
   {
      index = 0;
   }

   do
   {
      if (prime * prime > low_value)
         first = ((prime * prime - 3) - (low_value - 3)) / 2;
      else
      {
         if (!(low_value % prime))
         {
            first = 0;
         }
         if (prime > low_value % (2 * prime))
         {
            first = (prime - (low_value % prime)) / 2;
         }

         if (prime < low_value % (2 * prime))
         {
            first = prime - (low_value % prime) / 2;
         }
      }
      for (i = first; i < size; i += prime)
         marked[i] = 1;
      if (!id)
      {

         while (marked[++index])
            ;
         prime = index * 2 + 3;
      }
      MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD);
   } while (prime * prime <= n);

   count = 0;
   for (i = 0; i < size; i++)
   {
      if (!marked[i])
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