#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "9000"
#define BACKLOG 10

void sigchld_handler(int s)
{
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
    int sockFD, newFD;
    struct addrinfo hints, *servInfo, *p;
    struct sockaddr_storage theirAddr;
    socklen_t sinSize;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    bzero(&hints, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servInfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servInfo; p != NULL; p = p->ai_next)
    {
        if ((sockFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockFD, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockFD);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    freeaddrinfo(servInfo);

    if (listen(sockFD, BACKLOG) == -1)
    {
        perror("listen");
        error(1);
    }

    sa.__sigaction_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connection... \n");

    while (1)
    {
        sinSize = sizeof theirAddr;
        newFD = accept(sockFD, (struct sockaddr *)&theirAddr, &sinSize);
        if (newFD == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(theirAddr.ss_family, get_in_addr((struct sockaddr *)&theirAddr),
                  s,
                  sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork())
        {
            close(sockFD);
            if (send(newFD, "Hello, world!", 13, 0) == -1)
            {
                perror(newFD);
            }
            close(newFD);
            exit(0);
        }
        close(newFD);
    }
    return 0;
}
