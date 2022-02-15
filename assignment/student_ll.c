#include "student_ll.h"

void st_push(ST_LL *ll, ST_Detail data)
{
    struct ST_Node *e = malloc(sizeof(e));
    e->data = data;
    e->next = (*ll);
    *ll = e;
}

void st_print(ST_Detail sd)
{
    printf("ID: %d\tName: %s\tRegistration No: %lu\n",
           sd->id, sd->name, sd->regNo);
}

void st_ll_print(ST_LL ll)
{
    for (struct ST_Node *temp = ll;
         temp->next != NULL; temp = temp->next)
        st_print(temp->data);
}

int st_search(ST_LL ll, int id, ST_Detail *data)
{
    for (struct ST_Node *temp = ll;
         temp->next != NULL; temp = temp->next)
    {
        if (temp->data->id == id)
        {
            *data = temp->data;
            return 0;
        }
    }
    return -1;
}

void st_ll_free(ST_LL *ll)
{
    struct ST_Node *e;
    for (struct ST_Node *temp;
         temp->next != NULL; temp = temp->next)
    {
        e = temp;
        free(e);
    }
}

void *st_create(int id, char *name, int regNo)
{
    ST_Detail temp = malloc(sizeof(temp));
    temp->id = id;
    temp->regNo = regNo;
    strcpy(temp->name, name);
    return temp;
}

void *st_ll_get()
{
    ST_LL ll = malloc(sizeof(ll));
    ll->next = NULL;

    ST_Detail c1 = st_create(1, "Aashish", 171501);
    ST_Detail c2 = st_create(2, "Aashuthoh", 171502);
    ST_Detail c3 = st_create(3, "Abinash", 171503);
    ST_Detail c4 = st_create(4, "Aakriti", 171504);
    ST_Detail c5 = st_create(5, "Avash", 171505);
    ST_Detail c6 = st_create(6, "Biraj", 171506);

    st_push(&ll, c1);
    st_push(&ll, c2);
    st_push(&ll, c3);
    st_push(&ll, c4);
    st_push(&ll, c5);
    st_push(&ll, c6);

    return ll;
}