#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define EVER ;;

void exercicio1(int argc, char *argv[]);
void exercicio2(int argc, char *argv[]);
void exercicio3();

void childhandler(int signo);
void starthandler(int signo);
void endhandler(int signo);

int main(int argc, char *argv[]) {
	// exercicio1(argc, argv);
	/*exercicio2(argc, argv);*/
	exercicio3();
	
	return 0;
}

//
// Exercício 1
//

void exercicio1(int argc, char *argv[]) {
	int delay;
	pid_t pid;

	signal(SIGCHLD, childhandler);

	if ((pid = fork()) < 0) {
		fprintf(stderr, "Erro ao criar filho\n");
		exit(-1);
	}
	if (pid == 0) /* child */
		for(EVER); /* ou sleep(3);*/
	else { /* parent */
		sscanf(argv[1], "%d", &delay); /* read delay from command line */
		sleep(delay);
		printf("Program %s exceeded limit of %d seconds!\n", argv[2], delay);
		kill(pid, SIGKILL);
	}
}

//
// Exercício 2
//

void exercicio2(int argc, char *argv[]) {
	int delay;
	pid_t pid;

	signal(SIGCHLD, childhandler);

	if ((pid = fork()) < 0) {
		fprintf(stderr, "Erro ao criar filho\n");
		exit(-1);
	}
	if (pid == 0) { /* child */
		execv("sleep15", NULL);
	}
	else { /* parent */
		sscanf(argv[1], "%d", &delay); /* read delay from command line */
		sleep(delay);
		printf("Program %s exceeded limit of %d seconds!\n", argv[2], delay);
		kill(pid, SIGKILL);
	}
}

void childhandler(int signo) { /* Executed if child dies before parent */
	int status;
	pid_t pid = wait(&status);
	printf("Child %d terminated com estado %d.\n", pid, status);
	exit(0);
}

//
// Exercício 3
//

int time_count = 0;

void exercicio3() {
	signal(SIGUSR1, starthandler);
	signal(SIGUSR2, endhandler);
	for(EVER);
}

void starthandler(int signo) {
    for(EVER) {
	sleep(1);
	time_count++;
    }
}

void endhandler(int signo) {
    int cost;
    if(time_count < 60) {
	cost = 2 * time_count;
    } else {
	cost = 120 + (time_count - 60);
    }

    printf("custo da ligação (em centavos): %d\n", cost);

    exit(0);
}

