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
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    char buff[MAXLINE];
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
        sleep(3);
        len = sizeof(cliaddr);
        connfd = accept(listenfd, (SA *) &cliaddr, &len);
        sleep(3);
        printf("connection from %s, port %d\n",
            inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
            ntohs(cliaddr.sin_port));
        sleep(3);
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        write(connfd, buff, strlen(buff));
        sleep(3);
        close(connfd);
        sleep(3);
    }
}

