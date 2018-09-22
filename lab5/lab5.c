#include <stdio.h>
#include <unistd.h>
#include <pthread.h>



static void *contCrescente();
static void *contDecrescente();
static void *incrementa1();
static void *incrementa5();
void exercicio1();
void exercicio2();
void exercicio3();

int global = 0;

int main()
{
    // exercicio1();
    /*exercicio2();*/
    exercicio3();

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

void exercicio4() {
    
}

