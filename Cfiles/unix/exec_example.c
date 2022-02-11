#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int
main(int argv, char **args)
{
    int fd = fork();
    if (fd == -1){
        printf("fork error");
        exit(0);
    }
    if (fd == 0){
        execlp("ping", "ping", "-c", "5", "google.com", NULL);
    } else {
        wait(NULL);
        printf(" this is parent\n");
        printf("success!!!!!!!!!!\n");
    }
    return 0;
}
