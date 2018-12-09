typedef struct simulator Simulator;

Simulator *create_simulator(char *algorithm, char *filename, int page_size, int mem_size);

void init_simulation(Simulator *sim, int *fault_count, int *dirty_count);

