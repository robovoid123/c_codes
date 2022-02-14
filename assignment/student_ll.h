#if !defined(__STUD_LL__)
#define __STUD_LL__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ST_Detail
{
    unsigned int id;
    char name[200];
    unsigned long regNo;
};

struct ST_Node
{
    struct ST_Detail *data;
    struct ST_Node *next;
};

typedef struct ST_Node *ST_LL;
typedef struct ST_Detail *ST_Detail;
void st_push(ST_LL *ll, ST_Detail data);
void st_print(ST_Detail sd);
void st_ll_print(ST_LL ll);
int st_search(ST_LL ll, int id, ST_Detail *data);
void st_ll_free(ST_LL *ll);
void *st_create(int id, char *name, int regNo);
void *st_ll_get();

#endif