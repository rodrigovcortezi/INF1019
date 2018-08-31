#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void exercicio1();
void exercicio2();
void exercicio3();
int get_biggest(int *array, int n);

int main(int argc, char *argv[]) {
	//exercicio1();
	/* exercicio2(); */
	exercicio3();

	return 0;
}

//
// Exercício 1
//

void exercicio1() {
    int shm_id;
    char message[100];

    // Faz a leitura do teclado
    scanf("%[^\n]", message);

    shm_id = shmget(IPC_PRIVATE, sizeof(message), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if(shm_id == -1) {
	printf("Shared memory allocation error.\n");
	exit(-1);
    }
    char *page = (char*) shmat(shm_id, NULL, 0);
    if(*page == -1) {
	printf("Shared memory attachement error.\n");
	exit(-1);
    }
    memcpy(page, message, sizeof(message));
    printf("shm_id: %d\n", shm_id);
}

//
// Exercício 2
//

void exercicio2() {
    int i;
    int shm_id;
    int pid, status;
    int vetor[128];
    int *next_partition, *process_found;
    int result;
    const int partition_count = 8;
    const int partition_size = 128 / partition_count;
    
    // Inicializa o vetor com valores aleatórios
    srand(time(NULL));
    for(i = 0; i < 128; i++) {
	vetor[i] = 1 + rand() % 128;
    }

    // Aloca a memória compartilhada
    shm_id = shmget(IPC_PRIVATE, partition_count * sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if(shm_id == -1) {
	printf("Shared memory allocation error.\n");
	exit(-1);
    }

    process_found = (int*) shmat(shm_id, NULL, 0);
    if(*process_found == -1) {
	printf("Shared memory attachment error.\n");
	exit(-1);
    }

    for(i = 0; i < partition_count; i++) {
	pid = fork();
	if(pid == 0) {
	    // Child process
	    next_partition = vetor + (i * partition_size);
	    process_found[i] = (i * partition_size) + get_biggest(next_partition, partition_size);
	    exit(0);
	} else if(pid < 0) {
	    // Fork error
	    printf("Fork error.\n");
	    exit(-1);
	}
    }

    // Processo pai aguarda os processos filhos terminarem
    while(wait(&status) > 0);

    result = process_found[0];
    for(i = 1; i < partition_count; i++) {
	if(vetor[process_found[i]] > vetor[result]) {
	    result = process_found[i];
	}
    }

    if(shmdt(process_found) == -1) {
	printf("Shared memory detachment erro.\n");
	exit(-1);
    }
    shmctl(shm_id, IPC_RMID, NULL);

    for(i = 0; i < 128; i++) {
	printf("%d - ", vetor[i]);
    }
    printf("\n");

    printf("result: %d\n", vetor[result]);
}

int get_biggest(int *array, int n) {
    int i;
    int biggest = 0;
    for(i = 1; i < n; i++) {
	if(array[i] > array[biggest]) {
	    biggest = i;
	}
    }

    return biggest;
}

//
// Exercício 3
//

void exercicio3() {
    const int dimension = 4;
    int status;
    int shm_id, pid;
    int v_shm_id[dimension];
    int i, j;
    int matriz[dimension][dimension];
    int *linha;
    int **transposta;
    for(i = 0; i < dimension; i++) {
	for(j = 0; j < dimension; j++) {
	    scanf("%d", &matriz[i][j]);
	}
    }

    printf("\nmatriz original: \n");
    for(i = 0; i < dimension; i++) {
	for(j = 0; j < dimension; j++) {
	    printf("%d ", matriz[i][j]);
	}
	printf("\n");
    }

    shm_id = shmget(IPC_PRIVATE, dimension * sizeof(int*), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if(shm_id == -1) {
	printf("Shared memory allocation error.\n");
	exit(-1);
    }

    transposta = (int**) shmat(shm_id, NULL, 0);
    for(i = 0; i < dimension; i++) {
	v_shm_id[i] = shmget(IPC_PRIVATE, dimension * sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	if(v_shm_id[i] == -1) {
	    printf("Shared memory allocation error.\n");
	    exit(-1);
	}
	transposta[i] = (int *) shmat(v_shm_id[i], NULL, 0);
    }
    for(i = 0; i < dimension; i++) {
	pid = fork();
	if(pid == 0) {
	    // Child process
	    linha = matriz[i];
	    for(j = 0; j < dimension; j++) {
		transposta[j][i] = linha[j];
	    }
	    exit(0);
	} else if(pid < 0) {
	    // Fork error
	    printf("Fork error.\n");
	    exit(-1);
	}
    }

    while(wait(&status) > 0);
    
    printf("\nmatriz transposta: \n");
    for(i = 0; i < dimension; i++) {
	for(j = 0; j < dimension; j++) {
	    printf("%d ", transposta[i][j]);
	}
	printf("\n");
    }

    for(i = 0; i < dimension; i++) {
	if(shmdt(transposta[i]) == -1) {
	    printf("Shared memory detatchment error.\n");
	    exit(-1);
	}
	shmctl(v_shm_id[i], IPC_RMID, NULL);
    }

    if(shmdt(transposta) == -1) {
	printf("Shared memory detachment error.\n");
	exit(-1);
    }
    shmctl(shm_id, IPC_RMID, NULL);
}

