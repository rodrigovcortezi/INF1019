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
	printf("error when opening pipes.\n");
	exit(-1);
    }

    pid = fork();
    if(pid > 0) {
	// Parent process	
	close(fd[1]);
	wait(&status);
	read(fd[0], buffer, sizeof(buffer));
	printf("%s\n", buffer);
	close(fd[0]);
    } else if(pid == 0) {
	// Child process
	close(fd[0]);
	strcpy(buffer, "Hello world!\n");
	write(fd[1], buffer, sizeof(buffer));
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
    int i;
    int pid;
    int fd[2];
    char *argv[] = {"ls", NULL};
    char *argv2[3];
    char buffer;
    char filenames[100];
    int size;


    if(pipe(fd) < 0) {
	printf("error when opening pipes.\n");
	exit(-1);
    }

    for(i = 0; i < 2; i++) {
	pid = fork();
	if(pid == 0) {
	    if(i == 0) {
		close(fd[0]);
		dup2(fd[1], 1);
		execve("/bin/ls", argv, NULL);
	    } else {
		close(fd[1]);
		size = 0;
		while(read(fd[0], &buffer, 1) == 1) {
		    filenames[size] = buffer;
		    size++;
		}
		filenames[size] = '\0';
		argv2[0] = "echo";
		argv2[1] = filenames;
		argv2[2] = NULL;
		execve("/bin/echo", argv2, NULL);
	    }
	} else if(pid < 0) {
	    printf("Fork error.\n");
	    exit(-1);
	}
    }

}

