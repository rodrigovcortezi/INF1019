#include <stdio.h>
#include <pthread.h>
#include <sys/sem.h>
#include <signal.h>
#include "interpreter.h"
#include "scheduler.h"

void *interpreter_routine(void *arg) {
    init_interpreter();
    pthread_exit(NULL);
}

void *scheduler_routine(void *arg) {
    start_scheduler();
    pthread_exit(NULL);
}

int main()
{
    pthread_t interpreter_thread;
    pthread_t scheduler_thread;

    // Inicia o escalonador
    init_scheduler();

    pthread_create(&interpreter_thread, NULL, interpreter_routine, NULL);
    pthread_create(&scheduler_thread, NULL, scheduler_routine, NULL);
    pthread_join(interpreter_thread, NULL);
    pthread_kill(scheduler_thread, SIGUSR1);
    pthread_join(scheduler_thread, NULL);

    return 0;
}

