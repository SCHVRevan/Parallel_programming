// compile:
// mpicxx -O test_3.c -o test_3

#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <iostream>
using namespace std;

int main(int argc, char **argv) {
   int size, rank, q, n = 0, sum = 0, buff = 0, tag = 21;
   int *x;
   MPI_Status status;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   // Entering q from console
   if (rank == 0) {
	cout << "Enter q: ";
	cin >> q;
	n = pow(2, q);
	// Check conditions
	if (size != n / 2) {
		cout << "\nIncorrect number of processors!\n(The number of processors required for execution is n/2)\n\n";
		n = 0;
	}
	for (int i = 1; i < size; i++) MPI_Send(&n, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
   }
   else MPI_Recv(&n, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

   // Exit, if conditions was't meet
   if (n == 0) {
	MPI_Finalize();
	return 0;
   }

   // Initializing array
   x = new int[n];
   if (rank == 0) cout << "Array x:";

   // Filling array with numbers from 0 to n
   for (int i = 0; i < n; i++) x[i] = i;
   if (rank == 0) {
	for (int i = 0; i < n; i++) cout << " " << x[i];
	cout << "\n";
   }

   // Initial local sum
   sum = x[rank*2] + x[rank*2+1];
   cout << "Process " << rank << ":  sum = " << sum << "\n";

   if (rank % 2 != 0) MPI_Send(&sum, 1, MPI_INT, rank - 1, tag, MPI_COMM_WORLD);
   else MPI_Recv(&buff, 1, MPI_INT, rank + 1, tag, MPI_COMM_WORLD, &status);
   cout << "Process " << rank << ":  buff = " << buff << "\n";

   // Iterative part
   for (int i = 2; i < size; i *= 2) {
	if (rank % i == 0) {
		sum += buff;
		cout << "Process " << rank << ":  intermid_sum = " << sum << "\n";
		if ((rank / i) % 2 != 0) MPI_Send(&sum, 1, MPI_INT, rank - i, tag, MPI_COMM_WORLD);
		else if ((rank / i) % 2 == 0) MPI_Recv(&buff, 1, MPI_INT, rank + i, tag, MPI_COMM_WORLD, &status);
	}
   }

   cout << "\n";
   MPI_Barrier(MPI_COMM_WORLD);

   // Result output
   if (rank == 0) {
	sum += buff;
	cout << "Result (process " << rank << "): " << sum << "\n\n";
   }

   // Free memory
   delete[] x;
   MPI_Finalize();
   return 0;
 }
