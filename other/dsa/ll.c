#include <stdio.h>
#include <stdlib.h>

struct Data
{
    int data;
};

struct Node
{
    struct Data *data;
    struct Node *next;
};

typedef struct Node *LL;
typedef struct Data *Data;

void push(LL *head, Data data)
{
    struct Node *e;
    e = (struct Node *)malloc(sizeof(struct Node));
    e->data = data;
    e->next = (*head);
    (*head) = e;
}

int main()
{
    LL head = (LL)malloc(sizeof(LL));
    head->next = NULL;

    int da[] = {1, 2, 3, 4, 5, 6};

    for (int i = 0; i < 5; i++)
    {
        struct Data *temp = (struct Data *)malloc(sizeof(struct Data));
        temp->data = da[i];
        insert(&head, temp);
    }

    for (LL temp = head; temp->next != NULL; temp = temp->next)
    {
        printf("data: %d\n", temp->data->data);
    }

    return 0;
}