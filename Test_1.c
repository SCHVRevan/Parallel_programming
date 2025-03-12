// компилируется:
// mpicxx -O Test_1.c -o Test_1 
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char **argv) {
   // Объявляю переменные и выделяю память под массив вещественных чисел
   int rank, size;
   double x, sum, buff;
   double y[5];
   srand(time(NULL));
   MPI_Status status;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   // Для каждой копии процесса инициализирую значение x и y
   x = cos((rank + 1) * (double)rand() / RAND_MAX);
   //printf("Process: %d  X: %f\n", rank, x);
   for (int k = 1; k <= 5; k++) {
	y[k-1] = sin(k*x) / (double)k;
	// Сразу проверяю отрицательность элемента и считаю локальную сумму
	if (y[k-1] < 0) sum += y[k-1];
   }
   for (int i = 0; i < 5; i++) printf("Process: %d   y%d = %f\n", rank, i, y[i]);
   // Проверка локальных сумм
   printf("Process %d:  Sum: %f\n", rank, sum);
   // Отправляю локальные суммы процессу с номером 1
   if (rank != 1) MPI_Send(&sum, 1, MPI_DOUBLE, 1, 5, MPI_COMM_WORLD);
   else {
	// "Собираю" суммы воедино
	for (int i = 2; i <= size; i++) {
		MPI_Recv(&buff, 1, MPI_DOUBLE, i % size, 5, MPI_COMM_WORLD, &status);
		sum += buff;
	}
	printf("Process %d:  Sum: %f\n\n", rank, sum);
   }
   MPI_Finalize();
   return 0;
 }
