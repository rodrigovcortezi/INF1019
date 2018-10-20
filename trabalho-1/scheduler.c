#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "scheduler.h"
#include "queue.h"

#define TRUE 1
#define FALSE 0
#define OUTPUT_FILE "saida.txt"
#define QUANTUM 1

typedef enum state {

    /* Processo acabou de ser admitido no escalonador. */
    New,

    /* Processo está pronto para ser executado. */
    Ready,

    /* Processo está sendo executado. */
    Running,

    /* Processo está bloqueado por conta de I/O */
    Waiting,

    /* Processo terminou. */
    Finished 

} State;

typedef struct process {

    /* Identificação do processo. */
    pid_t pid;

    /* Nome do programa a ser executado. */
    char *program_name;

    /* Prioridade do processo: 0 -> 6 */
    int priority;

    /* Estado do processo. */
    State state;

} Process;

typedef struct scheduler {

    /* Filas de processos prontos para cada prioridade. */
    Queue *processes[7];

    /* Processo sendo executado. */
    Process *running_process;

    /* Semáforo usado para sicronizar a admissão e execução de processos no escalonador. */
    sem_t *semaphore;

    /* Contador de processos admitidos no escalonador. */
    int admitted_count;

    /* Contador de processos terminados no escalonador. */
    int finished_count;

} Scheduler;

// Variárel global armazenando o escalonador. Visível às duas as threads.
Scheduler *scheduler;

// Variável global que indica se todos os programas contidos em exec.txt foram admitidos no escalonador.
int all_admitted = FALSE;

typedef void (*pFunc) (void *);

// test
static void debug();

static void exec_process(Process *process);

static void process_finished(int signal);

static void interpreter_finished(int signal);

static void alarm_handler(int signal);

static Scheduler *create_scheduler();

static Process *create_process(char *program_name, int priority);

static void clean_scheduler();

static void free_process(Process *process);

/*
 * Inicializa o escalonador: aloca a estrutura de dados que o representa e
 * configura o tratamento de sinais.
 */
void init_scheduler() {
    scheduler = create_scheduler();
    signal(SIGUSR1, interpreter_finished);
    signal(SIGCHLD, process_finished);
    signal(SIGALRM, alarm_handler);
}

/*
 * Começa o escalonador, ou seja, inicia o escalonamento de processos.
 */
void start_scheduler() {
    raise(SIGALRM);
    while(TRUE);
}

/*
 * Admite um novo programa no escalonador.
 */
void add_program(char *program_name, int priority) {
    Process *process;

    priority -= 1;

    process = create_process(program_name, priority);
    insert_element(scheduler->processes[priority], process);
    scheduler->admitted_count += 1;
    sem_post(scheduler->semaphore);
}

/*
 * Executa um processo. Se o processo acabou de ser admitido no
 * escalonador e nunca foi executado antes, um novo processo é
 * criado e substituido pelo programa a ser executado. Se o processo
 * já foi executado antes e está em estado pronto, um sinal sigcont
 * é enviado.
 */
static void exec_process(Process *process) {
    pid_t pid;
    if(process->state == New) {
	pid = fork();
	if(pid > 0) {
	    // Parent process
	    process->pid = pid;
	}
	else if(pid == 0) {
	    // Child process
	    printf("%d\n", getpid());
	    if(execv(process->program_name, NULL) == -1) {
		printf("Can't exec program %s\n", process->program_name);
		exit(-1);
	    }
	} else {
	    // Fork error
	    printf("Fork error. \n");
	    exit(-1);
	}
    } else if(process->state == Ready) {
	kill(process->pid, SIGCONT);
    } else {
	printf("Can't execute process. Invalid state.\n");
	exit(-1);
    }
    process->state = Running;
    scheduler->running_process = process;
}

/*
 * Aloca a estrutura de dados que representa o escalonador.
 */
