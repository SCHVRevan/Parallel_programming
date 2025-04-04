// compile:
// mpicxx -O test_5.c -o test_5

// 4. f(x, y) = | xy(x + y)|, D = {|x| + |y| <= 1}
// Метод ячеек: sum(i, N)(f(x[i], y[i])*S[i])

#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <iostream>
#include <iomanip>
using namespace std;

// Required function
long double f(long double x, long double y) {
	long double res = 1;
	if (abs(x) + abs(y) <= 1) res = abs(x * y * (x + y));
	return res;
}

int main(int argc, char **argv) {
   int size, rank, tag = 21;
   long double clock, buff, hx, hy, x, y, res = 0, s = 1, x0 = -1, xn = 1, y0 = -1, yn = 1, N = 100000;
   MPI_Status status;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   hx = (xn - x0) / N;
   hy = (yn - y0) / N;
   s = hx * hy;

   // Start timer on process 0
   if (rank == 0) clock = MPI_Wtime();

   for (int i = 0 + rank; i < N; i += size) {
	x = x0 + i*hx + hx/2;
	for (int j = 0; j < N; j++) {
		y = y0 + j*hy + hy/2;
		buff = f(x, y);
		if (buff < 1) res += buff * s;
	}
   }

   // Sending all local results to process 0
   if (rank != 0) MPI_Send(&res, 1, MPI_LONG_DOUBLE, 0, tag, MPI_COMM_WORLD);
   else {
	for (int i = 1; i < size; i++) {
		MPI_Recv(&buff, 1, MPI_LONG_DOUBLE, i, tag, MPI_COMM_WORLD, &status);
		res += buff;
	}
   }

   // Constructing result on process 0
   if (rank == 0) {
 	clock = MPI_Wtime() - clock;
	cout << "Process " << rank << ":    I = " << setprecision(5) << res << "\n";
	cout << "Process 0: time = " << clock << " s\n";
   }

   MPI_Finalize();
   return 0;
 }
