#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

typedef struct element {

    void *value;

    struct element *next;

} Element;

typedef struct queue {

    Element *first;

    Element *last;

    int length;

    void (*free_value) (void *value);

} Queue;

static Element *create_element(void *value);

static void free_element(Queue *queue, Element *element);

Queue *create_queue(void (*func)(void *)) {
    Queue *new_queue = (Queue *) malloc(sizeof(Queue));
    if(new_queue == NULL) {
	printf("Dynamic memory allocation error. \n");
	exit(-1);
    }

    new_queue->first = NULL;
    new_queue->last = NULL;
    new_queue->length = 0;
    new_queue->free_value = func;

    return new_queue;
}

Queue *insert_element(Queue *queue, void *value) {
    Element *new_element = create_element(value);
    Element *last = queue->last;
    if(last == NULL) {
	queue->first = queue->last = new_element;
    } else {
	last->next = new_element;
    }

    queue->length += 1;

    return queue;
}

void *remove_element(Queue *queue) {
    Element *first = queue->first;
    if(first == NULL) {
	return NULL;
    }

    queue->first = first->next;
    queue->length -= 1;
    if(queue->length == 1) {
	queue->last = queue->first;
    }

    return first->value;
}

static Element *create_element(void *value) {
    Element *new_element = (Element *) malloc(sizeof(Element));
    if(new_element == NULL) {
	printf("Dynamic memory allocation error. \n");
	exit(-1);
    }

    new_element->value = value;
    new_element->next = NULL;

    return new_element;
}

static void free_element(Queue *queue, Element *element) {
    queue->free_value(element->value);
    free(element);
}

