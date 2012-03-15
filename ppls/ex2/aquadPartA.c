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
	fflush(stdout);
	MPI_Finalize();
	return 0;
}

double farmer(int numprocs) {
	int numworking = 0;	

	stack *s = new_stack();
	double initial[2] = { A, B };
	double answer = 0;

	/*MPI_Send(initial, 2, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);*/
	push(initial, s);

	while(1) {
		double data[2];

		int flag = 0;
		MPI_Status status;
		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);

		if (flag) {
			if (status.MPI_TAG == TAG_NEW_TASK) {
				MPI_Recv(data, 2, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_NEW_TASK, MPI_COMM_WORLD, &status);
				fprintf(stderr, "Farmer: Got new task { %f, %f (%d) } from %d.\n", data[0], data[1], status.MPI_TAG, status.MPI_SOURCE);
				push(data, s);
				tasks_per_process[status.MPI_SOURCE]++;

				MPI_Recv(data, 2, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_NEW_TASK, MPI_COMM_WORLD, &status);
				fprintf(stderr, "Farmer: Got new task { %f, %f (%d) } from %d.\n", data[0], data[1], status.MPI_TAG, status.MPI_SOURCE);
				push(data, s);
				tasks_per_process[status.MPI_SOURCE]++;

				numworking--;
			} else if (status.MPI_TAG == TAG_RESULT) {
				MPI_Recv(data, 2, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_RESULT, MPI_COMM_WORLD, &status);
				fprintf(stderr, "Farmer: Got result { %f (%d) } from %d.\n", data[0], status.MPI_TAG, status.MPI_SOURCE);

				// Add our new answer to the sum.
				answer += data[0];
				/*fprintf(stdout, "New Answer: %f\n", answer);*/

				numworking--;
			} else if (status.MPI_TAG == TAG_REQUEST_TASK) {
				MPI_Recv(data, 2, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_REQUEST_TASK, MPI_COMM_WORLD, &status);
				fprintf(stderr, "Farmer: Got task request (%d) from %d.\n", status.MPI_TAG, status.MPI_SOURCE);

				if (!is_empty(s)) {
					double* new_data = pop(s);
					MPI_Send(new_data, 2, MPI_DOUBLE, status.MPI_SOURCE, TAG_TASK, MPI_COMM_WORLD);
					fprintf(stdout, "Farmer: Sent task { %f, %f } to %d.\n", new_data[0], new_data[1], status.MPI_SOURCE);

					numworking++;
				} else {
					MPI_Send(data, 2, MPI_DOUBLE, status.MPI_SOURCE, TAG_NO_TASK, MPI_COMM_WORLD);
					fprintf(stderr, "Farmer: Sent no task message to %d.\n", status.MPI_SOURCE);
				}
			}
		}

		/*sleep(1);*/

		// Are we done?
		int done = (numworking == 0 && is_empty(s));
		/*fprintf(stdout, "Done: %d\n", done);*/
		/*fprintf(stdout, "Working: %d\n", numworking);*/
		if (done) {
			int i;
			double kill[2];

			for (i = 1; i < numprocs; i++) {
				MPI_Send(kill, 2, MPI_DOUBLE, i, TAG_KILL, MPI_COMM_WORLD);
				fprintf(stderr, "Farmer: Sent kill message to %d.\n", status.MPI_SOURCE);
			}

			free_stack(s);
			return answer;
		}
	}
}

void worker(int mypid) {
	double empty_data[2] = { 0, 0 };
	double data[2];
	double mid, left, right, fmid, larea, rarea, fleft, fright, lrarea;
	MPI_Status status;

	while(1) {
		// Get the task.
		MPI_Send(empty_data, 2, MPI_DOUBLE, 0, TAG_REQUEST_TASK, MPI_COMM_WORLD);
		MPI_Recv(data, 2, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		fprintf(stderr, "Worker %d: Got { %f, %f (%d) } from %d.\n", mypid, data[0], data[1], status.MPI_TAG, status.MPI_SOURCE);
		
		// If the task tells us to die: die.
		if (status.MPI_TAG == TAG_KILL) { break; }
		if (status.MPI_TAG == TAG_NO_TASK) { continue; }

		// Slow down.
		usleep(SLEEPTIME);

		// Calculations.
		left = data[0];
		right = data[1];
		mid = (left + right) / 2;

		fleft = F(left);
		fmid = F(mid);
		fright = F(right);

		larea = (fleft + fmid) * (mid - left) / 2;
		rarea = (fmid + fright) * (right - mid) / 2;
		lrarea = (fleft + fright) * ((right - left)/2);

		// Are we accurate enough yet?
		if( fabs((larea + rarea) - lrarea) > EPSILON ) {
			// Send the tasks back to the farmer.
			data[0] = left;
			data[1] = mid;
			MPI_Send(data, 2, MPI_DOUBLE, 0, TAG_NEW_TASK, MPI_COMM_WORLD);
			fprintf(stderr, "Worker %d: Sent { %f, %f } to %d.\n", mypid, data[0], data[1], 0);

			data[0] = mid;
			data[1] = right;
			MPI_Send(data, 2, MPI_DOUBLE, 0, TAG_NEW_TASK, MPI_COMM_WORLD);
			fprintf(stderr, "Worker %d: Sent { %f, %f } to %d.\n", mypid, data[0], data[1], 0);
		} else {
			// Send the answer back to the farmer.
			data[0] = larea + rarea;
			data[1] = -1;
			MPI_Send(data, 2, MPI_DOUBLE, 0, TAG_RESULT, MPI_COMM_WORLD);
		}
	}
}
