
typedef struct scheduler Scheduler;

void exec(Scheduler *scheduler);

Scheduler *create_scheduler();

void add_program(Scheduler *scheduler, char *program_name, int priority);

