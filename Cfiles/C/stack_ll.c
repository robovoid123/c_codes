#include <stdio.h>
#include <stdlib.h>

struct elt {
    int val;
    struct elt *next;
};

typedef struct elt *Stack;
#define STACK_EMPTY (0)

void
stackPush(Stack *s, int val)
{
    struct elt *e;

    e = malloc(sizeof(struct elt));

    e->val = val;
    e->next = *s;

    *s = e;
}

int
stackEmpty(Stack *s)
{
    return (*s == 0);
}

int
stackPop(Stack *s)
{
    int ret;
    struct elt *e;

    ret = (*s)->val;
    e = *s;
    *s = e->next;
    free(e);
    return ret;
}

void
stackPrint(Stack *s)
{
    struct elt *e;
    for(e=*s; e!=0; e=e->next) {
        printf("%d ", e->val);
    }
    putchar('\n');
}

int
main(int argc, char **argv)
{
    int i;
    Stack s;
    s = STACK_EMPTY;
    for(i = 0; i < 5; i++) {
        printf("push %d\n", i);
        stackPush(&s, i);
        stackPrint(&s);
    }

    while(!stackEmpty(&s)) {
        printf("pop gets %d\n", stackPop(&s));
        stackPrint(&s);
    }
    return 0;
}