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

#define MAXLINE 4096
#define LISTENQ 10
typedef struct sockaddr SA;

int main(int argc, char **argv)
{
    int sockfd, n;
    char recvline[MAXLINE + 1];
    struct sockaddr_in servaddr;

    if (argc != 2)
    {

        printf("usage: a.out <IPaddress>");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {

        printf("socket error");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5555);

    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    connect(sockfd, (SA *)&servaddr, sizeof(servaddr));

    while ((n = read(sockfd, recvline, MAXLINE)) > 0)
    {
        recvline[n] = 0;
        if (fputs(recvline, stdout) == EOF)
        {

            printf("fputs error");
            exit(1);
        }
    }

    if (n < 0)
    {
        printf("read error");
        exit(1);
    }

    exit(0);
}