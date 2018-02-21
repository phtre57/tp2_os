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
#include <sched.h>

#define NUMBER_OF_THREADS 5
#define SCHEDULER_TYPE 0

void *work(void *tid){
	while(1);
	pthread_exit(NULL);
}

void change_scheduler(int scheduler){
	int ret;
	if (scheduler == 0){
		//FIFO sched
		struct sched_param fifo_param;
		int min_max_priority = (sched_get_priority_max(SCHED_FIFO) + sched_get_priority_min(SCHED_FIFO));
		int average_priority = min_max_priority / 2;
		fifo_param.sched_priority = average_priority;
		ret = sched_setscheduler(0, SCHED_FIFO, &fifo_param);
		printf("errno number: %d\n", errno);
	}

	else if (scheduler == 1){
		//RR sched
		struct sched_param rr_param;
		int min_max_priority = (sched_get_priority_max(SCHED_RR) + sched_get_priority_min(SCHED_RR));
		int average_priority =  min_max_priority / 2;
		rr_param.sched_priority = average_priority;
		ret = sched_setscheduler(0, SCHED_RR, &rr_param);
		printf("errno number: %c\n", errno);
	}

	else{
		//No sched
		printf("No valid scheduler");
	}

}

int main(int argc, char *argv[]){
	pthread_t threads[NUMBER_OF_THREADS];
	int priority_increment = 0;
	int priority_value = 0;
	change_scheduler(SCHEDULER_TYPE);

	for(int i = 0; i < NUMBER_OF_THREADS; i++){
		pthread_create(&threads[i], NULL, work, (void*)i);
		threads[i] = syscall(SYS_gettid);
		int ret = setpriority(PRIO_PROCESS, threads[i], priority_value);
		printf("thread number: %d\n", i);
		printf("ret for that thread: %d\n", ret);
		printf("errno for that thread: %d\n\n", errno);
		priority_value += priority_increment;
	}

	pthread_exit(NULL);

	return 0;
}