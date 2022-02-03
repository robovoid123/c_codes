#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include<string.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<signal.h>
#include<fcntl.h>

int quit = 0;

void myhandler(int code, siginfo_t * info, void * uap){
	printf("Quiting...\n");
	quit = 1;
}

int main() {

	int sockfd;
	struct sockaddr_in server;
	char out[1000];
	size_t total = 0;
	ssize_t current;

	struct sigaction mysigaction;
	memset(&mysigaction, 0 , sizeof(struct sigaction));
	mysigaction.sa_flags = SA_SIGINFO;
	mysigaction.__sigaction_u.__sa_sigaction = &myhandler;
	sigaction(SIGINT, &mysigaction, NULL);

	memset(&server,0,sizeof(struct sockaddr_in));
	server.sin_port = htons(8080);
	server.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1",&server.sin_addr.s_addr);

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		perror("socket ");
		exit(0);
	}

	


	if(connect(sockfd,(struct sockaddr *)&server,sizeof(struct sockaddr_in)) < 0) {
		perror("bind");
		exit(0);
	}

	// set socket in non-blocking mode
	int flags;
	flags = fcntl(sockfd,F_GETFL);
	flags = flags|O_NONBLOCK;
	printf("\n%d\n",O_NONBLOCK);
	fcntl(sockfd,F_SETFL,flags);


	for (; ;) {
		if( (current = write(sockfd,out,1000)) < 0) {
			perror("write ");
			quit = 1;
			current = 0;
		}
		total+=current;
		printf("%zu\n",total);
		if(quit)
			break;
	}

	close(sockfd);
	return 0;
}