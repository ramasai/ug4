#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "stack.h"

#define EPSILON 1e-3
#define F(arg) cosh(arg)*cosh(arg)*cosh(arg)*cosh(arg)
#define A 0.0
#define B 5.0

#define TAG_NEW_TASK 100
#define TAG_RESULT 200
#define TAG_KILL 300
#define TAG_REQUEST_TASK 400
#define TAG_NO_TASK 500
#define TAG_TASK 600

#define SLEEPTIME 1

int *tasks_per_process;

double farmer(int);
void worker(int);

int main(int argc, char **argv ) {
	int i, myid, numprocs;
	double area, a, b;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);

	if(numprocs < 2) {
		fprintf(stderr, "ERROR: Must have at least 2 processes to run\n");
		MPI_Finalize();
		exit(1);
	}

	if (myid == 0) { // Farmer
		// init counters
		tasks_per_process = (int *) malloc(sizeof(int)*(numprocs));
		for (i=0; i<numprocs; i++) {
			tasks_per_process[i]=0;
		}
	}

	if (myid == 0) { // Farmer
		area = farmer(numprocs);
	} else { //Workers
		worker(myid);
	}

	if(myid == 0) {
		fprintf(stdout, "Area=%lf\n", area);
		fprintf(stdout, "\nTasks Per Process\n");
		for (i=0; i<numprocs; i++) {
			fprintf(stdout, "%d\t", i);
		}
		fprintf(stdout, "\n");
		for (i=0; i<numprocs; i++) {
			fprintf(stdout, "%d\t", tasks_per_process[i]);
		}
		fprintf(stdout, "\n");
		free(tasks_per_process);
	}
	MPI_Finalize();
	return 0;
}

double farmer(int numprocs) {
	int numworking = 0;	
	double answer = 0;
	double tasks[numprocs-1][2];

	// Create the tasks.
	double i;
	int task = 0;
	for(i = A; i < B;)
	{
		double bottom = i;
		double top = bottom + (B-A)/(numprocs-1);

		tasks[++task][0] = bottom;
		tasks[task][1] = top;

		i = top;
	}

	while(1) {
		double data[2];

		int flag = 0;
		MPI_Status status;
		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);

		if (flag) {
			if (status.MPI_TAG == TAG_RESULT) {
				// Get the answer
				MPI_Recv(data, 2, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_RESULT, MPI_COMM_WORLD, &status);

				// Add our new answer to the sum.
				answer += data[0];
				tasks_per_process[status.MPI_SOURCE] = data[1];

				if (--numworking == 0)
					break;
			} else if (status.MPI_TAG == TAG_REQUEST_TASK) {
				// If a process is requesting a task then we check if we have any to give before sending
				// an appropriate message back.
				MPI_Recv(data, 2, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_REQUEST_TASK, MPI_COMM_WORLD, &status);
				MPI_Send(tasks[status.MPI_SOURCE], 2, MPI_DOUBLE, status.MPI_SOURCE, TAG_TASK, MPI_COMM_WORLD);

				numworking++;
			}
		}
	}

	// Are we done?
	return answer;
}

double quad(double left, double right, double fleft, double fright, double lrarea, int * callcount) {
	double mid, fmid, larea, rarea;

	mid = (left + right) / 2;
	fmid = F(mid);
	larea = (fleft + fmid) * (mid - left) / 2;
	rarea = (fmid + fright) * (right - mid) / 2;
	if( fabs((larea + rarea) - lrarea) > EPSILON ) {
		larea = quad(left, mid, fleft, fmid, larea, callcount);
		rarea = quad(mid, right, fmid, fright, rarea, callcount);
	}
	*callcount += 1;
	return (larea + rarea);
}

void worker(int mypid) {
	double empty_data[2] = { 0, 0 };
	double data[2];

	MPI_Status status;

	// Get the task.
	MPI_Send(empty_data, 2, MPI_DOUBLE, 0, TAG_REQUEST_TASK, MPI_COMM_WORLD);
	MPI_Recv(data, 2, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	// Slow down.
	usleep(SLEEPTIME);

	// Calculations.
	int callcount = 0;
	double start = data[0];
	double end = data[1];

	double answer = quad(start, end, F(start), F(end), (F(start)+F(end)) * (end-start)/2, &callcount);
	data[0] = answer;
	data[1] = callcount;

	MPI_Send(data, 2, MPI_DOUBLE, 0, TAG_RESULT, MPI_COMM_WORLD);
}

