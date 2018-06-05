#include <stdio.h>
#include "random.c"
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <math.h>

double get_rand (void) {
	srand(time(NULL));
	double kram = ((double) rand() / RAND_MAX) * 2 - 1;
	return kram;
}

int main (int argc, char* argv[]) {

	double pi;
	int n_threads = atoi(argv[1]);
	long sample = atol(argv[2]);
	long sum = 0;


		long inside = 0;

		#pragma omp parallel for num_threads(n_threads) reduction( + : sum )
		for (long i = 0; i <= sample; i++) {

			double a;
			double b;
			double z;
			a = pr_random_f(1.0);
			b = pr_random_f(1.0);
			//~printf("a= %f, b= %f, counter= %d\n", a, b, counter);
			z = a*a + b*b;
			if ( z <= 1) sum++;
				//inside++;
		}

	pi = (double) sum / sample * 4.0;
	double error = (pi - M_PI) / M_PI;
	printf("rel_err= %f, sum= %ld, loops = %ld, pi = %f\n", error, sum, sample, pi);
}

