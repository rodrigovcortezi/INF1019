#include <stdio.h>
#include <pthread.h>
#include <sys/sem.h>
#include <signal.h>
#include "interpreter.h"
#include "scheduler.h"

// Escalonador - variável global
Scheduler *scheduler;

void *interpreter_routine(void *arg) {
    init_interpreter((Scheduler *) arg);
    pthread_exit(NULL);
}

void *scheduler_routine(void *arg) {
    exec((Scheduler *) arg);
    pthread_exit(NULL);
}

int main()
{
    pthread_t interpreter_thread;
    pthread_t scheduler_thread;

    scheduler = create_scheduler();

    pthread_create(&interpreter_thread, NULL, interpreter_routine, scheduler);
    pthread_create(&scheduler_thread, NULL, scheduler_routine, scheduler);
    pthread_join(interpreter_thread, NULL);
    pthread_kill(scheduler_thread, SIGUSR1);
    pthread_join(scheduler_thread, NULL);

    return 0;
}

