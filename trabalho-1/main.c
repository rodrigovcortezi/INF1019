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

    /* Cria uma thread para iniciar o interpretador, que irá ler os comandos no arquivo de
     * entrada e admiti-los ao escalonador. */
    pthread_create(&interpreter_thread, NULL, interpreter_routine, NULL);

    /* Cria uma thread para iniciar o escalonador de processos, que irá começar o escalonamento
     * dos processos admitidos pelo interpretador. */
    pthread_create(&scheduler_thread, NULL, scheduler_routine, NULL);

    pthread_join(interpreter_thread, NULL);

    /* Evia o sinal SIGUSR1 para a thread de execução do escalonador. Isso irá avisar ao escalonador que
     * todos os processos foram admitidos. Sabemos que isso é verdade pois nesse momento a chamada 
     * pthread_join da thread do interpretador foi retornada. */
    pthread_kill(scheduler_thread, SIGUSR1);

    pthread_join(scheduler_thread, NULL);

    return 0;
}

