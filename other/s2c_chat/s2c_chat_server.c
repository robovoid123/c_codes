#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define MAXLINE 1024
#define LISTENQ 10
typedef struct sockaddr SA;

int main(int argc, char **argv)
{
    int listenfd, connfd, n;
    struct sockaddr_in servaddr, cliaddr;
    char buff[MAXLINE];
    char recvline[MAXLINE+1];
    char sendline[MAXLINE+1];
    time_t ticks;
    socklen_t len;
    int port;

    listenfd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM protocol defaults to TCP
    port = atoi(argv[1]); // string to integer
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // host to network long
    servaddr.sin_port = htons(port);

    /*
        bind can take any kind of socket addr so we need to pass a
        generic address
    */
    bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
    printf("Server is waiting connection  at port 1234\n");
    listen(listenfd, LISTENQ);

    for(;;)
    {
        len = sizeof(cliaddr);
        connfd = accept(listenfd, (SA *) &cliaddr, &len);
        printf("connection from %s, port %d\n",
            inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
            ntohs(cliaddr.sin_port));

        while ((n=read(connfd, recvline, MAXLINE)) > 0) {
            recvline[n] = 0;
            printf("recvline: %s\n", recvline);
            printf("enter a line:\t");
            scanf("%s", sendline);
            write(connfd, sendline, strlen(sendline));
            memset(recvline, 0, sizeof(recvline));
        }

    }
}

