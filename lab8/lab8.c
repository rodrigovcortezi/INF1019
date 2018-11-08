#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#define MAX 8

static void exercicio1();
static void producer(int *sockfd, int *response_sockfd);
static void consumer(int sockfd, int response_sockfd);

int main()
{
    exercicio1();

    return 0;
}

static void exercicio1() {
    const int consumer_count = 2;
    pid_t pid;
    int i;
    int sockfd[consumer_count];
    int response_sockfd[consumer_count];

    for(i = 0; i < consumer_count; i++) {
	sockfd[i] = socket(AF_LOCAL, SOCK_STREAM, 0);
	if(sockfd[i] == -1) {
	    printf("Socket initialization error. \n");
	    exit(-1);
	}
	response_sockfd[i] = socket(AF_LOCAL, SOCK_STREAM, 0);
	if(response_sockfd[i] == -1) {
	    printf("Socket initialization error. \n");
	    exit(-1);
	}
    }

    for(i = 0; i < consumer_count; i++) {
	pid = fork();	
	if(pid == 0) {
	    // Child process
	    consumer(sockfd[i], response_sockfd[i]);
	    exit(0);
	} else if(pid < 0) {
	    printf("Fork error. \n");
	    exit(-1);
	}
    }

    producer(sockfd, response_sockfd);
    
    for(i = 0; i < consumer_count; i++) {
	close(sockfd[i]);
	close(response_sockfd[i]);
    }
}

static void producer(int *sockfd, int *response_sockfd) {
    const int produce_count = 64;
    int send_count[2];
    int response;
    int i;
    int available = 0;
    for(i = 0; i < produce_count; i++) {
	send(sockfd[available], &i, sizeof(int), 0);
	recv(response_sockfd[available], &response, sizeof(int), 0);
	send_count[available] += 1;
	if(!response) {
	    printf("No response. \n");
	    exit(-1);
	}
	sleep(1);
	if(send_count[available] == MAX) {
	    available ^= 0x01;
	}
    }
}

static void consumer(int sockfd, int response_sockfd) {
    const int consume_count = 32;
    int buf;
    int i;
    int response = 1;
    for(i = 0; i < consume_count; i ++) {
	send(response_sockfd, &response, sizeof(int), 0);
	recv(sockfd, &buf, sizeof(int), 0);
    }
}

