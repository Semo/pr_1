#include <stdio.h>
#include "random.c"
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <math.h>

int main (int argc, char* argv[]) {

	double pi;
	long sum = 0;

	if (argc != 3){ // Fehlerabfrage ob alle argumente existieren;
		fprintf(stderr,"Usage: ./pi_omp <n_threads> <n_samples>\n");
		exit(EXIT_FAILURE);
	}
	int n_threads = atoi(argv[1]); // Zuweisung des Parameter der die Anzahl der Threads bestimmt
	if (n_threads < 1){//Abfangen ob möglich min 1
		fprintf(stderr,"Bitte gebe eine höhere Zahl bei den Threads an");
		exit(EXIT_FAILURE);
	}
	long sample = atol(argv[2]);//Zuweisung der Samples
	if (sample < 2 * n_threads){//abfangen ob was sinvolles berechnet werden kann
		fprintf(stderr,"Bitte gebe eine höhere Anzahl an n_samples an (min 2x n_threads)");
		exit(EXIT_FAILURE);
	}

		//long inside = 0;

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
