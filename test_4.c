// compile:
// mpicxx -O test_4.c -o test_4

// 18. f(x) = 1 / pow(x (0.2*x + 1), 3/2), a = 1, b = 2.
// Трапеции: I = h * sum(1, n-1)f(xi) + h*(f(a)+f(b)/2)

#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <iostream>
#include <iomanip>
using namespace std;

// Required function
long double f(long double x) {
	long double res = 1 / pow(x * (0.2 * x + 1), (long double)3/2);
	return res;
}

int main(int argc, char **argv) {
   int size, rank, tag = 21;
   long double clock, buff, x, res = 0, a = 1, b = 2;
   MPI_Status status;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   long double h = (b - a) / 1000000;
   // Start timer on process 0
   if (rank == 0) clock = MPI_Wtime();

   for (int i = 1; i < 1000000; i++) {
	if (rank == i % size) {
		x = a + h * i;
		res += f(x);
	}
   }

   if (rank != 0) MPI_Send(&res, 1, MPI_LONG_DOUBLE, 0, tag, MPI_COMM_WORLD);
   else {
	for (int i = 1; i < size; i++) {
		MPI_Recv(&buff, 1, MPI_LONG_DOUBLE, i, tag, MPI_COMM_WORLD, &status);
		res += buff;
	}
   }

   // Constructing result on process 0
   if (rank == 0) {
	res *= h;
	res += h * ((f(a) + f(b)) / 2);
 	clock = MPI_Wtime() - clock;
	cout << "Process " << rank << ":    I = " << setprecision(10) << res << "\n";
	cout << "Process 0: time = " << clock << " s\n";
   }

   MPI_Finalize();
   return 0;
 }
