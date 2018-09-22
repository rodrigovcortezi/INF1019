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
    exercicio2();

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

void exercicio3() {

}

