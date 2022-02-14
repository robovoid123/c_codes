#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "9000"

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main()
{
    fd_set masterFDS;
    fd_set readFDs;
    int fdMax;

    int listenFD;
    int newFD;
    struct sockaddr_storage remoteAddr;
    socklen_t addrLen;

    char buf[256];
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes = 1;
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&masterFDS);
    FD_ZERO(&readFDs);

    bzero(&hints, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0)
    {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next)
    {
        listenFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listenFD < 0)
        {
            continue;
        }

        setsockopt(listenFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listenFD, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(listenFD);
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai);

    if (listen(listenFD, 10) == -1)
    {
        perror("listen");
        exit(3);
    }

    FD_SET(listenFD, &masterFDS);

    fdMax = listenFD;

    for (;;)
    {
        readFDs = masterFDS;
        if (select(fdMax + 1, &readFDs, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(4);
        }

        for (i = 0; i <= fdMax; i++)
        {
            if (FD_ISSET(i, &readFDs))
            {
                if (i == listenFD)
                {
                    addrLen = sizeof remoteAddr;
                    newFD = accept(listenFD, (struct sockaddr *)&remoteAddr, &addrLen);

                    if (newFD == 1)
                    {
                        perror("accept");
                    }
                    else
                    {
                        FD_SET(newFD, &masterFDS);
                        if (newFD > fdMax)
                        {
                            fdMax = newFD;
                        }
                        printf("selectserver: new connection from %s on "
                               "socket %d\n",
                               inet_ntop(remoteAddr.ss_family,
                                         get_in_addr((struct sockaddr *)&remoteAddr),
                                         remoteIP,
                                         INET6_ADDRSTRLEN),
                               newFD);
                    }
                }
                else
                {
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0)
                    {
                        if (nbytes == 0)
                        {
                            printf("selectserver: socket %d hung up\n", i);
                        }
                        else
                        {
                            perror("recv");
                        }
                        close(i);
                        FD_CLR(i, &masterFDS);
                    }
                    else
                    {
                        for (j = 0; j <= fdMax; j++)
                        {
                            if (FD_ISSET(j, &masterFDS))
                            {
                                if (j != listenFD && j != i)
                                {
                                    if (send(j, buf, nbytes, 0) == -1)
                                    {
                                        perror("send");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
