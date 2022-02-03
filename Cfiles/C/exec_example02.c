
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
        printf("fork error\n");
        exit(0);
    }

    if (fd == 0){
        execlp("ping", "ping", "-c", "5", "google.com", NULL);
    }
    return 0;
}
