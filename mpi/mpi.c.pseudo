#define ONE 1
#define TWO 2


int MPI_Barrier(MPI_Comm comm){
	int rank;
	int count;
	int err = 0;
	int i;
	err = MPI_Comm_rank(comm, &rank);
	if(err){
		return 1;
	}
	err = MPI_Comm_size(comm, &size);
	if(err){
		return 1;
	}
	if(rank != 0){
		MPI_SEND(NULL, 0, MPI_BYTE, 0, ONE, comm);
		MPI_RECV (NULL, 0, MPI_BYTE, 0, TWO, comm, MPI_STATUSES_IGNORE);
	}else{
		for(i = 0; i < count; i++){
			if(i != rank){
				MPI_RECV (NULL, 0, MPI_BYTE, i, ONE, comm, MPI_STATUSES_IGNORE);
			}
		}
		for(i = 0; i < count; i++){
			if(i != rank){
				MPI_SEND(NULL, 0, MPI_BYTE, i, TWO, comm);
			}
		}
	}
	
	return 0; //Aus pseudocodegründe habe ich mir jetzt gespart das fehlercode auswerten zu machen von send und recv
}
