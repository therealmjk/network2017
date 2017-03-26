#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>



int main(){
	int sockfd, clen, clientfd;
	struct sockaddr_in saddr, caddr;
	unsigned short port = 8729;

	int clientfds[100]; // list of connected clients, >0 if valid
	memset(clientfds, 0, sizeof(clientfds)); // clear the list

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

		fd_set set; // declaration of the set
		FD_ZERO(&set); // clear the set
		FD_SET(sockfd, &set); // add listening sockfd to set
		int maxfd = sockfd; // a required value to pass to select()
		for (int i = 0; i < 100; i++) {
			// add connected client sockets to set
			if (clientfds[i] > 0) FD_SET(clientfds[i], &set);
			if (clientfds[i] > maxfd) maxfd = clientfds[i];
		}
		// poll and wait, blocked indefinitely
		select(maxfd+1, &set, NULL, NULL, NULL);

		// a «listening» socket?
		if (FD_ISSET(sockfd, &set)) {
			clientfd = accept(sockfd, (struct sockaddr *) &saddr, &clen);
			if(clientfd > 0){
				printf("Connected\n");

				// make it nonblocking
				fl = fcntl(clientfd, F_GETFL, 0);
				fl |= O_NONBLOCK;
				fcntl(clientfd, F_SETFL, fl);
				// add it to the clientfds array
				for (int i = 0; i < maxfd; i++) {
					if (clientfds[i] == 0) {
						clientfds[i] = clientfd;
						break;
					}
				}
			}
		}

		// is that data from a previously-connect client?
		for (int i = 0; i < maxfd; i++) {
			if (clientfds[i] > 0 && FD_ISSET(clientfds[i], &set)) {
				memset(buffer, 0, sizeof(buffer)); //clear the buffer

				if (recv(clientfd, buffer, sizeof(buffer), 0) > 0) {
					printf("client %d says: %s\n", clientfds[i], buffer);
				}
				else {
					// some error. remove it from the "active" fd array
					printf("client %d has disconnected.\n", clientfds[i]);
					clientfds[i] = 0;
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