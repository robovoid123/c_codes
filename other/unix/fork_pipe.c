#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define PIPE_READ (0)
#define PIPE_WRITE (1)
#define PIPE_ERROE (2)
#define FORK_ERROR (3)
#define PIPE_WRITE_ERROR (4)
#define PIPE_READ_ERROR (5)

int main(int argc, char **argv)
{
    int array[] = {1, 2, 6, 7, 1, 2, 3, 6, 7, 1};
    int arraySize = sizeof(array) / sizeof(int);
    int start, end;
    int fd[2];

    if (pipe(fd) == -1)
    {
        return PIPE_ERROE;
    }

    int pid = fork();
    if (pid == -1)
    {
        return FORK_ERROR;
    }

    if (pid == 0)
    {
        start = 0;
        end = arraySize / 2;
    }
    else
    {
        start = arraySize / 2;
        end = arraySize;
    }

    int sum;

    for (int i = start; i < end; i++)
    {
        sum += array[i];
    }

    printf("partial sum is : %d\n", sum);

    if (pid == 0)
    {
        close(fd[PIPE_READ]);
        if (write(fd[PIPE_WRITE], &sum, sizeof(sum)) == -1)
        {
            return PIPE_WRITE_ERROR;
        }
        close(fd[PIPE_WRITE]);
    }
    else
    {
        int recv;
        close(fd[PIPE_WRITE]);
        if (read(fd[PIPE_READ], &recv, sizeof(recv)) == -1)
        {
            return PIPE_READ_ERROR;
        }
        close(fd[PIPE_READ]);
        printf("the full sum is: %d\n", recv + sum);
    }

    wait(NULL);

    return 0;
}