static Scheduler *create_scheduler() {
    Scheduler *new_scheduler = (Scheduler *) malloc(sizeof(Scheduler));
    if(new_scheduler == NULL) {
	printf("Dynamic memory allocation error. \n");
	exit(-1);
    }

    clean_scheduler(new_scheduler);
    new_scheduler->semaphore = sem_open("/scheduler_semaphore", O_CREAT, 0644, 0);
    sem_init(new_scheduler->semaphore, 0, 0);

    return new_scheduler;
}

/*
 * Aloca a estrutura de dados que representa um processo.
 */
static Process *create_process(char *program_name, int priority) {
    Process *new_process = (Process *) malloc(sizeof(Process));
    if(new_process == NULL) {
	printf("Dynamic memory allocation error. \n");
	exit(-1);
    }

    new_process->pid = -1;
    new_process->program_name = program_name;
    new_process->priority = priority;
    new_process->state = New;

    return new_process;
}

/*
 * Inicializa os atributos do escalonador.
 */
static void clean_scheduler(Scheduler *scheduler) {
    int i;

    for(i = 0; i < 7; i++) {
	scheduler->processes[i] = create_queue((pFunc) free_process);
    }
    scheduler->admitted_count = 0;
    scheduler->finished_count = 0;
    scheduler->running_process = NULL;
}

/*
 * Destrói um processo.
 */
static void free_process(Process *process) {
    free(process);
}

/*
 * Tratamento do sinal SIGCHLD.
 */
static void process_finished(int signal) {
    int status;
    Process *running_process = scheduler->running_process;

    if(running_process == NULL) {
	return;
    }

    // flag WNOHANG: chamada waitpid é retornada imediatamente, sem espera.
    waitpid(running_process->pid, &status, WNOHANG);
    // macro WIFEXITED: verifica se o processo terminou voluntariamente ou foi interrompido pelo sinal SIGSTOP.
    if(WIFEXITED(status)) {
	printf("Child %d terminated\n", running_process->pid);

	free_process(running_process);
	scheduler->finished_count += 1;
	scheduler->running_process = NULL;
	raise(SIGALRM);
    }
}

/*
 * Tratamento do sinal SIGUSR1.
 */
static void interpreter_finished(int signal) {
    all_admitted = TRUE;
}

/*
 * Tratamento do sinal SIGALRM.
 */
static void alarm_handler(int signal) {
    Process *running_process;
    Process *next;
    int priority;
    int current_priority;

    // Condição de parada do escalonador.
    if(scheduler->admitted_count == scheduler->finished_count && all_admitted) {
	exit(0);
    }

    printf("alarm..\n");

    running_process = scheduler->running_process;
    if(running_process != NULL) {
	current_priority = running_process->priority;

	// Parar o processo corrente
	scheduler->running_process = NULL;
	kill(running_process->pid, SIGSTOP);

	// Realoca processo na fila adequada
	if(current_priority < 6) {
	    running_process->priority += 1;
	    insert_element(scheduler->processes[current_priority+1], running_process);
	} else {
	    insert_element(scheduler->processes[current_priority], running_process);
	}
	running_process->state = Ready;
	sem_post(scheduler->semaphore);
    }

    sem_wait(scheduler->semaphore);

    priority = 0;
    next = remove_element(scheduler->processes[priority]);
    while(next == NULL && priority < 6) {
	priority += 1;
	next = remove_element(scheduler->processes[priority]);
    }

    if(next != NULL) {
	printf("executa o processo %d por %d segundos..\n", next->pid, QUANTUM * (7 - priority));
	exec_process(next);
	alarm(QUANTUM * (7 - priority));
    }
}

static void debug() {
    Process *p;
    Queue *new;
    int i;

    for(i = 0; i < 7; i++) {
	printf("fila %d:", i);
	new = create_queue((pFunc) free_process);

	p = remove_element(scheduler->processes[i]);
	while(p != NULL) {
	    printf("%d - ", p->pid);
	    insert_element(new, p);
	    p = remove_element(scheduler->processes[i]);
	}
	printf("\n");

	scheduler->processes[i] = new;
    }

}
