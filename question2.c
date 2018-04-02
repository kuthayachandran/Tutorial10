#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define MASTER 0
#define SLAVE 1

#define DATA_SIZE 10000000
#define NUM 1000

void master(int n_proc){
    double *data;
    double *received;
    double startTime;
    double endTime;

    data = (double*)malloc(sizeof(double)*DATA_SIZE);
    received = (double*)malloc(sizeof(double)*DATA_SIZE);

    for(int i = 0; i < DATA_SIZE; ++i)
        data[i] = i;

   	startTime = MPI_Wtime();

   	for(int i = 0; i < NUM; i++){
	    MPI_Send(&data[0], DATA_SIZE, MPI_DOUBLE, SLAVE, 0, MPI_COMM_WORLD);
	    MPI_Recv(&received[0], DATA_SIZE, MPI_DOUBLE, SLAVE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	  }

    endTime = MPI_Wtime();
    printf("Time: %f\n", endTime-startTime);
}


void slave(int proc_id){
    double *received;
	  received = (double*)malloc(sizeof(double)*DATA_SIZE);

   	for(int i = 0; i < NUM; i++){
	    MPI_Recv(&received[0], DATA_SIZE, MPI_DOUBLE, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	    MPI_Send(&received[0], DATA_SIZE, MPI_DOUBLE, MASTER, 0, MPI_COMM_WORLD);
	}
}


int main (int argc, char* argv[]){
    int proc_id;
    int n_proc;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);

    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    if(proc_id == MASTER){
        master(n_proc);
    } else if(proc_id == SLAVE){
        slave(proc_id);
    }

    MPI_Finalize();
}
