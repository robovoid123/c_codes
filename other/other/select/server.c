#include "server_store_ll.h"
#include "utils.h"

#define LISTENQ 10
#define BUFFSIZE 1024
#define MAXLINE 4096

int main(int argc, char const *argv[])
{
    LinkedList ll;
    get_student_ll(&ll);

    ServLL sll = (ServLL)malloc(sizeof(ServLL));
    ServerData currentServData = (ServerData)malloc(sizeof(ServerData));

    int listenFD, newFD;
    fd_set masterFDs;
    fd_set readFDs;
    int fdMax;

    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen;
    int nBytes;
    int port;

    char clientAddrBuffer[MAXLINE], recvBuffer[MAXLINE + 1];

    int yes = 1;
    int i;

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

    setsockopt(listenFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); // to reuse same port

    Bind(listenFD, (SA *)&serverAddr, sizeof(serverAddr));

    int flags;
    flags = fcntl(listenFD, F_GETFL, 0);
    fcntl(listenFD, F_SETFL, flags | O_NONBLOCK);

    Listen(listenFD, LISTENQ);
    FD_SET(listenFD, &masterFDs);
    fdMax = listenFD;

    printf("Server is listining at port %d\n", port);

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
                    if ((nBytes = Read(i, recvBuffer, MAXLINE + 1)) == 0)
                    {
                        printf("client on fd %d hung up\n", i);
                        close(i);
                        FD_CLR(i, &masterFDs);
                    }
                    else
                    {
                        recvBuffer[nBytes] = 0;

                        /**
                         *  1. first see if it is a new client (we create a list with fd mapped to clientId)
                         *  2. if new client then add it to the client fd map else see if it is the first time client is requesting
                         *  (for each client we will have a requestCount & we cache client info into into a map with clientId) &
                         *  set send_data_flag to true
                         *
                         *  map looks like this:
                         *  [fd : {
                         *          clientId: <clientId>,
                         *          requestCount: <requestCount>,
                         *          data: studentInfo
                         *          send_data_flag
                         *          thank_you_flag
                         *  }]
                         *
                         *  3. if its not the 1st request then wait for 'thank_you' from the client and if received then set tq_flag to true
                         *      3.1. if tq_flag is set & 'again' received then set send_data_flag to true
                         *  4. if send_data flag is set then send data of current client & set send_data_flag & tq_flag to false
                         *
                         */

                        // first request search for id in linked list

                        if (ss_search(&sll, i, &currentServData) < 0)
                        {
                            ServerData newServData = (ServerData)malloc(sizeof(ServerData));
                            newServData->fd = i;
                            newServData->data = (StudentDetail *)malloc(sizeof(StudentDetail));
                            newServData->clientId = atoi(recvBuffer);
                            newServData->send_data_flag = 1;
                            newServData->thank_you_flag = 0;

                            if (search(&ll, newServData->clientId, &(newServData->data)))
                            {
                                printf("student detail not found closing connection\n");
                                close(i);
                                FD_CLR(i, &masterFDs);
                            }

                            // add new student to sll
                            ss_push(&sll, newServData);
                            currentServData = newServData;
                        }

                        if (currentServData->send_data_flag == 1)
                        {
                            // send data to the client (StudentDetail)
                            nBytes = Write(i, currentServData->data, sizeof(StudentDetail));

                            currentServData->send_data_flag = 0;
                            currentServData->requestCount++;

                            print_student(currentServData->data);
                            printf("client request count: %d\n\n", currentServData->requestCount);
                        }

                        if (strncmp(recvBuffer, "thank_you", 9) == 0)
                        {
                            currentServData->thank_you_flag = 1;

                            // send ack to thank_you
                            nBytes = Write(i, "ackTy", 5);
                        }

                        if (currentServData->thank_you_flag == 1 && strncmp(recvBuffer, "again", 5) == 0)
                        {
                            currentServData->send_data_flag = 1;
                            currentServData->thank_you_flag = 0;
                        }
                    }
                }
            }
        }
    }

    free_ll(&ll); // deallocate memory
    return 0;
}
