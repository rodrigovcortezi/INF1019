
typedef struct scheduler Scheduler;

void exec();

Scheduler *create_scheduler();

void add_program(Scheduler *scheduler, char *program_name, int priority);

