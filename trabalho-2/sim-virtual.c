#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sim-virtual.h"
#include "util.h"

#define TRUE 1
#define FALSE 0

static void validate_parameters(char *algorithm, char *filename, int page_size, int mem_size);

int main(int argc, char **argv)
{
    char *algorithm;
    char *filename;
    int page_size;
    int mem_size;

    if(argc != 5) {
	char *msg = "Invalid parameters. Please specify the algorithm(1), the input file(2), the page size(3) and the memory total size(4).\n";
	raise_error_message(msg);
    }

    algorithm = argv[1];
    filename = argv[2];
    page_size = (int) strtol(argv[3], NULL, 10);
    mem_size = (int) strtol(argv[4], NULL, 10);

    init_simulation(algorithm, filename, page_size, mem_size);

    return 0;
}

void init_simulation(char *algorithm, char *filename, int page_size, int mem_size) {
    FILE *file;

    validate_parameters(algorithm, filename, page_size, mem_size);

    file = fopen(filename, "r");
    if(file == NULL) {
	raise_error_message("Error to open file.\n");
    }

    // teste
    printf("algorithm: %s\nfilename: %s\npage_size: %d\nmem_size: %d\n", algorithm, filename, page_size, mem_size);

    fclose(file);
}

static void validate_parameters(char *algorithm, char *filename, int page_size, int mem_size) {
    if(strcasecmp(algorithm, "LRU") && strcasecmp(algorithm, "NRU")) {
	raise_error_message("Invalid algorithm. Please, choose between LRU and NRU algorithms.\n");
    }
    if(access(filename, R_OK)) {
	raise_error_message("Invalid file. Check file path or read permission.\n");
    }
    if(page_size < 8 || page_size > 32) {
	raise_error_message("Invalid page size. Please use a size between 8 and 32 KB.\n");
    }
    if(mem_size < 1 || mem_size > 16) {
	raise_error_message("Invalid memory size. Please use a size between 1 and 16 MB.\n");
    }
}

