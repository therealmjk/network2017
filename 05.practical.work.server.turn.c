#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>


int main(){
	int sockfd, clen, clientfd;
	struct sockaddr_in saddr, caddr;
	unsigned short port = 8789;

	sockfd=socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("Error creating socket\n");
	}

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(port);

	if ((bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)) {
		perror("");
		printf("Error binding\n");
	}

	if (listen(sockfd, 5) < 0) {
		printf("Error listening\n");
	}

	clen=sizeof(caddr);
	if ((clientfd=accept(sockfd, (struct sockaddr *) &caddr, &clen)) < 0) {
		printf("Error accepting connection\n");
	}

	printf("Connected\n");

	while (1) {
		char buffer[100];
		recv(clientfd, buffer, sizeof(buffer), 0);
		printf("Receive: %s\n", buffer);
		printf("Send: ");
		scanf("%s\n", buffer);
		send(clientfd, buffer, strlen(buffer), 0);
	}

	close (sockfd);

	return 0;
}