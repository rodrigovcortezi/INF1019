typedef struct queue Queue;

Queue *create_queue(void(*func)(void *));

Queue *insert_element(Queue *queue, void *value);

void *remove_element(Queue *queue);

