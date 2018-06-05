#define _GNU_SOURCE
#define USE_MATTSON_RNG
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "random.c"
/*
* Methodendefinition
*/
void * pi(void * sample_thread);
double calcPI(long inside, long n_samples);
double deltaPi(double calculatedPi);

/*
*Struct zur Rückgabe aller Berechneten Ergebnisse
*/
typedef struct data{
	double pi;
	long inside;
	long outside;
} data;

/*
*Main methode aufgerufen mit argc=3 argv[0]== Programmname argv[1]== n_threads argv[2]== n_samples (long >10.000.000.000)
*/
int main(int argc, char* argv[]){
	
	if (argc != 3){ // Fehlerabfrage ob alle argumente existieren;
		fprintf(stderr,"Usage: ./montecarlo <n_threads> <n_samples>\n");
		exit(EXIT_FAILURE);
	}
	int n_threads = atoi(argv[1]); // Zuweisung des Parameter der die Anzahl der Threads bestimmt
	if (n_threads < 1){//Abfangen ob möglich min 1
		fprintf(stderr,"Bitte gebe eine höhere Zahl bei den Threads an");
		exit(EXIT_FAILURE);
	}
	long n_samples = atol(argv[2]);//Zuweisung der Samples
	if (n_samples < 2 * n_threads){//abfangen ob was sinvolles berechnet werden kann
		fprintf(stderr,"Bitte gebe eine höhere Anzahl an n_samples an (min 2x n_threads)");
		exit(EXIT_FAILURE);
	}
	printf("n_threads = %d, n_samples= %ld\n",n_threads,n_samples);// Ausgabe der Threads die benutzt werden und Samples die berechnet werden sollen
	long sample_per_thread = n_samples/n_threads; // berechnen des Min was jeder Thread berechnen soll
	long* sample_todo = (long*)calloc(n_threads, sizeof(long));// genaue angabe pro Tread der zuberechnenden Threads
	if (sample_todo == NULL){// wenn NULL dann nicht genug platz Fehler ausgeben
		fprintf(stderr,"Die Anzahl der Threads ist zu Groß Nicht genug Platz im Speicher");
		exit(EXIT_FAILURE);
	}
	
	int i;// Def der Zählvariable
	for (i =0; i< n_threads; i++){// weise jedem Thread seine minimale zu berechnenden Samples zu
		sample_todo[i] = sample_per_thread; 
		if(i < n_samples % n_threads){
			sample_todo[i]++;
		}
	}
	int err;// Fehler variable
	pthread_t * tid = (pthread_t*)calloc(n_threads, sizeof(pthread_t));
	if (tid == NULL){// Schauen ob alles geklappt hat sonst verfe fehler
		fprintf(stderr,"Die Anzahl der Threads ist zu Groß Nicht genug Platz im Speicher");
		exit(EXIT_FAILURE);
	}
	
	//Erzeugung der Threads mit fehler abfrage
	for (i=0; i< n_threads; i++){
		err = pthread_create (&tid[i], NULL, pi, (void *) (&sample_todo[i]));
		if (err!=0){
			fprintf(stderr,"Error pthread_create(), error code %i\n", err);
			exit(EXIT_FAILURE);
		}
	}
	
	double pi = 0.0; 
	data* erg;
	long inside = 0;// Anzahl der Richtigen Werte
	long outside = 0;//Anzahl der Falschen Werte
	for (i = 0; i < n_threads; i++ ){//Warte auf beenden der Threads mit fehlerabfrage
			err = pthread_join (tid[i], (void*)&erg);
            if (err != 0){
                printf("Error pthread_join(), error code %i\n", err);
                exit(EXIT_FAILURE);
            }
			inside += erg->inside;//Zähle richtige Werte zusammen
			outside += erg->outside;//Zähle falsche Werte zusammen
			printf("Teil-Pi= %f\n",erg->pi);//Gebe Teil-Pi aus 
			free(erg);//gebe Resavierten Speicher wieder frei
	}
	pi = calcPI(inside, n_samples);//Berechne Pi aus allen Samples
	printf("PI = %f\n", pi); 	//Gebe Pi wieder
	printf("Relativer Fehler: %f\n", deltaPi(pi));
	
	free(tid);// Gebe Speicher von den Threads wieder frei
	free(sample_todo); // gebe speicher von den Samples_todo wieder frei
	return EXIT_SUCCESS;
}

double deltaPi(double calculatedPi){
	return (calculatedPi - M_PI) / M_PI; 
}

//Berechne PI quelle: https://de.wikipedia.org/wiki/Monte-Carlo-Algorithmus#Probabilistische_Bestimmung_der_Zahl_Pi
double calcPI(long inside, long samples){
	return 4.0 * ((long double)inside) / samples;
}

void * pi (void * sample_thread){//funktion zur berechnung von PI
	long n_samples = *((long *) sample_thread);//hole Anzahl der vom Thread zu berechnenden Samples
	data* data = malloc(sizeof(data));// Allociere Speicher fürs RückgabeStruct.
	if (data == NULL){// Schauen ob alles geklappt hat sonst verfe fehler
		fprintf(stderr,"Kein freier Speicher mehr vorhanden");
		exit(EXIT_FAILURE);
	}
	data->inside = 0;
	data->outside = 0;
	int i;//Zählvariable
	for (i = 0; i<n_samples; i++){// berechne für n_samples richtig und Falsch + akktuelles PI
		double x = pr_random_f(1); // hole dir die erste Zahl in Range [-1;1]
		double y = pr_random_f(1); // hole dir die zweite Zahl in Range [-1;1]
		double ergebnis = x*x+y*y; // Wende auf die Zwei Zahlen den Satz des Pytagoras an sqrt(a²+b²) = c
		if (ergebnis <= 1){// schaue ob ergebnis <= 1 ist
			data->inside++; //addiere auf inside eins drauf
		} else {
			data->outside++;//addiere auf outside eins drauf
		}
	}
	data->pi = calcPI((data->inside),n_samples);// Berechne Akktuelles Pi
	return data;// gebe die gespeicherten Daten zurück
}