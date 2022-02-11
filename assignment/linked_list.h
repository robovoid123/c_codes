#if !defined(__LINKED_LIST__)
#define __LINKED_LIST__

struct StudentDetail
{
    unsigned int id;
    char name[200];
    unsigned long registration_no;
    unsigned long msg_sent_count;
};

struct node
{
    struct StudentDetail *data;
    struct node *next;
};

typedef struct node *LinkedList;
typedef struct StudentDetail StudentDetail;
void insert(LinkedList *ll, StudentDetail *data);
void print_student(StudentDetail *sd);
void print_ll(LinkedList *ll);
int search(LinkedList *ll, int registration_no, StudentDetail *data);
int get_student_ll(LinkedList *ll);
void free_ll(LinkedList *ll);

#endif