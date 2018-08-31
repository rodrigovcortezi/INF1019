#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int pid;
    printf("parent process id: %d\n", getpid());
    for(int i = 0; i < 2; i++) {
	pid = fork();
	if(pid > 0) {
	    // Parent process
	    printf("process %d created child process %d.\n", getpid(), pid);
	} else if(pid == 0) {
	    // Child process
	    /*exit(0);*/
	} else {
	    // Fork error
	    printf("fork error.\n");
	    exit(-1);
	}
    }

    printf("process id after for: %d\n", getpid());

    return 0;
}
