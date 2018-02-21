#include <asm-generic/mman.h>
#include <errno.h>
#include <limits.h>
#include <linux/futex.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sysexits.h>
#include <time.h>
#include <unistd.h>

// instruction pour compiler : gcc q2lock.c -o q2lock -lpthread

#define N_ITER 10000000
// variables globales (donc partagées entre threads)
long count = 0;

// ------------- Differents verrous pour le tp2 -------------
int lock = 0;       // Un verrou entier. 0 signifie qu'il est libre
int atomicLock = 0; // Le verrou atomique. 0 signifie qu'il est libre
pthread_mutex_t monMutex;

// http://en.wikipedia.org/wiki/Spinlock
static inline int xchg(int *ptr, int x)
{
    asm volatile("lock\n" "xchgl %0,%1\n"
                 :"=r" (x), "+m" (*(int*)ptr)
                 :"0" (x)
                 :"memory");
    return x;
}

// -------- à implémenter ----------
void BadLock() {
    while (lock == 1);
    lock = 1;
}

void BadUnlock() {
    lock = 0;
}

void AtomicLock() {
    xchg(&atomicLock, 1);
}

void AtomicUnlock() {
    xchg(&atomicLock, 0);
}

void PthreadLock() {
    pthread_mutex_lock(&monMutex);
}

void PthreadUnlock() {
    pthread_mutex_unlock(&monMutex);
}

void *CodeThread(void * a)
{
    long i, tmp;
    for(i = 0; i < N_ITER; i++)
    {
        //PthreadLock();
        //AtomicLock();
        BadLock(); 
        count++;
        BadUnlock();
        //AtomicUnlock();
        //PthreadUnlock();
    }
}


// -------- la fonction main ----------

int main(int argc, char **argv)
{
    pthread_t T1, T2;
    int iTrial;
    int correct = 0;
    clock_t time_start = clock();
    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (iTrial = 0; iTrial < 10000; iTrial++) {
        count = 0;
        pthread_create(&T1, 0, CodeThread, 0);
        pthread_create(&T2, 0, CodeThread, 0);
        pthread_join(T1, NULL);
        pthread_join(T2, NULL);

        printf("La valeur finale est %ld/%d \n",count, 2*N_ITER);

        if (count == 2*N_ITER) correct++;
        printf("%d/%d de correct!\n",correct,iTrial+1);

        // Déterminer le temps moyen par itération.
        clock_gettime(CLOCK_MONOTONIC, &finish);
        double elasped = finish.tv_sec - start.tv_sec;
        elasped += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

        printf("Temps moyen par itération: %f.\n", elasped / iTrial);
    }
}
