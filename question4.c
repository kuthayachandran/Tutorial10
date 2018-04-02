#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define MASTER 0

#define RANGE_START 1
#define RANGE_END 1000

void master(int n_proc){
  int size = 0, r = 0, n_sent = 0, n_recv = 0;
  int result[size];
  int range[2] = {0};
	MPI_Status status;

	int range_end = floor(RANGE_END / (n_proc - 1));
	size = range_end - RANGE_START;

  for (int i = 1; i < n_proc; ++i){
    // Sends data, of type double to process 'i'
    r = (i - 1) * size + i;
    range[0] = r;
    range[1] = r + size;

    MPI_Send(&range, 2, MPI_INT, i, n_sent, MPI_COMM_WORLD);
    n_sent++;
  }

  for (int i = 0; i < n_proc - 1; ++i){
    MPI_Recv(result, size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    // Get process tag that sent the data and send it the next chunk
    n_recv = status.MPI_TAG;

    // Copy the results from the slave into the results array
    for (int i = 0; i < size; ++i){
      if(result[i] != 0)
      	printf("%d - ", result[i]);
    }
  }
}

void slave(int proc_id){
  int range[2] = {0, 0};
  int n_recv = 0;
  int num_primes = 0;
  bool prime = true;
  MPI_Status status;

  // Receive the chunk to calculate from MASTER
  MPI_Recv(range, 2, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  n_recv = status.MPI_TAG;

  int size = range[1] - range[0];
  int res[size];
  for(int i = 0; i < size; i++)
  	res[i] = 0;

  for(int i = range[0]; i < range[1]; i++){
  	prime = true;
  	for(int j = 2; j <= i/2; j++){
  		if(i % j == 0)
  			prime = false;
  	}
  	if(prime && (i > 1)){
  		num_primes = num_primes + 1;
  		res[num_primes] = i;
  	}
  }
  // Send the results back
  MPI_Send(res, size, MPI_INT, MASTER, n_recv, MPI_COMM_WORLD);
}

int main(int argc, char* argv[]){
    int proc_id;
    int n_proc;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);

    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);


    if (proc_id == MASTER)
    {
        master(n_proc);
    }
    else
    {
        slave(proc_id);
    }

    MPI_Finalize();
}
