#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define BLOCK_LOW(id, p, n) ((id) * (n) / (p))
#define BLOCK_HIGH(id, p, n) (BLOCK_LOW((id) + 1, p, n) - 1)
#define BLOCK_SIZE 15000
int main(int argc, char *argv[])
{
   unsigned long int count; /* Local prime count */
   double elapsed_time;     /* Parallel execution time */
   unsigned long int first; /* Index of first multiple */
   int local_first;
   unsigned long int global_count = 0; /* Global prime count */
   unsigned long long int high_value;  /* Highest value on this proc */
   unsigned long int i;
   int id;                           /* Process ID number */
   unsigned long int index;          /* Index of current prime */
   unsigned long long int low_value; /* Lowest value on this proc */
   char *marked;                     /* Portion of 2,...,'n' */
   char *local_prime_marked;
   unsigned long long int n;     /* Sieving from 2, ..., 'n' */
   int p;                        /* Number of processes */
   unsigned long int proc0_size; /* Size of proc 0's subarray */
   unsigned long int prime;
   unsigned long int local_prime; /* Current prime */
   unsigned long int size;        /* Elements in 'marked' */
   int ca;
   unsigned long int local_prime_size;

   MPI_Init(&argc, &argv);

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

   low_value = 2 * BLOCK_LOW(id, p, (n - 3) / 2 + 1) + 3;
   if (low_value % 2 == 0)
   {
      low_value = low_value + 1;
   }
   high_value = 2 * BLOCK_HIGH(id, p, (n - 3) / 2 + 1) + 3;

   if (high_value % 2 == 0)
   {
      high_value = high_value - 1;
   }
   size = ((id + 1) * ((n - 3) / 2 + 1) / (p)) - ((id) * ((n - 3) / 2 + 1) / (p));

   marked = (char *)malloc(size);
   index = 0;
   if (marked == NULL)
   {
      printf("Cannot allocate enough memory\n");
      MPI_Finalize();
      exit(1);
   }
   local_prime_size = ((sqrt(n) - 3) / 2) + 1;
   for (i = 0; i < size; i++)
   {
      marked[i] = 0;
   }
   local_prime_marked = (char *)malloc(((sqrt(n) - 3) / 2) + 1);
   for (i = 0; i < local_prime_size; i++)
      local_prime_marked[i] = 0;
   prime = 3;
   do
   {
      for (i = (prime * prime - 3) / 2; i < local_prime_size; i += prime)
         local_prime_marked[i] = 1;
      while (local_prime_marked[++index])
         ;
      prime = 2 * index + 3;
   } while (prime * prime <= sqrt(n));

   for (local_first = 0; local_first < size; local_first += BLOCK_SIZE)
   {
      index = 0;
      prime = 3;
      do
      {
         if (prime * prime > 2 * ((low_value - 3) / 2 + local_first) + 3)
            first = (prime * prime - 3) / 2 - (2 * ((low_value - 3) / 2 + local_first)) / 2;
         else
         {
            ca = (2 * ((low_value - 3) / 2 + local_first) + 3) % prime;
            if (!ca)
               first = 0;
            else
            {
               first = prime - ca;
               if (!((2 * ((low_value - 3) / 2 + local_first) + 3 + first) % 2))
                  first = (first + prime) / 2;
               else
               {
                  first = first / 2;
               }
            }
         }
         for (i = first + local_first; i < first + local_first + BLOCK_SIZE && i < size; i = i + prime)
         {
            marked[i] = 1;
         }
         while (local_prime_marked[++index])
            ;
         prime = 2 * index + 3;
      } while (prime * prime <= n);
   }

   count = 0;
   for (i = 0; i < size; i++)
      if (!marked[i])
      {
         count++;
      }

   MPI_Reduce(&count, &global_count, 1, MPI_INT, MPI_SUM,
              0, MPI_COMM_WORLD);

   elapsed_time += MPI_Wtime();
   global_count++;

   if (!id)
   {
      printf("The total number of prime: %ld, total time: %10.6f, total node %d\n", global_count, elapsed_time, p);
   }
   MPI_Finalize();
   return 0;
}
