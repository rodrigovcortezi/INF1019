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
#define QUANTUM 2

typedef enum state {

    New,

    Ready,

    Running,

    Waiting,

    Finished 

} State;

typedef struct process {

    pid_t pid;

    char *program_name;

    int priority;

    State state;

} Process;

typedef struct scheduler {

    Queue *processes[7];

    Process *running_process;

    sem_t *semaphore;

    int admitted_count;

    int finished_count;

} Scheduler;

Scheduler *scheduler;

typedef void (*pFunc) (void *);

static void exec_process(Process *process);

static void process_finished(int signo);

static void interpreter_finished(int signal);

static Scheduler *create_scheduler();

static Process *create_process(char *program_name, int priority);

static void clean_scheduler();

static void free_process(Process *process);

int all_admitted = FALSE;

void init_scheduler() {
    scheduler = create_scheduler();
    signal(SIGUSR1, interpreter_finished);
    signal(SIGCHLD, process_finished);
}

void start_scheduler() {
    Process *process;
    int i;

    while(!(scheduler->finished_count == scheduler->admitted_count && all_admitted)) {
	sem_wait(scheduler->semaphore);
	for(i = 0; i < 7; i++) {
	    process = remove_element(scheduler->processes[i]);
	    if(process != NULL) {
		exec_process(process);
	    }
	}
    }
    sleep(6);
}

void add_program(char *program_name, int priority) {
    Process *process;

    process = create_process(program_name, priority);
    insert_element(scheduler->processes[priority-1], process);
    scheduler->admitted_count += 1;
    sem_post(scheduler->semaphore);
}

static void exec_process(Process *process) {
    pid_t pid;
    if(process->state == New) {
	pid = fork();
	if(pid > 0) {
	    // Parent process
	    process->pid = pid;
	    process->state = Ready;
	} else if(pid == 0) {
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
    } else if(process->state == Ready){
	kill(process->pid, SIGCONT);
    } else {
	printf("Can't execute process. Invalid state.\n");
	exit(-1);
    }
    scheduler->running_process = process;
}

static Scheduler *create_scheduler() {
    Scheduler *new_scheduler = (Scheduler *) malloc(sizeof(Scheduler));
    if(new_scheduler == NULL) {
	printf("Dynamic memory allocation error. \n");
	exit(-1);
    }

    clean_scheduler(new_scheduler);
    new_scheduler->semaphore = sem_open("/scheduler_semaphore", O_CREAT, 0644, 0);

    return new_scheduler;
}

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

static void clean_scheduler(Scheduler *scheduler) {
    int i;

    for(i = 0; i < 7; i++) {
	scheduler->processes[i] = create_queue((pFunc) free_process);
    }
    scheduler->admitted_count = 0;
    scheduler->finished_count = 0;
    scheduler->running_process = NULL;
}

static void free_process(Process *process) {
    free(process);
}

static void process_finished(int signo) {
    int status;
    Process *running_process = scheduler->running_process;

    if(running_process == NULL) {
	return;
    }

    waitpid(running_process->pid, &status, WNOHANG);
    if(WIFEXITED(status)) {
	printf("Child %d terminated\n", running_process->pid);

	free_process(running_process);
	scheduler->finished_count += 1;
	scheduler->running_process = NULL;
    }
}

static void interpreter_finished(int signal) {
    all_admitted = TRUE;
}
