#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(){

  char domainName[20];
  int counter = 0;

  printf("Enter domain name: ");
  scanf("%s", domainName);

  struct hostent *domain = gethostbyname(domainName);

  struct in_addr **list = (struct in_addr **) domain->h_addr_list;

  for(int i = 0; list[i] != NULL; i++) {
    counter = counter + 1;
    printf("Address %d is : %s\n", counter, inet_ntoa(*list[i]));
  }  

  return 0;
}