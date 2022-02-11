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

#include "linked_list.h"

#define LISTENQ 10
#define BUFFSIZE 1024
#define MAXLINE 4096


typedef struct sockaddr SA;

union Response
{
    StudentDetail detail;
    char buffer[MAXLINE + 1];
};

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

int main(int argc, char **argv)
{
    int sockFD = 0;
    ssize_t n = 0;
    union Response response;
    int requestCount = 0;
    char sendBuffer[MAXLINE], recvBuffer[MAXLINE + 1];
    int port;
    struct sockaddr_in serverAddress;
    int again = 1;

    if (argc != 4)
    {
        printf("usage: client <ipaddress> <port> <id>");
        return -1;
    }

    if ((sockFD = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error");
        return -1;
    }

    port = atoi(argv[2]);
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    if (inet_pton(AF_INET, argv[1], &serverAddress.sin_addr) < 0)
    {
        printf("inet pton error");
        return -1;
    }

    if (connect(sockFD, (SA *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        printf("connect error");
        return -1;
    }

    Write(sockFD, argv[3], strlen(argv[3]));

    while (1)
    {
        Read(sockFD, &response, sizeof(StudentDetail));
        print_student(&response.detail);
        printf("Request count: %d\n", requestCount);

        if (n = read(sockFD, recvBuffer, MAXLINE + 1) < 0)
        {
            printf("reading error\n");
            return -1;
        }

        recvBuffer[n] = 0;

        printf("recvd from server %s\n", recvBuffer);

        printf("Do you want to go again?\n");
        fgets(sendBuffer, MAXLINE, stdin);
        write(sockFD, sendBuffer, strlen(sendBuffer));
        if (strncmp(sendBuffer, "again", 5) == 0)
        {

            if (recv(sockFD, &response.detail, sizeof(StudentDetail), 0) == 0)
            {
                printf("recv error\n");
                return -1;
            }

            print_student(&response.detail);
        }
        else if (strncmp(sendBuffer, "quit", 4) == 0)
        {
            close(sockFD);
            exit(0);
        }

        requestCount++;
    }
    close(sockFD);
    exit(0);
}
