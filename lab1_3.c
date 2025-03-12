// компилируется:
// mpicxx -O lab1_3.c -o lab1_3
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <iostream>
using namespace std;

int main(int argc, char **argv) {
   int size, rank, i, j, tmp = 0;
   srand(time(NULL));
   MPI_Status status;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   int x_global[size], loc_res[size], recvbuffer[size];
   int A_global[size][size];
   if (rank == 0) {
     cout << "\nВектор-столбец x:\n";
     // Заполнение вектора случайными значениями
     for (i = 0; i < size; i++) {
       x_global[i] = rand() % 10;
       cout << x_global[i] << " ";
     }
     cout << "\n\nМатрица A:\n";
     // Заполнение матрицы случайными значениями
     for (i = 0; i < size; i++) {
       for (j = 0; j < size; j++) {
         A_global[i][j] = rand() % 10;
         cout << A_global[i][j] << " ";
       }
       cout << "\n";
     }
   }

   MPI_Bcast(x_global, size, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Scatter(A_global, size, MPI_INT, recvbuffer, size, MPI_INT, 0, MPI_COMM_WORLD);
   for (i = 0; i < size; i++) tmp += x_global[i] * recvbuffer[i];
   MPI_Allgather(&tmp, 1, MPI_INT, loc_res, 1, MPI_INT, MPI_COMM_WORLD);

   // Проверка полученного вектора на каждом процессе
   cout << "Process: " << rank << "   loc_res =";
   for (i = 0; i < size; i++) cout << " " << loc_res[i];
   cout << "\n";

   MPI_Barrier(MPI_COMM_WORLD);
   // Последовательное умножение на 0 процессе
   if (rank == 0) {
     // Очистка локального массива
     for (i = 0; i < size; i++) loc_res[i] = 0;
     for (i = 0; i < size; i++) {
       for (j = 0; j < size; j++) loc_res[i] += x_global[j] * A_global[i][j];
     }
     cout << "\nРезультат последовательного умножения\nProcess: " << rank << "   loc_res = ";
     for (i = 0; i < size; i++) cout << loc_res[i] << " ";
     cout << "\n";
   }
   MPI_Finalize();
   return 0;
 }
