#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "interpreter.h"

#define INPUT_FILE "exec.txt"

struct command {

    char program_name[50];

    int priority;

};

typedef struct command Command;

static Command *read_next_command(FILE *file);
static Command *create_command(char *program_name, int priority);

void init_interpreter() {
    Command *command;

    FILE *input_file = fopen(INPUT_FILE, "r");
    if(input_file == NULL) {
	printf("Error during input file opening. \n");
	exit(-1);
    }

    command = read_next_command(input_file);
    while(command != NULL) {
	add_program(command->program_name, command->priority);
	sleep(1);
	command = read_next_command(input_file);
    }
}

static Command *read_next_command(FILE *file) {
    char *buffer = NULL;
    size_t n;
    int read;
    char *token;
    char program_name[50];
    int priority;
    Command *command;

    read = getline(&buffer, &n, file);
    if(read == -1) {
	// End of file
	return NULL;
    }

    token = strtok(buffer, " ");
    if(token == NULL || strcmp(token, "exec") != 0) {
	printf("Wrong input file format: 'exec' not found. \n");
	exit(-1);
    }

    token = strtok(NULL, " ");
    if(token == NULL) {
	printf("Wrong input file format: program name not found. \n");
	exit(-1);
    }
    strcpy(program_name, token);

    token = strtok(NULL, "=");
    if(token == NULL || strcmp(token, "prioridade")) {
	printf("Wrong input file format: 'prioridade' not found. \n");
	exit(-1);
    }

    token = strtok(NULL, "\0");
    if(token == NULL) {
	printf("Wrong input file format: priority value not found. \n");
	exit(-1);
    }
    priority = atoi(token);
    if(priority == 0) {
	printf("Wrong input file format: priority value not valid. \n");
	exit(-1);
    }

    command = create_command(program_name, priority);

    return command;
}

static Command *create_command(char *program_name, int priority) {
    Command *new_command = (Command *) malloc(sizeof(Command));
    if(new_command == NULL) {
	printf("Dynamic memory allocation error. \n");
	exit(-1);
    }

    strcpy(new_command->program_name, program_name);
    new_command->priority = priority;

    return new_command;
}

