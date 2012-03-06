#include <stdio.h>
#include <pthread.h>
#define P 8

void *sayhello(void *id) {
	printf("Hello from thread %d\n", (int)id);
}

int main(int argc, char *argv[]) {
	int i;
	pthread_t thread[P];
	printf("Hello from the main thread\n");

	for (i = 0; i < P; i++) {
		pthread_create(&thread[i], NULL, sayhello, (void *)i);
	}

	for (i = 0; i < P; i++) {
		pthread_join(thread[i], NULL);
	}

	printf("Goodbye from the main thread\n");
}
