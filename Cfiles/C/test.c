#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

int
main(int argc, char **argv)
{
    int n;
    int id = fork();

    if(id != 0){
        n = 6;
        wait(NULL);
    } else {
        n = 1;
    }

    if(id != 0){
        putchar('\n');
    }

    for(int i=n; i<n+5; i++){
        printf("%d ", i);
        fflush(stdout);
    }


    return 0;
}