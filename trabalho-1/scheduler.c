#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "scheduler.h"
#include "queue.h"

#define OUTPUT_FILE "saida.txt"

typedef enum state {

    Ready,

    Running,

    Waiting,

    Finished 

} State;

typedef struct scheduler {

    Queue *processes[7];

    int process_count;

} Scheduler;

typedef struct process {

    pid_t pid;

    int priority;

    State state;

} Process;

typedef void (*pFunc) (void *);

static Process *create_process(pid_t pid, int priority);

static void clean_scheduler();

static void free_process(Process *process);

void exec() {
    while(1) {
	printf(".\n");
	sleep(1);
    }
}

Scheduler *create_scheduler() {
    Scheduler *new_scheduler = (Scheduler *) malloc(sizeof(Scheduler));
    if(new_scheduler == NULL) {
	printf("Dynamic memory allocation error. \n");
	exit(-1);
    }
    
    clean_scheduler(new_scheduler);

    return new_scheduler;
}

void add_program(Scheduler *scheduler, char *program_name, int priority) {
    Process *process;
    pid_t pid;

    pid = fork();
    if(pid == 0) {
	// Child process
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

    process = create_process(pid, priority);
    if(scheduler->processes[priority-1] == NULL) {
	scheduler->processes[priority-1] = create_queue((pFunc) free_process);
    }

    insert_element(scheduler->processes[priority-1], process);

    scheduler->process_count += 1;
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
}

static void free_process(Process *process) {
    free(process);
}

