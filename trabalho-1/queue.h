typedef struct queue Queue;

Queue *create_queue();

Queue *insert_element(Queue *queue, void *value);

void *remove_element(Queue *queue);

void destroy_queue(Queue *queue);

