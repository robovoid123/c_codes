#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define DQUEU_FRONT 0
#define DQUEU_BACK 1

#define DQUEU_EMPTY -1
#define NUM_DIRECTION 2

struct dqueu{
    struct dqueu *next[NUM_DIRECTION];
    int value;
};

typedef struct dqueu Dqueu;

Dqueu *
createDqueu(void)
{
    Dqueu *d;
    d = malloc(offsetof(struct dqueu, value));

    if(d){
        d->next[DQUEU_FRONT] = d->next[DQUEU_BACK] = d;
    }

    return d;
}

void
dqueuPush(Dqueu *d, int direction, int value)
{
    struct dqueu *e;
    e = malloc(sizeof(struct dqueu));

    e->next[direction] = d->next[direction];
    e->next[!direction] = d;
    e->value = value;

    d->next[direction] = e;
    e->next[direction]->next[!direction] = e;
}

int
dqueuPop(Dqueu *d, int direction)
{
    struct dqueu *e;
    int ret;

    e = d->next[direction];

    if(d == e){
        return DQUEU_EMPTY;
    }

    d->next[direction] = e->next[direction];
    e->next[direction]->next[!direction] = d;

    ret = e->value;

    free(e);

    return ret;
}

int
dqueuIsEmpty(Dqueu *d)
{
    return d->next[DQUEU_FRONT] == d;
}

void
destroyDqueu(Dqueu *d)
{
    while(!dqueuIsEmpty(d))
    {
        dqueuPop(d, DQUEU_FRONT);
    }

    free(d);
}
