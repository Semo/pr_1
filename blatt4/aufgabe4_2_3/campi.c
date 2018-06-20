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

int getEnd(int rank, int size, int lines);

int getStart(int rank, int size, int lines){
	if(rank > 0){
		return getEnd(rank-1, size, lines)+1;
	}else{
		return 1;
	}
}

int getEnd(int rank, int size, int lines){
	int start = getStart(rank,size,lines);
	int end = start + lines/size;
	if(rank >= lines% size){
		end = end-1;
	}
	return end;
}

void handleError(int ierr){
	int errclass, resultlen;
	char err_buffer[MPI_MAX_ERROR_STRING];
	if (ierr != MPI_SUCCESS) {
		MPI_Error_class(ierr,&errclass);
		if (errclass== MPI_ERR_RANK) {
			fprintf(stderr,"Invalid rank used in MPI send call\n");
			MPI_Error_string(ierr,err_buffer,&resultlen);
			fprintf(stderr,"%s",err_buffer);
			MPI_Finalize();             /* abort*/
		}
	}
}

int main(int argc, char** argv){
	int lines, its;
	int x,y;
	int i, ierr;
	Line *from, *to, *temp;
	char* hash;
	
	assert(argc == 3);
	
	lines = atoi(argv[1]);
	its   = atoi(argv[2]);
	from = malloc((lines + 2) * sizeof(Line));
	to   = malloc((lines + 2) * sizeof(Line));
	
	
	MPI_Init(NULL, NULL);
	
	
	MPI_Status status;
	
	// Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	
	MPI_Comm_set_errhandler(MPI_COMM_WORLD,MPI_ERRORS_RETURN);
	
	initConfig(from, lines);
	
	int start = getStart(world_rank,world_size,lines);
	int end = getEnd(world_rank,world_size,lines);
	
	int vor = (world_rank == 0)? world_size-1: world_rank-1;
	int nach = (world_rank == world_size-1)? 0: world_rank+1;
	printf("Size: %d\n",sizeof(from)/sizeof(State));
	for (i = 0;  i < its;  i++) {
		for (y = start-1;  y <= end+1;  y++) {
			/* copy rightmost column to the buffer column 0 */
			from[y][0      ] = from[y][XSIZE];

			/* copy leftmost column to the buffer column XSIZE + 1 */
			from[y][1+1] = from[y][1    ];
		}
		
		if(world_rank == world_size-1){
			MPI_Recv(&(from[start-1]), sizeof(Line),MPI_CHAR,vor,99,MPI_COMM_WORLD,&status);
			ierr =MPI_Send(&(from[end]),sizeof(Line),MPI_CHAR,nach,99,MPI_COMM_WORLD);
			handleError(ierr);
			MPI_Recv(&(from[end+1]), sizeof(Line),MPI_CHAR,nach,99,MPI_COMM_WORLD,&status);
			ierr =MPI_Send(&(from[start]),sizeof(Line),MPI_CHAR,vor,99,MPI_COMM_WORLD);
			handleError(ierr);
		}else{
			ierr =MPI_Send(&(from[end]),sizeof(Line),MPI_CHAR,nach,99,MPI_COMM_WORLD);
			handleError(ierr);
			MPI_Recv(&(from[start-1]), sizeof(Line),MPI_CHAR,vor,99,MPI_COMM_WORLD,&status);
			ierr =MPI_Send(&(from[start]),sizeof(Line),MPI_CHAR,vor,99,MPI_COMM_WORLD);
			handleError(ierr);
			MPI_Recv(&(from[end+1]), sizeof(Line),MPI_CHAR,nach,99,MPI_COMM_WORLD,&status);
		}
		
		for (y = start;  y <= end;  y++) {
			printf("%s, %d\n",from[y],y);
				for (x = 1;  x <= XSIZE;  x++) {
					to[y][x] = transition(from, x, y);
				}
			}
		temp = from;  
		from = to;  
		to = temp;
	}
	if(world_rank == 0){
		for(i = 0; i < world_size;i++){
			if(i != world_rank){
				int starti = getStart(i,world_size,lines);
				int endi = getEnd(i,world_size,lines);
				int difi = endi - starti+1;
				MPI_Recv(&(from[starti]), difi *sizeof(Line),MPI_CHAR,i,99,MPI_COMM_WORLD,&status);
			}	
		}
		hash = getMD5DigestStr(from[1], sizeof(Line) * (lines));
		printf("hash: %s\n", hash);
	}else{
		int dif = end - start+1;
		ierr =MPI_Send(&(from[start]),dif *sizeof(Line),MPI_CHAR,0,99,MPI_COMM_WORLD);
		handleError(ierr);
	}
	
	
	
	MPI_Finalize();
	
	free(from);
	free(to);
	free(hash);
	return 0;
}
