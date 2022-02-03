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

int main(int argc, char const *argv[])
{
    LinkedList ll;
    StudentDetail clientStudentDetail;
    int clientRequestCount = 0;
    get_student_ll(&ll);

    int serverFD = 0, clientFD = 0;
    ssize_t n;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen;
    int port;
    char clientAddrBuffer[MAXLINE], recvBuffer[MAXLINE + 1], sendBuffer[MAXLINE];
    pid_t pid;

    if (argc != 2)
    {
        printf("usage: server <port>");
        return -1;
    }

    if ((serverFD = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error");
        return -1;
    }

    bzero(&serverAddr, sizeof(serverAddr));

    port = atoi(argv[1]);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    bind(serverFD, (SA *)&serverAddr, sizeof(serverAddr));

    printf("Server is listining at port %d\n", port);
    listen(serverFD, LISTENQ);

    while (1)
    {
        clientLen = sizeof(clientAddr);
        bzero(&clientAddr, clientLen);

        clientFD = accept(serverFD, (SA *)&clientAddr, &clientLen);
        system("clear");
        printf("Connection from %s, port %d\n",
               inet_ntop(AF_INET, &clientAddr.sin_addr,
                         clientAddrBuffer,
                         sizeof(clientAddrBuffer)),
               ntohs(clientAddr.sin_port));

        pid = fork();
        if (pid < 0)
        {
            perror("ERROR ON fork");
            exit(-1);
        }

        if (pid == 0)
        {
            close(serverFD);

            bzero(recvBuffer, sizeof(recvBuffer));
            while ((n = read(clientFD, recvBuffer, MAXLINE + 1)) > 0)
            {

                recvBuffer[n] = 0;

                if (clientRequestCount < 1)
                {
                    int clientRegNo = atoi(recvBuffer);

                    bzero(&clientStudentDetail, sizeof(StudentDetail));
                    if (search(&ll, clientRegNo, &clientStudentDetail))
                    {
                        printf("Client Student Detail not found!\n");
                        exit(-1);
                    }

                    printf("Client Student Detail found!\n");
                    printf("ID: %d\tName: %s\tRegistration No: %lu \n",
                           clientStudentDetail.id,
                           clientStudentDetail.name,
                           clientStudentDetail.registration_no);

                    n = write(clientFD, &clientStudentDetail, sizeof(StudentDetail));
                    if (n < 0)
                    {
                        printf("write err");
                        return -1;
                    }
                }
                else
                {
                    printf("%s\n", recvBuffer);
                    if (strncmp(recvBuffer, "Thank You", 9) == 0)
                    {
                        strcpy(sendBuffer, "Do you wish to see the same information again?");
                        n = write(clientFD, sendBuffer, strlen(sendBuffer));

                        if (n < 0)
                        {
                            printf("write err");
                            return -1;
                        }
                    }
                    else if (strncmp(recvBuffer, "again", 5) == 0)
                    {
                        printf("Sending count: %d\n", clientRequestCount);
                        n = write(clientFD, &clientStudentDetail, sizeof(StudentDetail));

                        if (n < 0)
                        {
                            printf("write err");
                            return -1;
                        }
                    }
                    else if (strncmp(recvBuffer, "quit", 4) == 0)
                    {
                        printf("Closing client connection\n");
                        close(clientFD);
                        exit(0);
                    }
                }

                clientRequestCount++;
                bzero(recvBuffer, sizeof(recvBuffer));
            }

            close(clientFD);
            exit(0);
        }
        else
        {
            close(clientFD);
        }
    }

    return 0;
}