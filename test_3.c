// compile:
// mpicxx -O test_3.c -o test_3

#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <iostream>
using namespace std;

int main(int argc, char **argv) {
   int size, rank, sum, tmp, q = 0, buff = 0, tag = 21;
   MPI_Status status;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   sum = rank;
   tmp = size;
   // Check, if entered n = 2^q
   while (tmp % 2 == 0) {
	tmp = tmp / 2;
	q++;
   }

   // Exit, if conditions was't meet
   if (pow(2, q) != size) {
	if (rank == 0) cout << "Incorrect number of processors (n != 2^q)\n";
	MPI_Finalize();
	return 0;
   }

   // Iterative part
   for (int k = 0; k < q; k++) {
	if (rank + pow(2, k) < size) {
		MPI_Send(&sum, 1, MPI_INT, rank + pow(2, k), tag, MPI_COMM_WORLD);
//		cout << "Process " << rank << " sending to " << rank + pow(2, k) << "\n";
	}
	if (rank - pow(2, k) > 0) {
		MPI_Recv(&buff, 1, MPI_INT, rank - pow(2, k), tag, MPI_COMM_WORLD, &status);
		cout << "Process " << rank << ":  " << sum << " + " << buff << "\n";
		sum += buff;
	}
   }

   MPI_Barrier(MPI_COMM_WORLD);
   cout << "Process " << rank << ":  sum = " << sum << "\n";

   MPI_Finalize();
   return 0;
 }
