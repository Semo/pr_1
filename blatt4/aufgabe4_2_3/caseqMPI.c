#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <mpi.h>

#include "random.h"
#include "md5tool.h"

/* horizontal size of the configuration */
#define XSIZE 1024

/* "ADT" State and line of states (plus border) */
typedef char State;
typedef State Line[XSIZE + 2];

/* determine random integer between 0 and n-1 */
#define randInt(n) ((int)(nextRandomLEcuyer() * n))

/* random starting configuration */
static void initConfig(Line *buf, int lines){
	int x, y;
	
	initRandomLEcuyer(424243);
	for (y = 1;  y <= lines;  y++) {
		for (x = 1;  x <= XSIZE;  x++) {
			buf[y][x] = randInt(100) >= 50;
		}
	}
}

/* annealing rule from ChoDro96 page 34 
 * the table is used to map the number of nonzero
 * states in the neighborhood to the new state
 */
static State anneal[10] = {0, 0, 0, 0, 1, 0, 1, 1, 1, 1};

/* a: pointer to array; x,y: coordinates; result: n-th element of anneal,
      where n is the number of neighbors */
#define transition(a, x, y) \
   (anneal[(a)[(y)-1][(x)-1] + (a)[(y)][(x)-1] + (a)[(y)+1][(x)-1] +\
           (a)[(y)-1][(x)  ] + (a)[(y)][(x)  ] + (a)[(y)+1][(x)  ] +\
           (a)[(y)-1][(x)+1] + (a)[(y)][(x)+1] + (a)[(y)+1][(x)+1]])

/* treat torus like boundary conditions */
static void boundary(Line *buf, int lines){
	int x,y;
	for (y = 0;  y <= lines+1;  y++) {
		/* copy rightmost column to the buffer column 0 */
		buf[y][0      ] = buf[y][XSIZE];

		/* copy leftmost column to the buffer column XSIZE + 1 */
		buf[y][1+1] = buf[y][1    ];
	}
	
	for (x = 0;  x <= XSIZE+1;  x++) {
		/* copy bottommost row to buffer row 0 */
		buf[0][x      ] = buf[lines][x];

		/* copy topmost row to buffer row lines + 1 */
		buf[lines+1][x] = buf[1][x    ];
	}
}
/* make one simulation iteration with lines lines.
 * old configuration is in from, new one is written to to.
 */
 
 //umschreiben
 /**
static void simulate(Line *from, Line *to, int lines){
	
  
	boundary(from, lines); 
	for (y = 1;  y <= lines;  y++) {
		for (x = 1;  x <= XSIZE;  x++) {
			to[y][x  ] = transition(from, x  , y);
		}
	}
}
*/

/* --------------------- measurement ---------------------------------- */

int main(int argc, char** argv){
	int lines, its;
	int i;
	Line *from, *to, *temp, *next;
	char* hash;
	
	assert(argc == 3);
	
	lines = atoi(argv[1]);
	its   = atoi(argv[2]);
	from = malloc((lines + 2) * sizeof(Line));
	to   = malloc((lines + 2) * sizeof(Line));
	
	MPI_Init(NULL, NULL);
	// Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	
	if(rank == 0){
		next = malloc((lines + 2) * sizeof(Line));
		initConfig(from, lines);
		for (i = 0;  i < its;  i++) {
			int x,y;
			boundary(from, lines);
			int z;
			for(z = 0; z < world_size;z++){
				if(z !=world_rank ){
					MPI_SEND(from,(lines + 2) * sizeof(Line),MPI_CHARACTER,z,0,MPI_COMM_WORLD);
				}
			}
			MPI_Status status;
			for(z = 0; z < world_size;z++){
				if(z !=world_rank ){
					MPI_RECIVE(next,(lines + 2) * sizeof(Line),MPI_CHARACTER,z,1,&status);
					if(status == MPI_ERROR){
						//TODO
						MPI_Abort(MPI_COMM_WORLD,1);
					}
					
					for (y = 1;  y <= (lines%world_size+lines/world_size);  y++) {
						to[y*z] = next[y*z];
					}
					
				}
			}
		
			temp = from;  
			from = to;  
			to = temp;
	}

	hash = getMD5DigestStr(from[1], sizeof(Line) * (lines));
	printf("hash: %s\n", hash);
		free(next);
	}else{
		
		MPI_Status status;
		for(i = 0;  i < its;  i++){
			MPI_RECIVE(from,(lines + 2) * sizeof(Line),MPI_CHARACTER,0,0,&status);
			if(status == MPI_ERROR){
				MPI_Abort(MPI_COMM_WORLD,2);
			}
			for (y = 1;  y <= (lines%world_size+lines/world_size);  y++) {
				for (x = 1;  x <= XSIZE;  x++) {
					to[y*world_rank][x  ] = transition(from, x  , y*world_rank);
				}
			}
			MPI_SEND(to,(lines + 2) * sizeof(Line),MPI_CHARACTER,0,1,MPI_COMM_WORLD)
			
		}
		
		
	}
	
	
	
	MPI_Finalize();
	
	free(from);
	free(to);
	free(hash);
	return 0;
}