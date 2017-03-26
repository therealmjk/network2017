#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>



int main(){
	int sockfd, clen, clientfd;
	struct sockaddr_in saddr, caddr;
	unsigned short port = 8799;

	sockfd=socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("Error creating socket\n");
	}

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(port);

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

	int fl = fcntl(sockfd, F_GETFL, 0);
	fl |= O_NONBLOCK;
	fcntl(sockfd, F_SETFL, fl);

	if ((bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)) {
		perror("");
		printf("Error binding\n");
	}

	if (listen(sockfd, 5) < 0) {
		printf("Error listening\n");
	}

	clen=sizeof(caddr);

	while (1) {
		char buffer[100];

		clientfd=accept(sockfd, (struct sockaddr *) &caddr, &clen);

		if(clientfd > 0){
			printf("Connected\n");

			int fl = fcntl(clientfd, F_GETFL, 0);
			fl |= O_NONBLOCK;
			fcntl(clientfd, F_SETFL, fl);

			while(1){
				memset(buffer, 0, sizeof(buffer));
				
				if(recv(clientfd, buffer, sizeof(buffer), 0) > 0){
					printf("Receive: %s\n", buffer);
				}
			}
		}

		while(strcmp(buffer, "/dc\n") == 0){
			shutdown(sockfd, SHUT_RDWR);
			close(sockfd);
			printf("Closing...\n");
			break;
		}
	}

	close (sockfd);

	return 0;
}