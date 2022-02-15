#include "server_store_ll.h"
#include "utils.h"

#define LISTENQ 10
struct Message
{
    char msg[200];
    char buffer[BUFFSIZE];
    struct ST_Detail detail;
};
typedef struct Message *Message;

void *ss_init()
{
    SS_LL serverStore = malloc(sizeof(serverStore));
    serverStore->next = NULL;
    return serverStore;
}

int main(int argc, char **argv)
{
    SS_LL serverStore = ss_init();
    ST_LL studentStore = st_ll_get();
    SS_Data clientData = malloc(sizeof(clientData));

    Message req = malloc(sizeof(struct Message));
    Message res = malloc(sizeof(struct Message));

    fd_set masterFDs, readFDs;
    int listenFD, newFD;
    int fdMax;

    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen;
    int nBytes, port, optval = 1;

    char clientAddrBuff[BUFFSIZE];

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
                    bzero(res, sizeof(struct Message));
                    if ((nBytes = Read(i, res, sizeof(struct Message))) == 0)
                    {
                        FD_CLR(i, &masterFDs); // if its 0 then client hung up
                        ss_delete(&serverStore, i);
                    }
                    else // if everything is ok and we heard from one of the client
                    {
                        if (ss_search(serverStore, i, &clientData) < 0)
                        {
                            int id = atoi(res->buffer);
                            ST_Detail stDetail = malloc(sizeof(stDetail));
                            if (st_search(studentStore, id, &stDetail) < 0)
                            {
                                printf("Not found in StudentStore\n");

                                bzero(req, sizeof(struct Message));
                                strcpy(req->msg, "buffer");
                                strcpy(req->buffer, "not_found");
                                Write(i, req, sizeof(struct Message));
                            }
                            else
                            {
                                SS_Data newClientData = create_ss(i, id, stDetail);
                                newClientData->s_flag = 1;
                                ss_push(&serverStore, newClientData);
                                // ss_ll_print(serverStore);
                                clientData = newClientData;
                            }
                        }

                        if (strncmp(res->msg, "buffer", 6) == 0)
                        {
                            printf("fd:%d> %s %s\n", i, res->msg, res->buffer);

                            if (strncmp(res->buffer, "thank_you", 9) == 0)
                            {
                                clientData->ty_flag = 1;
                            }

                            if (clientData->ty_flag == 1 && strncmp(res->buffer, "again", 5) == 0)
                            {
                                clientData->ty_flag = 0;
                                clientData->s_flag = 1;
                            }
                        }

                        if (clientData->s_flag == 1)
                        {
                            bzero(req, sizeof(struct Message));
                            strcpy(req->msg, "detail");
                            req->detail = *clientData->data;
                            Write(i, req, sizeof(struct Message));
                            clientData->s_flag = 0;
                        }
                    }
                }
            }
        }
    }
    return 0;
}