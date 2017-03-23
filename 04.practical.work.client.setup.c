#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>

int main(){
	struct sockaddr_in saddr;
	struct hostent *h;
	int sockfd;
	unsigned short port = 8789;

	sockfd=socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("Error creating socket\n");
	}

	h = gethostbyname("vp4.sontg.net");
	if (h == NULL) {
		printf("Unknown host\n");
	}

	struct in_addr **list = (struct in_addr **) h->h_addr_list;

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	memcpy((char *) &saddr.sin_addr.s_addr, list[0], h->h_length);
	saddr.sin_port = htons(port);

	if (connect(sockfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
		printf("Cannot connect\n");
	}

	return 0;
}