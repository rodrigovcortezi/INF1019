#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define N 5
#define X 100
#define Y 10
#define D 300

static void *contCrescente();
static void *contDecrescente();
static void *incrementa1();
static void *incrementa5();
void exercicio1();
void exercicio2();
void exercicio3();
void exercicio4();


int main()
{
    /*exercicio1();*/
    /*exercicio2();*/
    /*exercicio3();*/
    exercicio4();

    return 0;
}

static void *contCrescente() {
    int i;
    for(i = 1; i <= 20; i++) {
	printf("%d \n", i);
	sleep(2);
    }
    pthread_exit(NULL);
}

static void *contDecrescente() {
    int i;
    for(i = 30; i >= 1; i--) {
	printf("%d \n", i);
	sleep(1);
    }
    pthread_exit(NULL);
}

void exercicio1() {
    pthread_t thread1;
    pthread_t thread2;

    pthread_create(&thread1, NULL, contCrescente, NULL);
    pthread_create(&thread2, NULL, contDecrescente, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
}

int global = 0;

static void *incrementa1() {
    global += 1;
    printf("thread1, global: %d\n", global);
    pthread_exit(NULL);
}

static void *incrementa5() {
    global += 5;
    printf("thread2, global: %d\n", global);
    pthread_exit(NULL);
}

void exercicio2() {
    pthread_t thread1;
    pthread_t thread2;

    pthread_create(&thread1, NULL, incrementa1, NULL);
    pthread_create(&thread2, NULL, incrementa5, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
}

int buffer[8];
int length;
int head;

static void bufferInsert(int number) {
    int i = (head + length) % 8;
    buffer[i] = number;
    length++;
}

static int bufferRemove() {
    int first = buffer[head];
    head = (head + 1) % 8;

    return first;
}

static void *producer() {
    int i = 0;
    while(i < 64) {
	bufferInsert(i + 1);
	bufferInsert(i + 2);
	bufferInsert(i + 3);
	bufferInsert(i + 4);
	i += 4;
	sleep(1);
    }

    pthread_exit(NULL);
}

static void *consumer() {
    int i = 0;
    int k = 0;
    int number;

    sleep(2); // Para dar tempo de encher o buffer

    printf("Consumi: ");
    while(i < 64) {
	for(k = 0; k < 8; k++) {
	    number = bufferRemove();	    
	    printf("%d ", number);
	}
	i += 8;
	sleep(2);
    }
    printf("\n");

    pthread_exit(NULL);
}

void exercicio3() {
    pthread_t producerThread;
    pthread_t consumerThread;

    length = 0;
    head = 0;

    pthread_create(&producerThread, NULL, producer, NULL);
    pthread_create(&consumerThread, NULL, consumer, NULL);
    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);
}

static void *frog(void *id) {
    int currentDistance = 0;
    int jump;
    srand((unsigned int) time(NULL));
    while(currentDistance < 300) {
	jump = 1 + rand() % X;
	printf("sapo %d pulou %d\n", *((int *)id), jump);
	currentDistance += jump;
	sleep(1 + rand() % Y);
    }

    printf("Sapo %d chegou ao fim da corrida.\n", *((int *)id));

    pthread_exit(NULL);
}

void exercicio4() {
    int i;
    int ids[] = {1, 2, 3, 4, 5};
    pthread_t threads[N];

    for(i = 0; i < N; i++) {
	pthread_create(&threads[i], NULL, frog, &ids[i]);
    }

    for(i = 0; i < N; i++) {
	pthread_join(threads[i], NULL);
    }
}

