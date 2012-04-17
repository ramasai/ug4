/**
 * aquadPartA.c
 * 0824586
 *
 * Only three different MPI functions were used in the implementation of this
 * answer: MPI_Send, MPI_Recv, and MPI_Iprobe. The first two are obvious in
 * their usage. However, MPI_Iprobe is used to grab the presence and tag of an
 * incoming message. The main purpose of this is to not block if there is no
 * message (which may occur at the end of the computation) causing the program
 * not to exit.
 *
 * Farmer
 * ------
 * 
 * The farmer initially pushes the first task onto the stack before entering
 * the main work loop. The first thing it does in the loop is probe the
 * MPI_COMM_WORLD communication channel for any new messages. This call is used
 * in order to find the tag of the message without blocking and taking the sent
 * message out of the buffer. If a message is detected then the program checks
 * the tag of the incoming message and then takes different steps depending on
 * the value:
 *
 * 	 TAG_NEW_TASK - This tag is sent by the worker when it is delivering two
 * 	 new tasks back to the farmer. The two new tasks are removed from the
 * 	 message buffer before being pushed on to the stack. Some housekeeping
 * 	 tasks are then performed such as decrementing the count of the number of
 * 	 working workers and incrementing the task counts for the process.
 *
 *   TAG_RESULT - This tag is sent by the worker if it is sending a final
 *   result back to the farmer. The final result is taken from the message
 *   buffer before being added onto the final answer sum. The count of working
 *   workers is then decremented.
 *
 *   TAG_REQUEST_TASK - This tag is sent by the worker if it needs more work.
 *   The farmer removes the message from the buffer before checking if there is
 *   any work on the stack. If there is then this is popped and then sent back
 *   to the worker with the tag TAG_TASK before the number of working workers
 *   is incremented and if there isn't any work to do then it is sent a message
 *   with tag TAG_NO_TASK.
 *
 * After this is done (or no message was found in the probe call) then we check
 * to see if we are done with the overall task. This is done by checking to see
 * if there are no workers working *and* if there is no more work to do on the
 * stack. If we aren't done then we start the loop all over again. However, if
 * we are done then a message is sent to all workers to die (with the tag
 * TAG_KILL) before we return the answer to the caller.
 *
 * Worker
 * ------
 *
 * The worker is significantly simpler in its operation. It enters the main
 * work loop immediately and then asks the farmer for some work before
 * performing a blocking receive until it gets an answer back (a blocking
 * operation was chosen as there is no point continuing until we have something
 * to act upon). The worker observes the tag of the message it receives and
 * then acts in different ways depending on its value:
 *
 *   TAG_KILL - The worker exits by returning from the function.
 *
 *   TAG_NO_TASK - The worker restarts its main work loop again.
 *
 *   Any other tag (TAG_TASK) - The worker performs the calculation using the
 *   task descriptor that it has received. If the value is accurate enough then
 *   the result is accurate enough then the result is sent back to the farmer
 *   with the tag TAG_RESULT. If the answer is not accurate enough then two new
 *   task descriptors are sent back to the farmer with the tag TAG_NEW_TASK.
 */

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

	stack *s = new_stack();
	double initial[2] = { A, B };
	push(initial, s);

	while(1) {
		double data[2];

		int flag = 0;
		MPI_Status status;
		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);

		if (flag) {
			if (status.MPI_TAG == TAG_NEW_TASK) {
				// Get the new tasks.
				MPI_Recv(data, 2, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_NEW_TASK, MPI_COMM_WORLD, &status);
				push(data, s);
				tasks_per_process[status.MPI_SOURCE]++;

				MPI_Recv(data, 2, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_NEW_TASK, MPI_COMM_WORLD, &status);
				push(data, s);
				tasks_per_process[status.MPI_SOURCE]++;

				numworking--;
			} else if (status.MPI_TAG == TAG_RESULT) {
				// Get the answer
				MPI_Recv(data, 2, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_RESULT, MPI_COMM_WORLD, &status);

				// Add our new answer to the sum.
				answer += data[0];

				numworking--;
			} else if (status.MPI_TAG == TAG_REQUEST_TASK) {
				// If a process is requesting a task then we check if we have any to give before sending
				// an appropriate message back.
				MPI_Recv(data, 2, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_REQUEST_TASK, MPI_COMM_WORLD, &status);

				if (!is_empty(s)) {
					// Send the task.
					double* new_data = pop(s);
					MPI_Send(new_data, 2, MPI_DOUBLE, status.MPI_SOURCE, TAG_TASK, MPI_COMM_WORLD);

					numworking++;
				} else {
					// Send back a message saying no tasks.
					MPI_Send(data, 2, MPI_DOUBLE, status.MPI_SOURCE, TAG_NO_TASK, MPI_COMM_WORLD);
				}
			}
		}

		// Are we done?
		int done = (numworking == 0 && is_empty(s));
		if (done) {
			// Kill all of the workers.
			int i;
			double kill[2];

			for (i = 1; i < numprocs; i++) {
				MPI_Send(kill, 2, MPI_DOUBLE, i, TAG_KILL, MPI_COMM_WORLD);
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

			data[0] = mid;
			data[1] = right;
			MPI_Send(data, 2, MPI_DOUBLE, 0, TAG_NEW_TASK, MPI_COMM_WORLD);
		} else {
			// Send the answer back to the farmer.
			data[0] = larea + rarea;
			data[1] = -1;
			MPI_Send(data, 2, MPI_DOUBLE, 0, TAG_RESULT, MPI_COMM_WORLD);
		}
	}
}
