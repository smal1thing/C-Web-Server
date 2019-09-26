#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT "3490"

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(){
    int sockfd;
    struct sockaddr_storage their_addr;
    char s[INET6_ADDRSTRLEN];
    int rv; //return value.
    struct addrinfo hints, *servinfo, *p;
    
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_flags = AI_PASSIVE;
    
    memset(&hints, 0, sizeof(hints));
    rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo);
    
    for(p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        
        //print the ip address
        void *addr;
		char *ipver;
        char ipstr[INET6_ADDRSTRLEN];
		// get the pointer to the address itself,
		// different fields in IPv4 and IPv6:
		if (p->ai_family == AF_INET) { // IPv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		} else { // IPv6
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		// convert the IP to a string and print it:
		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		printf("  %s: %s\n", ipver, ipstr);
		
        if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            perror("connect error: ");
            continue;
        }
        printf("connect success.\n");
        break;
    }
    if(p == NULL) {
        printf("ahhhhh\n");
        return 1;
    }
    printf("???\n");
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s); 
    char* hello = "hello from client";
    send(sockfd, hello, strlen(hello), 0);
    printf("hello message sent\n");
    
    char buf[100];
    int num_byte = recv(sockfd, buf, 100, 0);
    buf[num_byte] = '\0';
    printf("The message received: %s\n", buf);
    close(sockfd);
    return 0;
}

























