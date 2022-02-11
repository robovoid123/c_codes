#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

int Fork()
{
    int pid = fork();
    if (pid < 0)
    {
        perror("error on fork\n");
        exit(-1);
    }
    return pid;
}

int Accept(int fd, struct sockaddr *addr, socklen_t *addr_len)
{
    int newFD;
    if ((newFD = accept(fd, addr, addr_len)) < 0)
    {
        printf("accept error\n");
        return -1;
    }
    return newFD;
}

ssize_t Read(int clientFD, void *recvBuffer, size_t size)
{
    ssize_t n;
    if ((n = read(clientFD, recvBuffer, size)) < 0)
    {
        printf("read error\n");
        return -1;
    }
    return n;
}

ssize_t Write(int clientFD, void *data, size_t size)
{
    ssize_t n;

    if ((n = write(clientFD, data, size)) < 0)
    {
        printf("write err");
        return -1;
    }
    return n;
}