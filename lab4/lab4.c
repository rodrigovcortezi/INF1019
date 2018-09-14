#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

void exercicio1();
void exercicio2();
void exercicio3();

int main(int argc, char *argv[]) {
    exercicio3();

    return 0;
}

//
// Exercício 1
//

void exercicio1() {
    char buffer[100];
    int fd[2];
    int pid, status;

    if(pipe(fd) < 0) {
	printf("Error when opening pipes.\n");
	exit(-1);
    }

    pid = fork();
    if(pid > 0) {
	// Parent process	
	close(fd[1]);
	wait(&status);
	read(fd[0], buffer, sizeof(buffer));
	printf("Processo pai leu: %s\n", buffer);
	close(fd[0]);
    } else if(pid == 0) {
	// Child process
	close(fd[0]);
	strcpy(buffer, "Hello world!\n");
	write(fd[1], buffer, sizeof(buffer));
	printf("Processo filho escreveu: %s\n");
	close(fd[1]);
	exit(0);
    } else {
	// Fork error
	printf("Error when forking.\n");
	exit(-1);
    }
}

//
// Exercício 2
//

void exercicio2() {
    const int stdin_fd = 0;
    const int stdout_fd = 1;
    int fd_input, fd_output;
    char buffer;
    int bytes_read;

    fd_input = open("entrada.txt", O_RDONLY);
    if(fd_input == -1) {
	printf("Error when opening input file: entrada.txt.\n");
	exit(-1);
    }

    fd_output = open("saida.txt", O_WRONLY | O_TRUNC);
    if(fd_output == -1) {
	printf("Error when opening output file: saida.txt.\n");
    }

    if(dup2(fd_input, 0) == -1) {
	printf("Error when duping input file descriptor.\n");
	exit(-1);
    }
    if(dup2(fd_output, 1) == -1) {
	printf("Error when duping output file descriptor.\n");
	exit(-1);
    }

    do {
	bytes_read = read(stdin_fd, &buffer, 1);
	if(write(stdout_fd, &buffer, 1) != 1) {
	    printf("Error when writing data to output file.\n");
	    exit(-1);
	}
    } while(bytes_read > 0);

    close(fd_input);
    close(fd_output);
}

//
// Exercício 3
//

void exercicio3() {
    char *argv[1] = {"ls"};
    char *argv2[2] = {"echo"};
    const int stdout_fd = 1;
    int fd[2];
    char buffer;
    int i, k;
    char *filename, filenames[500];
    int bytes_read;
    int pid, status;
    if(pipe(fd) < 0) {
	printf("Error when opening pipes.\n");
	exit(-1);
    }

    for(i = 0; i < 2; i++) {
	pid = fork();
	if(pid == 0) {
	    // Child process
	    if(i == 0) {
		close(fd[0]);
		dup2(fd[1], stdout_fd);
		execve("/usr/bin/ls", argv, NULL);
	    } else {
		close(fd[1]);
		k = 0;
		do {
		    bytes_read = read(fd[0], &filenames[k], 1);
		    k++;
		} while(bytes_read > 0);
		argv2[1] = filenames;
		printf("%s\n", filenames);
		execve("/usr/bin/echo", argv2, NULL);
	    }
	    exit(0);
	} else if(pid < 0){
	    // Fork error
	    printf("Error when forking.\n");
	    exit(-1);
	}
    }
}

