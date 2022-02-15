#include "server_store_ll.h"
#include "utils.h"

#define STDIN 0

struct Message
{
    char msg[200];
    char buffer[BUFFSIZE];
    struct ST_Detail detail;
};
typedef struct Message *Message;

int main(int argc, char **argv)
{
    int sockFD, nBytes, port;
    char writeBuff[50];

    Message req = malloc(sizeof(struct Message));
    Message res = malloc(sizeof(struct Message));

    fd_set masterFDs, readFDs;
    int fdMax;

    struct sockaddr_in serverAddr;

    if (argc != 4)
    {
        printf("usage: client <ip_address> <port> <clientid>");
        exit(-1);
    }

    FD_ZERO(&masterFDs);
    FD_ZERO(&readFDs);

    sockFD = Socket(AF_INET, SOCK_STREAM, 0);

    port = atoi(argv[2]);
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, argv[1], &serverAddr.sin_addr) < 0)
    {
        printf("ip address error\n");
        exit(-1);
    }

    if (connect(sockFD, (SA *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        printf("connect error\n");
        exit(-1);
    }

    printf("Connected to the server\n");

    FD_SET(STDIN, &masterFDs);
    FD_SET(sockFD, &masterFDs);
    fdMax = sockFD;

    bzero(req, sizeof(struct Message));
    strcpy(req->msg, "buffer");
    strcpy(req->buffer, argv[3]);

    Write(sockFD, req, sizeof(struct Message));

    for (;;)
    {
        readFDs = masterFDs;

        if (select(fdMax + 1, &readFDs, NULL, NULL, NULL) == -1)
        {
            printf("select error\n");
            exit(-1);
        }

        if (FD_ISSET(STDIN, &readFDs)) // from stdin
        {
            nBytes = Read(0, writeBuff, 50);
            if (strncmp(writeBuff, "quit", 4) == 0)
            {
                printf("Closing connection\n");
                FD_CLR(sockFD, &masterFDs);
                close(sockFD);
                exit(0);
            }
            writeBuff[nBytes] = 0;

            bzero(req, sizeof(struct Message));
            strcpy(req->msg, "buffer");
            strncpy(req->buffer, writeBuff, nBytes);
            Write(sockFD, req, sizeof(struct Message));
        }
        else if (FD_ISSET(sockFD, &readFDs)) // from socket
        {
            bzero(res, sizeof(struct Message));
            nBytes = Read(sockFD, res, sizeof(struct Message));

            if (strncmp(res->msg, "buffer", 6) == 0)
            {
                printf("server> %s %s\n", res->msg, res->buffer);

                if (strncmp(res->buffer, "not_found", 9) == 0)
                {
                    printf("No client data found by server\n");
                    printf("Closing connection\n");
                    FD_CLR(sockFD, &masterFDs);
                    close(sockFD);
                    exit(-1);
                    return 1;
                }
            }

            if (strncmp(res->msg, "detail", 6) == 0)
            {
                printf("\n");
                printf("Displaying info:\n");
                st_print(&(res->detail));
                printf("\n");

                bzero(req, sizeof(struct Message));
                strcpy(req->msg, "buffer");
                strcpy(req->buffer, "thank_you");
                Write(sockFD, req, sizeof(struct Message));
            }
        }
    }

    close(sockFD);
    return 0;
}