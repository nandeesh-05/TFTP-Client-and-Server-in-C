#include "tftp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include<stdio_ext.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <ctype.h>

int mode=1;
int packno=1;
void handle_client(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet);
int main() {
	int sockfd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len = sizeof(client_addr);
	tftp_packet packet;


	// Create UDP socket
	sockfd=socket(AF_INET, SOCK_DGRAM, 0);


	// Set socket timeout option
	//TODO Use setsockopt() to set timeout option


	// Set up server address
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(PORT);
	server_addr.sin_addr.s_addr=INADDR_ANY;
	int ret=bind(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr));
	if(ret==-1)
	{
		perror("bind");
		return -1;
	}



	// Bind the socket


	printf("TFTP Server listening on port %d...\n", PORT);

	// Main loop to handle incoming requests
	while (1) {
		int n = recvfrom(sockfd, &packet, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
		if (n < 0) {
			perror("Receive failed or timeout occurred");
			continue;
		}

		handle_client(sockfd, client_addr, client_len, &packet);
	}

	close(sockfd);
	return 0;
}

void handle_client(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet) 
{
	// Extract the TFTP operation (read or write) from the received packet
	packno=1;
	mode=packet->body.request.mode;
	if (packet->opcode == WRQ)
	{
		//printf("%s\n",packet->body.request.filename);
		receive_file(sockfd, client_addr, client_len,packet->body.request.filename);

	}
	else if (packet->opcode == RRQ)
	{
		send_file(sockfd, client_addr, client_len,packet->body.request.filename);
	}

}


