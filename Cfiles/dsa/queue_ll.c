#include <stdio.h>
#include <stdlib.h>

struct elt {
    int value;
    struct elt *next;
};

struct queue {
    struct elt *head;
    struct elt *tail;
};

struct queue *
createQueue(void)
{
    struct queue *q;
    q = malloc(sizeof(struct queue));
    q -> head = q -> tail = 0;
    return q;
}

void
enq(struct queue *q, int value)
{
    struct elt *e;
    e = malloc(sizeof(struct elt));

    e->value = value;
    e->next = 0;

    if (q->head == 0){
        q->head = e;
    } else {
        q->tail->next = e;
    }
    q->tail = e;
}

int
queueEmpty(struct queue *q)
{
    return (q->head == 0);
}

int
deq(struct queue *q)
{
    int ret;
    struct elt *e;

    ret = q->head->value;
    e = q->head;
    q->head = e->next;
    free(e);

    return ret;

}

void
queuePrint(struct queue *q)
{
    struct elt *e;

    for(e = q->head; e != 0; e=e->next){
        printf("%d ", e->value);
    }
    printf("\n");
}

void
queueDestroy(struct queue *q)
{
    while(!queueEmpty(q))
        deq(q);

    free(q);
}

int
main(int argc, char **argv)
{
    int i;
    struct queue *q;

    q = createQueue();

    for(i=0; i<5; i++){
        printf("enq %d\n", i);
        enq(q, i);
        queuePrint(q);
    }

    while(!queueEmpty(q)){
        printf("deq %d\n", deq(q));
        queuePrint(q);
    }

    queueDestroy(q);
    return 0;
}