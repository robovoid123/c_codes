#include "server_store_ll.h"

void ss_push(SS_LL *ll, SS_Data data)
{
    struct SS_Node *e = malloc(sizeof(e));
    e->data = data;
    e->next = (*ll);
    *ll = e;
}

void ss_print(SS_Data data)
{
    printf("fd: %d  clientId: %d\nreqCount: %d\nsend flag: %d thank you flag: %d\n",
           data->fd,
           data->clientId,
           data->reqCount,
           data->s_flag,
           data->ty_flag);
    st_print(data->data);
}

void ss_ll_print(SS_LL ll)
{
    for (struct SS_Node *temp = ll;
         temp->next != NULL; temp = temp->next)
    {
        ss_print(temp->data);
        printf("\n\n");
    }
}

int ss_search(SS_LL ll, int fd, SS_Data *data)
{
    for (struct SS_Node *temp = ll;
         temp->next != NULL; temp = temp->next)
    {
        if (temp->data->fd == fd)
        {
            *data = temp->data;
            return 0;
        }
    }
    return -1;
}

int ss_delete(SS_LL *ll, int fd)
{
    struct SS_Node *prev = NULL;

    for (struct SS_Node *temp = *ll;
         temp->next != NULL; temp = temp->next)
    {
        if (temp->data->fd == fd)
        {
            if (prev == NULL) // if head
            {
                *ll = temp->next;
                free(temp);
                return 0;
            }

            prev->next = temp->next;
            free(temp);
            return 0;
        }
        prev = temp;
    }
    return -1;
}

void *create_ss(int fd, int clientId, ST_Detail data)
{
    SS_Data temp = malloc(sizeof(temp));
    temp->fd = fd;
    temp->clientId = clientId;
    temp->data = st_create(data->id, data->name, data->regNo);
    temp->reqCount = 0;
    temp->s_flag = 0;
    temp->ty_flag = 0;
    return temp;
}

void ss_ll_free(SS_LL ll)
{
    struct SS_Node *e;
    for (struct SS_Node *temp = ll;
         temp->next != NULL; temp = temp->next)
    {
        e = temp;
        free(e);
    }
}

// int main()
// {
//     SS_LL ss_ll = malloc(sizeof(ss_ll));
//     ST_LL st_ll = st_ll_get();

//     ST_Detail std = malloc(sizeof(std));

//     st_search(st_ll, 1, &std);
//     SS_Data d1 = create_ss(1, 1, std);

//     st_search(st_ll, 2, &std);
//     SS_Data d2 = create_ss(2, 2, std);

//     ss_push(&ss_ll, d1);
//     ss_push(&ss_ll, d2);
//     ss_delete(&ss_ll, 2);

//     ss_ll_print(ss_ll);

//     return 0;
// }