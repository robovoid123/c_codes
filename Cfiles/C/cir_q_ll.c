#include <stdio.h>
#include <stdlib.h>

#define QUEUE_EMPTY -1
#define QUEUE_NULL 0

struct elt{
    int value;
    struct elt *next;
};

struct queue{
    struct elt *front;
    struct elt *rear;
};

void
enq(struct queue *q, int value)
{
    struct elt *e;

    e = malloc(sizeof(struct elt));

    e->value = value;

    if(q->front == 0){
        q->front = e;
    } else {
        q->rear->next = e;
    }
    q->rear = e;
    q->rear->next = q->front;
}

int
deq(struct queue *q)
{
    struct elt *e;
    int ret;

    if(q->front == 0){
        printf("queue empty\n");
        return QUEUE_EMPTY;
    }

    e = q->front;
    ret = e->value;

    if(q->front == q->rear){
        q->front = QUEUE_NULL;
        q->rear = QUEUE_NULL;
    } else {
        q->front = q->front->next;
        q->rear->next = q->front;
    }

    free(e);

    return ret;
}

int
main(int argv, char **args)
{
    return 0;
}
