#include "utils.h"

#define STDIN 0

int main(int argc, char **argv)
{
    int sockFD, nBytes, port;
    char sendBuff[BUFFSIZE], recvBuff[BUFFSIZE + 1];

    fd_set masterFDs, readFDs;
    int listenFD, newFD;
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
            nBytes = Read(0, sendBuff, BUFFSIZE);
            if (strncmp(sendBuff, "quit", 4) == 0)
            {
                printf("Closing connection\n");
                FD_CLR(sockFD, &masterFDs);
                close(sockFD);
                exit(0);
            }
            sendBuff[nBytes] = 0;
            Write(sockFD, sendBuff, BUFFSIZE);
        }
        else if (FD_ISSET(sockFD, &readFDs)) // from socket
        {
            // FIXME: if we close connection we server gets read error
            nBytes = Read(sockFD, recvBuff, BUFFSIZE);
            recvBuff[nBytes] = 0;
            printf("%s", recvBuff);
        }
    }

    close(sockFD);
    return 0;
}