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
#include <netdb.h>

#include "linked_list.h"

#define LISTENQ 10
#define BUFFSIZE 1024
#define MAXLINE 4096

typedef struct sockaddr SA;

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
        exit(-1);
    }
    return newFD;
}

ssize_t Read(int clientFD, void *recvBuffer, size_t size)
{
    ssize_t n;
    if ((n = read(clientFD, recvBuffer, size)) < 0)
    {
        printf("read error\n");
        exit(-1);
    }
    return n;
}

ssize_t Write(int clientFD, void *data, size_t size)
{
    ssize_t n;

    if ((n = write(clientFD, data, size)) < 0)
    {
        printf("write err");
        exit(-1);
    }
    return n;
}

int main(int argc, char const *argv[])
{
    LinkedList ll;
    StudentDetail clientStudentDetail;
    int clientRequestCount = 0;
    get_student_ll(&ll);
    int write_flag = 1, thank_you_flag = 1; // 0 is ack , 1 not ack
    int port;

    int listenFD, newFD;
    fd_set masterFDs;
    fd_set readFDs;
    int fdMax;

    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen;
    int nBytes;

    char clientAddrBuffer[MAXLINE], recvBuffer[MAXLINE + 1];

    int yes = 1;
    int i;

    if (argc != 2)
    {
        printf("usage: server <port>");
        exit(-1);
    }

    FD_ZERO(&masterFDs);
    FD_ZERO(&readFDs);

    if ((listenFD = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error");
        exit(-1);
    }

    bzero(&serverAddr, sizeof(serverAddr));

    port = atoi(argv[1]);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    setsockopt(listenFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(listenFD, (SA *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        close(listenFD);
        printf("bind error\n");
        exit(-1);
    }

    printf("Server is listining at port %d\n", port);
    if (listen(listenFD, LISTENQ) == -1)
    {
        printf("listen ersor\n");
        exit(-1);
    }

    FD_SET(listenFD, &masterFDs);
    fdMax = listenFD;

    while (1)
    {
        readFDs = masterFDs;
        if (select(fdMax + 1, &readFDs, NULL, NULL, NULL) == -1)
        {
            printf("select error\n");
            exit(-1);
        }

        for (i = 0; i <= fdMax; i++)
        {
            if (FD_ISSET(i, &readFDs))
            {
                if (i == listenFD)
                {
                    addrLen = sizeof clientAddr;
                    bzero(&clientAddr, addrLen);
                    system("clear");
                    newFD = Accept(listenFD, (SA *)&clientAddr, &addrLen);
                    FD_SET(newFD, &masterFDs);
                    if (newFD > fdMax)
                    {
                        fdMax = newFD;
                    }
                    printf("Connection from %s, port %d\n",
                           inet_ntop(
                               AF_INET,
                               &clientAddr.sin_addr,
                               clientAddrBuffer,
                               sizeof(clientAddrBuffer)),
                           ntohs(clientAddr.sin_port));
                }
                else
                {
                    while (1)
                    {
                        if ((nBytes = Read(i, recvBuffer, MAXLINE + 1)) <= 0)
                        {

                            if (nBytes == 0)
                            {
                                printf("client on fd %d hung up\n", i);
                            }
                            else
                            {
                                printf("read error \n");
                            }
                            close(i);
                            FD_CLR(i, &masterFDs);
                        }
                        else
                        {
                            recvBuffer[nBytes] = 0;
                            // first request search for id in linked list
                            if (clientRequestCount == 0)
                            {
                                int clientID = atoi(recvBuffer);
                                if (search(&ll, clientID, &clientStudentDetail))
                                {
                                    printf("Client Student Detail not found!\n");
                                    // close conn for the client
                                    break;
                                }
                            }

                            // first request so flag is set to true
                            if (clientRequestCount == 0)
                            {
                                write_flag = 0;
                            }

                            // client said thank you
                            if (strncmp(recvBuffer, "thank_you", 9) == 0)
                            {
                                thank_you_flag = 0;
                            }

                            // client said thank you now we wait for client to say again
                            if (!thank_you_flag)
                            {
                                if (strncmp(recvBuffer, "again", 5) == 0)
                                {
                                    write_flag = 0; // client say again we set write_flag
                                }
                            }

                            if (!write_flag)
                            {
                                nBytes = Write(i, &clientStudentDetail, sizeof(StudentDetail));

                                write_flag = 1;     // reset write flag
                                thank_you_flag = 1; // reset thank_you_flag

                                clientRequestCount++;

                                print_student(&clientStudentDetail);
                                printf("client request count: %d\n\n", clientRequestCount);
                            }

                            if (strncmp(recvBuffer, "quit", 4) == 0)
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    free_ll(&ll); // deallocate memory
    return 0;
}
