#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUMBER_OF_THREADS 5
#define PRIORITY_INCREMENT 2

void *work(void *tid){
	while(1);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){
	pthread_t threads[NUMBER_OF_THREADS];
	int increment = 0;

	for(int i = 0; i < NUMBER_OF_THREADS; i++){
		pthread_create(&threads[i], NULL, work, (void*)i);
		threads[i] = syscall(SYS_gettid);
		int ret = setpriority(PRIO_PROCESS, threads[i], increment);
		printf("thread number: %d\n", i);
		printf("ret for that thread: %d\n", ret);
		printf("errno for that thread: %d\n\n", errno);
		increment += PRIORITY_INCREMENT;

	}

	pthread_exit(NULL);

	return 0;
}