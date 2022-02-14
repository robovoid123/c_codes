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

int main(int argc, char const *argv[])
{
    LinkedList ll;
    StudentDetail clientStudentDetail;
    int clientRequestCount = 0;
    get_student_ll(&ll);
    int write_flag = 1, thank_you_flag = 1; // 0 is ack , 1 not ack
    int sockFD = 0, clientFD = 0;
    ssize_t n;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen;
    int port;
    char clientAddrBuffer[MAXLINE], recvBuffer[MAXLINE + 1];
    pid_t pid;

    if (argc != 2)
    {
        printf("usage: server <port>");
        return -1;
    }

    if ((sockFD = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error");
        return -1;
    }

    bzero(&serverAddr, sizeof(serverAddr));

    port = atoi(argv[1]);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    bind(sockFD, (SA *)&serverAddr, sizeof(serverAddr));

    printf("Server is listining at port %d\n", port);
    listen(sockFD, LISTENQ);

    while (1)
    {
        clientLen = sizeof(clientAddr);
        bzero(&clientAddr, clientLen);

        clientFD = Accept(sockFD, (SA *)&clientAddr, &clientLen);

        system("clear");

        printf("Connection from %s, port %d\n",
               inet_ntop(
                   AF_INET,
                   &clientAddr.sin_addr,
                   clientAddrBuffer,
                   sizeof(clientAddrBuffer)),
               ntohs(clientAddr.sin_port));

        if ((pid = Fork()) == 0)
        {
            close(sockFD);

            while (1)
            {
                n = Read(clientFD, recvBuffer, MAXLINE + 1);
                recvBuffer[n] = 0;

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
                    n = Write(clientFD, &clientStudentDetail, sizeof(StudentDetail));

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

            printf("closing connection to client\n\n");
            close(clientFD);
            exit(0);
        }
    }

    free_ll(&ll); // deallocate memory
    return 0;
}