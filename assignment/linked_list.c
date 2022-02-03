#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

/**
 * @brief adds a new node at the end of the linked list
 * 
 * @param ll LinkedList * 
 * @param data StudentDetail *
 * @return ** void 
 */
void insert(LinkedList *ll, StudentDetail *data)
{
    struct node *e, *temp;

    e = malloc(sizeof(struct node));
    e->next = NULL;
    e->data = data;

    if (*ll == NULL)
    {
        *ll = e;
    }
    else
    {
        temp = *ll;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = e;
    }
}

/**
 * @brief  prints all the node in linked list
 * 
 * @param ll LinkedList *
 * @return ** void 
 */
void print_ll(LinkedList *ll)
{
    struct node *temp;
    temp = *ll;

    while (temp != NULL)
    {
        printf("%d %s %lu\n", temp->data->id, temp->data->name, temp->data->registration_no);
        temp = temp->next;
    }
}

/**
 * @brief search for if any node in the linked list has the matching registration number
 * 
 * @param ll 
 * @param registration_no 
 * @param data 
 * @return ** int  0 if success 1 if not found
 */
int search(LinkedList *ll, int registration_no, StudentDetail *data)
{
    struct node *temp;
    temp = *ll;

    while (temp != NULL)
    {
        if (temp->data->registration_no == registration_no)
        {
            strcpy(data->name, temp->data->name);
            data->id = temp->data->id;
            data->registration_no = temp->data->registration_no;
            return 0;
        }
        temp = temp->next;
    }
    return 1;
}

void print_student(StudentDetail *sd)
{
    printf("ID: %d\tName: %s\tRegistration No: %lu", sd->id, sd->name, sd->registration_no);
}

StudentDetail *create_student(unsigned int id, char *name, unsigned long registration_no)
{
    StudentDetail *st;
    st = malloc(sizeof(StudentDetail));
    st->id = id;
    strcpy(st->name, name);
    st->registration_no = registration_no;
    return st;
}

/**
 * @brief Get the student ll object
 * 
 * @param ll 
 * @return int 
 */
int get_student_ll(LinkedList *ll)
{
    *ll = NULL;
    StudentDetail *c1 = create_student(1, "Aashish", 171501);
    StudentDetail *c2 = create_student(2, "Aashuthoh", 171502);
    StudentDetail *c3 = create_student(3, "Abinash", 171503);
    StudentDetail *c4 = create_student(4, "Aakriti", 171504);
    StudentDetail *c5 = create_student(5, "Avash", 171505);
    StudentDetail *c6 = create_student(6, "Biraj", 171506);
    insert(ll, c1);
    insert(ll, c2);
    insert(ll, c3);
    insert(ll, c4);
    insert(ll, c5);
    insert(ll, c6);
    return 0;
}
