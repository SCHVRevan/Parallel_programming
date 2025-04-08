// compile:
// mpicxx -O test_5.c -o test_5

#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <iostream>
#include <iomanip>
using namespace std;

// Required function
long double f(long double x, long double y) {
	long double res = 0;
	// abs() работает некорректно
	if (x < 0) x *= -1;
	if (y < 0) y *= -1;
	// Выполняется проверка на принадлежность координат области
	// Проверка x + y <= 1 не срабатывает корректно
	if (x + y <= 1.000000000001) res = abs(x * y * (x + y));
	return res;
}

int main(int argc, char **argv) {
   int size, rank, tag = 21;
   long double clock, hx, hy, x, y, buff = 0, res = 0, s = 1, x0 = -1, xn = 1, y0 = -1, yn = 1, N = 100000;
   MPI_Status status;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   hx = (xn - x0) / N;
   hy = (yn - y0) / N;
   s = hx * hy;

   // Старт таймера на процессе с рангом 0
   if (rank == 0) clock = MPI_Wtime();

   // Распределение столбцов сетки по процессам
   for (int i = 0 + rank; i < N; i += size) {
	x = x0 + i*hx + hx/2;
	// Вычисление интеграла по столбцу
	for (int j = 0; j < N; j++) {
		y = y0 + j*hy + hy/2;
		buff = f(x, y);
		res += buff;
	}
   }

   // Сбор всех локальных результатов и формирование итогового значения процессом с рангом 0
   if (rank != 0) MPI_Send(&res, 1, MPI_LONG_DOUBLE, 0, tag, MPI_COMM_WORLD);
   else {
	for (int i = 1; i < size; i++) {
		MPI_Recv(&buff, 1, MPI_LONG_DOUBLE, i, tag, MPI_COMM_WORLD, &status);
		res += buff;
	}
	res *= s;
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
