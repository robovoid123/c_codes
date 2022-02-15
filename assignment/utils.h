#if !defined(__UTILS__)
#define __UTILS__

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
#include <fcntl.h>
#include <errno.h>

#define BUFFSIZE 1023

typedef struct sockaddr SA;

int Socket(int domain, int type, int protocol);
void Bind(int fd, const struct sockaddr *addr, socklen_t len);
int Fork();
int Accept(int fd, struct sockaddr *addr, socklen_t *addr_len);
void Listen(int fd, int backlog);
ssize_t Read(int clientFD, void *recvBuffer, size_t size);
ssize_t Write(int clientFD, void *data, size_t size);

#endif