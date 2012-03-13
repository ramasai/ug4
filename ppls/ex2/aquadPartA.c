#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "stack.h"

#define EPSILON 1e-3
#define F(arg)  cosh(arg)*cosh(arg)*cosh(arg)*cosh(arg)
#define A 0.0
#define B 5.0

#define TAG_NEW_TASK 100
#define TAG_RESULT 200

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
	int numworkers = numprocs - 1;
	int kills_recv = 0;	
	MPI_Status status;

	stack *s = new_stack();
	double initial[2] = { A, B };
	push(initial, s);
	double answer = 0;

	while(1) {
		double* data;
		data = pop(s);

		if(data == NULL)
		{
			return answer;
		}

		fprintf(stdout, "Farmer sending: { %f, %f }\n", data[0], data[1]);
		MPI_Send(data, 2, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);

		if( !is_empty(s)) {
			data = pop(s);
			fprintf(stdout, "Farmer sending: { %f, %f }\n", data[0], data[1]);
			MPI_Send(data, 2, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD);
		}

		fprintf(stdout, "Farmer waiting... 1\n");
		MPI_Recv(data, 2, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		fprintf(stdout, "Farmer getting: { %f, %f } %d\n", data[0], data[1], status.MPI_TAG);

		if (status.MPI_TAG == TAG_NEW_TASK) {
			push(data, s);
			fprintf(stdout, "Farmer waiting... 2\n");
			MPI_Recv(data, 2, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			fprintf(stdout, "Farmer getting: { %f, %f } %d\n", data[0], data[1], status.MPI_TAG);
			push(data, s);
		} else {
			fprintf(stdout, "Farmer getting answer: { %f }\n", data[0]);
			answer += data[0];

			fprintf(stdout, "Farmer has answer: { %f }\n", answer);
		}
	}

	free_stack(s);
}

void worker(int mypid) {
	double data[2];
	double mid, left, right, fmid, larea, rarea, fleft, fright, lrarea;

	while(1) {
		fprintf(stdout, "Worker %d waiting...\n", mypid);
		MPI_Recv(data, 2, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
		fprintf(stdout, "Worker %d got: { %f, %f }\n", mypid, data[0], data[1]);
		usleep(SLEEPTIME);

		left = data[0];
		right = data[1];
		mid = (left + right) / 2;

		fleft = F(left);
		fmid = F(mid);
		fright = F(right);

		larea = (fleft + fmid) * (mid - left) / 2;
		rarea = (fmid + fright) * (right - mid) / 2;
		lrarea = (fleft + fright) * ((right - left)/2);

		if( fabs((larea + rarea) - lrarea) > EPSILON ) {
			// Send the task back to the farmer.
			data[0] = left;
			data[1] = mid;
			fprintf(stdout, "Worker %d sent new task: { %f, %f }\n", mypid, data[0], data[1]);
			MPI_Send(data, 2, MPI_DOUBLE, 0, TAG_NEW_TASK, MPI_COMM_WORLD);

			data[0] = mid;
			data[1] = right;
			fprintf(stdout, "Worker %d sent new task: { %f, %f }\n", mypid, data[0], data[1]);
			MPI_Send(data, 2, MPI_DOUBLE, 0, TAG_NEW_TASK, MPI_COMM_WORLD);
		} else {
			// Send the answer back to the farmer.
			data[0] = larea + rarea;
			fprintf(stdout, "Worker %d sent new result: { %f }\n", mypid, data[0]);
			MPI_Send(data, 2, MPI_DOUBLE, 0, TAG_RESULT, MPI_COMM_WORLD);
		}
	}
}
