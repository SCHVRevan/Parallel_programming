// компилируется с использованием mpicxx для поддержки библиотеки math.h
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char **argv) {
   // Объявляю переменные и выделяю память под массив вещественных чисел
   int rank, size;
   double x, sum, buff, y[5];
   srand(time(NULL));
   MPI_Status status;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   // Для каждой копии процесса инициализирую значение x и y
   // Приведение типов необходимо для получения вещественного результата
   x = cos((rank + 1) * (double)rand() / RAND_MAX);
   for (int k = 1; k <= 5; k++) {
	y[k-1] = sin(k*x) / (double)k;
	// Сразу же проверяю отрицательность элемента и считаю локальную сумму
	if (y[k-1] < 0) sum += y[k-1];
   }
   // Отправляю локальные суммы процессу с номером 1
   if (rank != 1) MPI_Send(&sum, 1, MPI_DOUBLE, 1, 5, MPI_COMM_WORLD);
   else {
	// "Собираю" суммы воедино
	for (int i = 2; i <= size; i++) {
		MPI_Recv(&buff, 1, MPI_DOUBLE, i % size, 5, MPI_COMM_WORLD, &status);
		sum += buff;
	}
	printf("Process %d:  Sum: %f\n", rank, sum);
   }
   MPI_Finalize();
   return 0;
 }
