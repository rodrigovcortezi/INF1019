#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "sim-virtual.h"
#include "page.h"
#include "util.h"

typedef enum {

    /* Expressa a falta de conteúdo na memória. */
    GarbageContent,

    /* Expressa o conteúdo na memória. */
    Content

} MemoryContent;

struct node {
    /* índice da página. */
    int page;

    /* Próximo nó. */
    struct node *next;
};

typedef struct list {
    /* Primeiro nó. */
    struct node *first;

    /* Tamanho da lista. */
    int size;

} List;

struct simulator {

    /* Função que realiza o algoritimo de substituição de página. */
    int (*replacement_func)(Simulator*);

    /* Arquivo de entrada. */
    char *filename;

    /* Tamanho da página em KB. */
    int page_size;

    /* Número de páginas. */
    int page_count;

    /* Tamanho total de memória em MB. */
    int mem_size;

    /* Tabela de páginas. */
    Page **page_table;

    /* Lista de quadro de páginas. */
    List *page_frames;

    /* Memória física. */
    MemoryContent *memory;
};


static void validate_parameters(char *algorithm, char *filename, int page_size, int mem_size);

static int available_space(Simulator *sim);

static int LRU(Simulator *sim);

static int NRU(Simulator *sim);

static List *create_list();

static int get_list_size(List *list);

static void list_add(List *list, int page);

static void list_remove(List *list, int page);

static void list_replace(List* list, int page, int new_page);

static int list_random(List *list);

static void list_destroy(List *list);


Simulator *create_simulator(char *algorithm, char *filename, int page_size, int mem_size) {
    // Tamanho do endereço virtual em bits.
    const int addr_size = 32;
    int i, aux;
    int displacement_bits; 
    int page_count;

    Simulator *new = (Simulator *) _malloc(sizeof(Simulator));

    validate_parameters(algorithm, filename, page_size, mem_size);

    if(!strcasecmp(algorithm, "LRU")) {
	new->replacement_func = LRU;
    } else {
	new->replacement_func = NRU;
    }

    new->filename = filename;
    new->page_size = page_size;
    new->mem_size = mem_size;


    displacement_bits = 1;
    aux = page_size * 1000;
    while(aux >>= 1) {
	displacement_bits += 1;
    }

    new->page_count = 0x01 << (addr_size - displacement_bits);
    new->page_table = (Page **) _malloc(page_count * sizeof(Page *));
    for(i = 0; i < new->page_count; i++) {
	new->page_table[i] = create_page();
    }

    new->memory = (MemoryContent *) _malloc(1000000 * mem_size * sizeof(Content));
    for(i = 0; i < 1000000 * mem_size; i++) {
	new->memory[i] = GarbageContent;
    }

    new->page_frames = create_list();

    return new;
}

void init_simulation(Simulator *sim) {
    const int addr_size = 32;
    FILE *file;
    int (*replacement_func)(Simulator*) = sim->replacement_func;
    Page *page, *page_out;
    int page_frame;
    unsigned int address;
    char op;
    int displacement_bits, page_bits;
    int displacement, page_idx;
    int aux;
    int page_out_idx;

    file = fopen(sim->filename, "r");
    if(file == NULL) {
	raise_error_message("Error to open input file.\n");
    }

    displacement_bits = 1;
    aux = sim->page_size * 1000;
    while(aux >>= 1) {
	displacement_bits += 1;
    }

    page_bits = addr_size - displacement_bits;

    srand(time(NULL));
    int i = 0;
    while(fscanf(file, "%x %c", &address, &op) == 2 && i < 10) {
	page_idx = address >> displacement_bits;
	displacement = (0xffffffff >> page_bits) & address;

	printf("page: %d\ndisplacement: %d\n", page_idx, displacement);

	page = sim->page_table[page_idx];
	if(get_present(page)) {
	    // Página está na memória física.
	    page_frame = get_page_frame(page);
	} else if(available_space(sim)){
	    // Página não está na memória física, porém há espaço para alocação.
	    page_frame = get_list_size(sim->page_frames) * sim->page_size * 1000;
	    list_add(sim->page_frames, page_idx);
	    allocate_page(page, page_frame);
	} else {
	    // Página não está na memória física e não há espaço para alocação, sendo necessário fazer a substituição.
	    page_out_idx = replacement_func(sim);
	    page_out = sim->page_table[page_out_idx];
	    page_frame = get_page_frame(page_out);
	    deallocate_page(page_out);
	    allocate_page(page, page_frame);
	    list_replace(sim->page_frames, page_out_idx, page_idx);
	}

	if(op == 'R') {
	    set_referenced(page);
	    MemoryContent content = sim->memory[page_frame + displacement];
	} else {
	    set_modified(page);
	    sim->memory[page_frame + displacement] = Content;
	}

	i += 1;
    }

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

static int available_space(Simulator *sim) {
    // verifica se há espaço para alocar mais uma página na memória física.
    return ((get_list_size(sim->page_frames) + 1) * sim->page_size) <= (sim->mem_size * 1000);
}

static int LRU(Simulator *sim) {

}

static int NRU(Simulator *sim) {
    struct node *p;
    Page *page;
    int i, page_idx;
    int chosen;
    List *page_frames = sim->page_frames;
    List *class[4];

    for(i = 0; i < 4; i++) {
	class[i] = create_list();
    }

    p = page_frames->first;
    while(p != NULL) {
	page_idx = p->page;
	page = sim->page_table[page_idx];
	if(!(get_referenced(page) || get_modified(page))) {
	    // não referenciada, não modificada.
	    i = 0;
	} else if(!get_referenced(page) && get_modified(page)) {
	    // não referenciada, modificada.
	    i = 1;
	} else if(get_referenced(page) && !get_modified(page)) {
	    // referenciada, não modificada.
	    i = 2;
	} else {
	    // referenciada, modificada.
	    i = 3;
	}
	list_add(class[i], page_idx);

	p = p->next;
    }

    for(i = 0; i < 4; i++) {
	if(get_list_size(class[i]) > 0) {
	    // índice de menor classe escolhido ao acaso.
	    chosen = list_random(class[i]);
	    break;
	}
    }

    for(i = 0; i < 4; i++) {
	list_destroy(class[i]);
    }

    return chosen;
}

static List *create_list() {
    List *new = _malloc(sizeof(List));
    new->first = NULL;
    new->size = 0;

    return new;
}

static int get_list_size(List *list) {
    return list->size;
}

static void list_add(List *list, int page) {
    struct node *new_node = _malloc(sizeof(struct node));
    new_node->page = page;
    new_node->next = list->first;
    list->first = new_node;
    list->size += 1;
}

static void list_remove(List *list, int page) {
    struct node *previous = NULL;
    struct node *p = list->first;

    while(p->page != page) {
	previous = p;
	p = p->next;
    }

    if(previous != NULL) {
	previous->next = p->next;
    } else {
	list->first = p->next;
    }

    list->size -= 1;
}

static void list_replace(List* list, int page, int new_page) {
    struct node *p = list->first;

    while(p->page != page) {
	p = p->next;
    }

    p->page = new_page;
}

static int list_random(List *list) {
    int r = rand() % get_list_size(list);
    struct node *p = list->first;

    while(r > 0) {
	p = p->next;
	r -= 1;
    }

    return p->page;
}

static void list_destroy(List *list) {
    struct node *p = list->first;
    struct node *next;

    while(p != NULL) {
	next = p->next;
	free(p);
	p = next;
    }

    free(list);
}

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

    Simulator *sim = create_simulator(algorithm, filename, page_size, mem_size);
    init_simulation(sim);

    return 0;
}

