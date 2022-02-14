#if !defined(__SERV_STOR_LL__)
#define __SERV_STOR_LL__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "student_ll.h"

struct SS_Data
{
    int fd;
    int clientId;
    int reqCount;
    int s_flag;
    int ty_flag;
    ST_Detail data;
};

struct SS_Node
{
    struct SS_Data *data;
    struct SS_Node *next;
};

typedef struct SS_Node *SS_LL;
typedef struct SS_Data *SS_Data;

void ss_push(SS_LL *ll, SS_Data data);
void ss_print(SS_Data data);
void ss_ll_print(SS_LL ll);
int ss_search(SS_LL ll, int fd, SS_Data *data);
int ss_delete(SS_LL *ll, int fd);
void *create_ss(int fd, int clientId, ST_Detail data);
void ss_ll_free(SS_LL ll);

#endif