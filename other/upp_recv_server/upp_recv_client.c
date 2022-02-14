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
#define MAXLINE 1024
#define LISTENQ 10
typedef struct sockaddr SA;

struct send_buff{
    char messg[MAXLINE];
    int fd;
    int size;
};
typedef struct send_buff Buffer;


int main(int argc, char **argv)
{
    int sockfd, n;
    char recvline[MAXLINE+1];
    char sendline[MAXLINE+1];
    struct sockaddr_in servaddr;
    int port;

    if (argc != 3){
        printf("usage: client <IPaddress> <port>");
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        printf("socket error");

    port = atoi(argv[2]);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
        printf("inet_pton error for %s", argv[1]);

    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
        printf("connection error");


    memset(sendline, 0, sizeof(sendline));
    printf("enter a line:\t");
    fgets(sendline, MAXLINE, stdin);
    write(sockfd, sendline, strlen(sendline));

    Buffer sb;
    bzero(&sb, sizeof(sb));

    if (read(sockfd, &sb, sizeof(sb)) == 0){
        printf("error");
        exit(0);
    }
    printf("Received from server:\n line: %s fd: %d size: %d",
        sb.messg, sb.fd, sb.size);
    memset(recvline, 0, sizeof(recvline));

    exit(0);
}
