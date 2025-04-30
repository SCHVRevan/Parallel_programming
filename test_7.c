// compile:
// g++ -fopenmp test_7.c -o test_7

#include <stdio.h>
#include <iostream>
#include <omp.h>
#include <time.h>
#include <math.h>
using namespace std;

void print_mtrx(double** mtrx, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cout << mtrx[i][j] << " ";
		}
		cout << "\n";
	}

	return;
}

int main(int argc, char *argv[]) {
   srand(time(NULL));
   const int n = 2048;
   long int i, j, k;
   double clock;
   double** mtrxA = new double*[n];
   double** mtrxB = new double*[n];
   double** res = new double*[n];
   for (i = 0; i < n; i++) {
	mtrxA[i] = new double[n];
	mtrxB[i] = new double[n];
	res[i] = new double[n];
   }

   for (i = 0; i < n; i++) {
	for (j = 0; j < n; j++) {
		mtrxA[i][j] = 1;
	}
   }

   for (i = 0; i < n; i++) {
	for (j = 0; j < n; j++) {
		mtrxB[i][j] = 1;
	}
   }

   for (i = 0; i < n; i++) {
	for (j = 0; j < n; j++) {
		res[i][j] = 0;
	}
   }

//   long int N = pow(n, 3);
   // Старт таймера на мастер-нити
   clock = omp_get_wtime();

   #pragma omp parallel num_threads(atoi(argv[1])) private(i, j, k)
   {
	#pragma omp single
	{
		cout << "Threads num: " << omp_get_num_threads() << "\n";
	}
	#pragma omp for schedule(static) collapse(3)
/*	// i-j-k
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			for (k = 0; k < n; k++) {
				res[i][j] += mtrxA[i][k] * mtrxB[k][j];
			}
		}
	}
*/
	// k-i-j
	for (k = 0; k < n; k++) {
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				#pragma omp atomic
				res[i][j] += mtrxA[i][k] * mtrxB[k][j];
			}
		}
	}
   }

   clock = omp_get_wtime() - clock;
   cout << "time: " << clock << " s\n";
/*
   cout << "Matrix A:\n";
   print_mtrx(mtrxA, n);

   cout << "Matrix B:\n";
   print_mtrx(mtrxB, n);

   cout << "Result:\n";
   print_mtrx(res, n);
*/

   // Check result correctness
   for (i = 0; i < n; i++) {
	for (j = 0; j < n; j++) {
		if (res[i][j] != n) {
			cout << "Error in res[" << i << "][" << j << "] = " << res[i][j] << "\n\n";
			goto end;
		}
	}
   }

end:
   for (i = 0; i < n; i++) {
	delete [] mtrxA[i];
	delete [] mtrxB[i];
	delete [] res[i];
   }
   delete [] mtrxA;
   delete [] mtrxB;
   delete [] res;

   return 0;
 }
