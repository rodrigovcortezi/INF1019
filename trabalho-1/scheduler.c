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

    Ready,

    Running,

    Waiting,

    Finished 

} State;

typedef struct process {

    pid_t pid;

    int priority;

    State state;

} Process;

typedef struct scheduler {

    Queue *processes[7];

    Process *running_process;

    sem_t *semaphore;

    int process_count;

} Scheduler;

Scheduler *scheduler;

typedef void (*pFunc) (void *);

static void process_finished(int signo);

static void interpreter_finished(int signal);

static Scheduler *create_scheduler();

static Process *create_process(pid_t pid, int priority);

static void clean_scheduler();

static void free_process(Process *process);

int all_admitted = FALSE;

void turn_on() {
    scheduler = create_scheduler();
    signal(SIGUSR1, interpreter_finished);
    signal(SIGCHLD, process_finished);
}

void exec() {
    Process *process;
    int i;

    while(!(scheduler->process_count == 0 && all_admitted)) {
	sem_wait(scheduler->semaphore);
	for(i = 0; i < 7; i++) {
	    if(scheduler->processes[i] != NULL) {
		process = remove_element(scheduler->processes[i]);
		if(process != NULL) {
		    scheduler->running_process = process;
		    kill(process->pid, SIGCONT);
		    scheduler->process_count -= 1;
		}
	    }
	}
    }
    sleep(6);
}

void add_program(char *program_name, int priority) {
    Process *process;
    pid_t pid;

    pid = fork();
    if(pid == 0) {
	// Child process
	printf("%d\n", getpid());
	raise(SIGSTOP);
	if(execv(program_name, NULL) == -1) {
	    printf("Can't exec program %s\n", program_name);
	    exit(-1);
	}
    } else if(pid < 0){
	// Fork error
	printf("Fork error. \n");
	exit(-1);
    }

    waitpid(pid, NULL, WUNTRACED);

    process = create_process(pid, priority);
    if(scheduler->processes[priority-1] == NULL) {
	scheduler->processes[priority-1] = create_queue((pFunc) free_process);
    }

    insert_element(scheduler->processes[priority-1], process);
    scheduler->process_count += 1;
    sem_post(scheduler->semaphore);
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

static Process *create_process(pid_t pid, int priority) {
    Process *new_process = (Process *) malloc(sizeof(Process));
    if(new_process == NULL) {
	printf("Dynamic memory allocation error. \n");
	exit(-1);
    }

    new_process->pid = pid;
    new_process->priority = priority;
    new_process->state = Ready;

    return new_process;
}

static void clean_scheduler(Scheduler *scheduler) {
    int i;

    for(i = 0; i < 7; i++) {
	scheduler->processes[i] = NULL;
    }
    scheduler->process_count = 0;
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
    }
}

static void interpreter_finished(int signal) {
    all_admitted = TRUE;
}
