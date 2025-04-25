// compile:
// g++ -fopenmp test_6.c -o test_6

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <omp.h>
using namespace std;

// Required function
long double f(long double &x, long double &y) {
	long double res = 0;
	if (x < 0) x *= -1;
	if (y < 0) y *= -1;
	// Выполняется проверка на принадлежность координат области
	// Проверка x + y <= 1 не срабатывает корректно
	if (x + y <= 1.000000000001) res = fabs(x * y * (x + y));
	return res;
}

int main(int argc, char *argv[]) {
   int N = 60000;
   double clock;
   long double hx, hy, x, y, buff = 0, res = 0, s = 1, x0 = -1, xn = 1, y0 = -1, yn = 1;

   hx = (xn - x0) / N;
   hy = (yn - y0) / N;
   s = hx * hy;

   // Старт таймера на процессе с рангом 0
   clock = omp_get_wtime();

   omp_set_num_threads(atoi(argv[1]));
   #pragma omp parallel private(x, y)
   {
	#pragma omp single
	{
		cout << "Threads num: " << omp_get_num_threads() << "\n";
	}
	#pragma omp for schedule(guided) collapse(2) reduction(+ : res)
	// Распределение столбцов сетки по процессам
	for (int i = 0; i < N; i++) {
		// Вычисление интеграла по столбцу
		for (int j = 0; j < N; j++) {
			x = x0 + i*hx + hx/2;
			y = y0 + j*hy + hy/2;
			res += f(x, y);
		}
   	}
   }

   res *= s;

   clock = omp_get_wtime() - clock;
   cout << "I = " << setprecision(5) << res << "\n";
   cout << "time = " << clock << " s\n";

   return 0;
 }
