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

} Queue;

static Element *create_element(void *value);

Queue *create_queue() {
    Queue *new_queue = (Queue *) malloc(sizeof(Queue));
    if(new_queue == NULL) {
	printf("Dynamic memory allocation error. \n");
	exit(-1);
    }

    new_queue->first = NULL;
    new_queue->last = NULL;
    new_queue->length = 0;

    return new_queue;
}

Queue *insert_element(Queue *queue, void *value) {
    Element *new_element = create_element(value);
    Element *last = queue->last;
    if(last == NULL) {
	queue->first = queue->last = new_element;
    } else {
	last->next = new_element;
	queue->last = new_element;
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
    if(queue->length == 0) {
	queue->last = NULL;
    }

    return first->value;
}

void destroy_queue(Queue *queue) {
    Element *p = queue->first;
    Element *next;
    while(p != NULL) {
	next = p->next;
	free(p);
	p = next;
    }

    free(queue);
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

