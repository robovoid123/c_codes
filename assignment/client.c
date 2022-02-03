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

int main(int argc, char **argv)
{
    int clientFD = 0;
    ssize_t n = 0;
    StudentDetail clientStudentDetail;
    int requestCount = 0;
    char sendBuffer[MAXLINE], recvBuffer[MAXLINE + 1];
    int port;
    struct sockaddr_in serverAddress;
    int again = 1;

    if (argc != 3)
    {
        printf("usage: client <ipaddress> <port>");
        return -1;
    }

    if ((clientFD = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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

    if (connect(clientFD, (SA *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        printf("connect error");
        return -1;
    }

    while (1)
    {
        bzero(sendBuffer, sizeof(sendBuffer));
        if (requestCount < 1)
        {
            printf("First request\n");
            printf("Enter your registration no: ");
            fgets(sendBuffer, MAXLINE, stdin);
            printf("\n");
            write(clientFD, sendBuffer, strlen(sendBuffer));

            if (recv(clientFD, &clientStudentDetail, sizeof(StudentDetail), 0) == 0)
            {
                printf("recv error\n");
                return -1;
            }

            printf("Client Student Detail found!\n");
            printf("ID: %d\tName: %s\tRegistration No: %lu\n",
                   clientStudentDetail.id,
                   clientStudentDetail.name,
                   clientStudentDetail.registration_no);
        }
        else
        {
            printf("Request count: %d\n", requestCount);
            bzero(recvBuffer, MAXLINE + 1);
            if (n = read(clientFD, recvBuffer, MAXLINE + 1) < 0)
            {
                printf("reading error\n");
                return -1;
            }

            recvBuffer[n] = 0;

            printf("%s\n", recvBuffer);

            printf("Do you want to go again?\n");
            fgets(sendBuffer, MAXLINE, stdin);
            write(clientFD, sendBuffer, strlen(sendBuffer));
            if (strncmp(sendBuffer, "again", 5) == 0)
            {

                if (recv(clientFD, &clientStudentDetail, sizeof(StudentDetail), 0) == 0)
                {
                    printf("recv error\n");
                    return -1;
                }

                printf("Client Student Detail found!\n");
                printf("ID: %d\tName: %s\tRegistration No: %lu\n",
                       clientStudentDetail.id,
                       clientStudentDetail.name,
                       clientStudentDetail.registration_no);
            }
            else if (strncmp(sendBuffer, "quit", 4) == 0)
            {
                close(clientFD);
                exit(0);
            }
        }

        requestCount++;
        bzero(sendBuffer, MAXLINE);
        strcpy(sendBuffer, "Thank You");
        write(clientFD, sendBuffer, strlen(sendBuffer));

        bzero(&clientStudentDetail, sizeof(StudentDetail));
    }
    close(clientFD);
    exit(0);
}