#include "utils.h"

int Socket(int domain, int type, int protocol)
{
    int listenFD = socket(domain, type, protocol);
    if (listenFD < 0)
    {
        perror("socket error\n");
        exit(-1);
    }
    return listenFD;
}

void Bind(int fd, const struct sockaddr *addr, socklen_t len)
{
    if (bind(fd, addr, len) < 0)
    {
        close(fd);
        perror("bind error\n");
        exit(-1);
    }
}

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
        perror("accept error\n");
        exit(-1);
    }
    return newFD;
}

void Listen(int fd, int backlog)
{
    if (listen(fd, backlog) < 0)
    {
        perror("listen error\n");
        exit(-1);
    }
}

ssize_t Read(int clientFD, void *recvBuffer, size_t size)
{
    ssize_t n;
    if ((n = read(clientFD, recvBuffer, size)) <= 0)
    {
        if (n == 0)
            printf("closed by the client\n");
        else
            printf("read error\n");
        close(clientFD);
    }
    return n;
}

ssize_t Write(int clientFD, void *data, size_t size)
{
    ssize_t n;
    if ((n = write(clientFD, data, size)) < 0)
    {
        printf("write err\n");
        exit(-1);
    }
    return n;
}