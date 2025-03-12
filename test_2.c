// compile:
// mpicxx -O test_2.c -o test_2
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <time.h>
#include <iostream>
using namespace std;

int main(int argc, char **argv) {
   int size, rank;
   srand(time(NULL));
   MPI_Status status;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   // Generate a real number from -1 to 1
   double x = cos((rank + 1) * (double)rand() / RAND_MAX);
   double recv_buff[size];
   if (rank == 0) cout << "\nCheck x:\n";
   MPI_Barrier(MPI_COMM_WORLD);

   // Show generated numbers on each process
   cout << "Process " << rank << ": " << x << "\n";
   MPI_Gather(&x, 1, MPI_DOUBLE, recv_buff, 1, MPI_DOUBLE, 2, MPI_COMM_WORLD);

   // Show gathered elements of array on proccess №2
  if (rank == 2) {
    cout << "\nGather check (process " << rank << "):\nrecv[2]: " << recv_buff[2] << "\n";
    for (int i = 0; i < size; i++) {
      if (i != 2) cout << "recv[" << i << "]: " <<  recv_buff[i] << "\n";
    }
    cout << "\n";
  }

  MPI_Finalize();
  return 0;
}
