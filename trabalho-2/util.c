#include <stdio.h>
#include <stdlib.h>
#include "util.h"

void raise_error_message(char *msg) {
    printf("%s\n", msg);
    exit(-1);
}

void *_malloc(size_t size) {
    void *ptr = malloc(size);
    if(ptr == NULL) {
	raise_error_message("Dynamic memory allocation error.\n");
    }

    return ptr;
}
