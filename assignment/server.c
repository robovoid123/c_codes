#include "utils.h"

#define LISTENQ 10

int main(int argc, char **argv)
{
    fd_set masterFDs, readFDs;
    int listenFD, newFD;
    int fdMax;

    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen;
    int nBytes, port, optval = 1;

    char clientAddrBuff[BUFFSIZE], recvBuffer[BUFFSIZE];

    if (argc != 2)
    {
        printf("usage: server <port>\n");
        exit(-1);
    }

    FD_ZERO(&masterFDs);
    FD_ZERO(&readFDs);

    listenFD = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serverAddr, sizeof(serverAddr));
    port = atoi(argv[1]);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    setsockopt(listenFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

    Bind(listenFD, (SA *)&serverAddr, sizeof(serverAddr));

    Listen(listenFD, LISTENQ);
    FD_SET(listenFD, &masterFDs);
    fdMax = listenFD;

    printf("Server is listening at port %d\n\n", port);

    for (;;)
    {
        readFDs = masterFDs;

        if (select(fdMax + 1, &readFDs, NULL, NULL, NULL) == -1) // indefine from only readable
        {
            printf("select error\n");
            exit(-1);
        }

        for (int i = 0; i <= fdMax; i++)
        {
            if (FD_ISSET(i, &readFDs)) // if there is something to read
            {
                if (i == listenFD) // if we heard from listenFD we connect new client
                {
                    clientAddrLen = sizeof(clientAddr);
                    bzero(&clientAddr, clientAddrLen);
                    newFD = Accept(listenFD, (SA *)&clientAddr, &clientAddrLen);
                    FD_SET(newFD, &masterFDs);
                    if (newFD > fdMax)
                        fdMax = newFD;
                    printf("A Client has connected\n");
                    printf("Connection:\nFrom: %s\nPort: %d\nFD: %d\n\n",
                           inet_ntop(AF_INET, &clientAddr.sin_addr, clientAddrBuff, sizeof(clientAddrBuff)),
                           ntohs(clientAddr.sin_port),
                           newFD);
                }
                else
                {
                    if ((nBytes = Read(i, recvBuffer, BUFFSIZE + 1)) == 0) // if its 0 then client hung up
                        FD_CLR(i, &masterFDs);
                    else // if everything is ok and we heard from one of the client
                    {
                        recvBuffer[nBytes] = 0;
                        printf("fd:%d> %s\n", i, recvBuffer);
                    }
                }
            }
        }
    }
    return 0;
}