#include <stdio.h>
#include <stdlib.h>
#include "util.h"

void raise_error_message(char *msg) {
    printf("%s\n", msg);
    exit(-1);
}
