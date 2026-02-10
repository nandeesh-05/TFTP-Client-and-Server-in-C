#include "tftp.h"
#include "tftp_client.h"
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
#include <errno.h>
#include <fcntl.h>

int mode=1;
int packno=1;
int main() {
	char command[256];
	tftp_client_t client;
	memset(&client, 0, sizeof(client));  // Initialize client structure

	// Main loop for command-line interface
	while (1) {
		__fpurge(stdin);
		printf("\nMenu \n");
		printf("1.Connect\n2.Put(upload to server)\n3.Get(Download from server)\n4.Mode(Data rate)\n5.Exit\nEnter the operation :");
		fgets(command, sizeof(command), stdin);

		// Remove newline character
		command[strcspn(command, "\n")] = 0;

		// Process the command
		process_command(&client, command);
	}

	return 0;
}

// Function to process commands
void process_command(tftp_client_t *client, char *command) {
	if(strcasecmp(command,"connect")==0)
	{
		char ip[100];
		printf("Enter the IPv4 address :");
		scanf(" %[^\n]",ip);
		if(validateip(ip)==SUCCESS)
		{
			connect_to_server(client,ip,PORT);
			printf("Valid IP address connected to server.\n");
		}
		else
		{

			printf("Connection failed due to an invalid IP address.\n");
		}
	}
	else if(strcasecmp(command,"put")==0)
	{
		char filename[100];
		printf("\nClient files:\n");
		system("ls");   // ✅ CLIENT files make sense here
		printf("Enter file to upload: ");
		scanf(" %[^\n]", filename);
		put_file(client, filename);

	}
	else if(strcasecmp(command,"get")==0)
	{
		char filename[100];
		printf("Enter SERVER file name to download: ");
		scanf(" %[^\n]", filename);
		get_file(client, filename);
	}
	else if(strcasecmp(command,"mode")==0)
	{

		printf("1.Normal mode\n2.Octal mode\n3.Net ASCII\n");
		printf("Enter the mode :");
		scanf("%d",&mode);

	}
	else if(strcasecmp(command,"Exit")==0)
	{
		disconnect(client);
		exit(0);
	}
	else
	{
		printf("Invalid Operation\n");
	}
}

// This function is to initialize socket with given server IP, no packets sent to server in this function
void connect_to_server(tftp_client_t *client, char *ip, int port) {
	// Create UDP socket
	client->sockfd=socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(client->server_ip,ip);
	// Set socket timeout option
	// Set up server address
	client->server_addr.sin_family=AF_INET;
	client->server_addr.sin_port=htons(port);
	client->server_addr.sin_addr.s_addr=inet_addr(ip);
	client->server_len=sizeof(client->server_addr);
	printf("Connecting to server......\n");

}

void put_file(tftp_client_t *client, char *filename) {
	// Send WRQ request and send file
	// check the whether the file present in current working directory
	int fd=open(filename,O_RDONLY);
	if(fd==-1)
	{ 
		printf("The requested file from the client was not found in the current working directory.\n");
		return ;  

	}
	close(fd);
	send_request(client->sockfd,client->server_addr,filename,WRQ);

}

void get_file(tftp_client_t *client, char *filename) {

	send_request(client->sockfd,client->server_addr,filename,RRQ);

}

void disconnect(tftp_client_t *client) {

	close(client->sockfd);

	printf("Disconnected from server\n");
}
void send_request(int sockfd, struct sockaddr_in server_addr, char *filename, int opcode)
{
	packno=1;
	tftp_packet packet;
	//tftp_packet ack,filename;
	packet.opcode=opcode;
	packet.body.request.mode=mode;
	strcpy(packet.body.request.filename,filename);
	// sending the filename by packet to server
	sendto(sockfd,&packet,sizeof(packet),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
	receive_request(sockfd,server_addr,filename,opcode);
}


void receive_request(int sockfd, struct sockaddr_in server_addr, char *filename, int opcode)
{
	tftp_packet packet;

	// declare packet ,memset
	socklen_t len=sizeof(server_addr);
	memset(&packet,0,sizeof(packet));
// based upon the acknowledgement sending or receiveing the data
	recvfrom(sockfd,&packet,sizeof(packet),0,(struct sockaddr *)&server_addr,&len);
	if(packet.opcode==ACK)
	{
		if(packet.body.ack_packet.block_number==ACK_WRG)
		{
			send_file(sockfd, server_addr,len,filename);
		}
		else if(packet.body.ack_packet.block_number==ACK_RRG)
		{
			receive_file(sockfd, server_addr,len,filename);
		}
		else
		{
			return ;
		}
	}
	else if(packet.opcode==ERROR)
	{
		printf("Error : %s\n",packet.body.error_packet.error_msg);
		return;
	}
	else
	{
		printf("Invalid opcode\n");
		return;
	}


	
}

// function def to validate the ipv4 address
int validateip(char *ip)
{
	int i = 0;
	int dotcount = 0;

	if (ip[0] == '.' || ip[0] == '\0')
		return FAILURE;

	while (ip[i])
	{
		if (ip[i] == '.' && ip[i+1]!='.')
		{
			dotcount++;
		}
		else if (!isdigit(ip[i]))
		{
			return FAILURE;
		}
		i++;
	}

	if (dotcount == 3)
	{
		return SUCCESS;
	}

	return FAILURE;
}
