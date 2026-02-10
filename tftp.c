/* Common file for server & client */

#include "tftp.h"
extern int mode;
extern int packno;
void send_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename) 
{
	// send the packet data from client side to server side
	tftp_packet packet;
	int fd=open(filename,O_RDONLY);
	char buff[512];

	ssize_t nbytes;
	if(mode==1)
	{ 
		do {

			memset (&packet, 0, sizeof (packet));
			nbytes = read(fd,packet.body.data_packet.data,512);
			packet.opcode = DATA;
			packet.body.data_packet.block_number = packno;
			// send the data to server
			sendto(sockfd, &packet,sizeof(packet), 0,(struct sockaddr *)&client_addr, client_len);
			printf("Sending %ld bytes with packet number :%d\n", nbytes, packet.body.data_packet.block_number);
			// receive the ack from client
			recvfrom(sockfd,&packet,sizeof(packet), 0, (struct sockaddr *)&client_addr,&client_len);

			if (packet.opcode == ACK && packet.body.ack_packet.block_number == packno) 
			{
				packno++;
			}

		} while (nbytes == 512);
	}
	else if(mode==2)
	{ 
		do {

			memset (&packet, 0, sizeof (packet));
			nbytes = read(fd,packet.body.data_packet.data,1);
			packet.opcode = DATA;
			packet.body.data_packet.block_number = packno;
			// send the data to server
			sendto(sockfd, &packet,sizeof(packet), 0,(struct sockaddr *)&client_addr, client_len);
			printf("Sending %ld bytes with packet number : %d\n", nbytes, packet.body.data_packet.block_number);
			// receive the ack from client
			recvfrom(sockfd,&packet,sizeof(packet), 0, (struct sockaddr *)&client_addr,&client_len);

			if (packet.opcode == ACK&&
					packet.body.ack_packet.block_number == packno) {
				packno++;
			}

		} while (nbytes == 1);
	}
	else if (mode == 3)
	{
		do {
			char local_buff[512];
			int idx = 0;
			char ch;

			memset(&packet, 0, sizeof(packet));
			memset(local_buff, 0, sizeof(local_buff));

		
			while (idx < 512 && read(fd, &ch, 1) > 0)
			{
				if (ch == '\n')
				{
					local_buff[idx++] = '\r';
				}
				local_buff[idx++] = ch;
			}

			memcpy(packet.body.data_packet.data, local_buff, idx);
			nbytes = idx;
			

			packet.opcode = DATA;
			packet.body.data_packet.block_number = packno;

			/* send data to client */
			sendto(sockfd, &packet, sizeof(packet), 0,
					(struct sockaddr *)&client_addr, client_len);

			printf("Sending %ld bytes with packet number : %d\n",nbytes, packet.body.data_packet.block_number);

			/* receive ACK */
			recvfrom(sockfd, &packet, sizeof(packet), 0,
					(struct sockaddr *)&client_addr, &client_len);

			if (packet.opcode == ACK &&
					packet.body.ack_packet.block_number == packno)
			{
				packno++;
			}

		} while (nbytes == 512);
	}

	close(fd);
	printf("File '%s' copied to server successfully.\n", filename);

}

void receive_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename) 
{
	// in get operation downloading the file from server
    // geting packet from server to client
	tftp_packet packet;
	int fd=open(filename,O_WRONLY|O_EXCL);

	if(fd==-1)
	{
		if(errno==ENOENT)
		{
			printf("File '%s' not found in Client CWD. Created successfully.\n", filename);
			fd=open(filename,O_CREAT|O_WRONLY,0644);
		}
	}
	else
	{
		printf("INFO: File '%s' exists in Client CWD. File contents cleared.\n", filename);
		fd=open(filename,O_WRONLY|O_TRUNC);
	}
	// open the file receiving the packet
	if(mode==1 || mode==3)
	{
		ssize_t nbytes;
		do {
			memset (&packet, 0, sizeof (packet));
			recvfrom(sockfd, &packet, sizeof(packet), 0,(struct sockaddr *)&client_addr,&client_len);

			if (packet.opcode == DATA &&
					packet.body.data_packet.block_number == packno) {

				nbytes = write(fd,packet.body.data_packet.data,strlen(packet.body.data_packet.data));
				printf("Currently receiving %ld bytes of data, packet number %d\n", nbytes, packet.body.data_packet.block_number);


				packet.opcode = ACK;
				packet.body.ack_packet.block_number = packno;

				sendto(sockfd,&packet,sizeof(packet),0,(struct sockaddr *)&client_addr,client_len);

				packno++;
			}

		} while (nbytes == 512);
		close(fd);
		printf("File downloaded successfully from the server.\n");
	}
	if(mode==2)
	{
		ssize_t nbytes;
		do {
			memset (&packet, 0, sizeof (packet));
			recvfrom(sockfd, &packet, sizeof(packet), 0,(struct sockaddr *)&client_addr,&client_len);

			if (packet.opcode == DATA &&
					packet.body.data_packet.block_number == packno) {

				nbytes = write(fd,packet.body.data_packet.data,strlen(packet.body.data_packet.data));
				printf("Currently receiving %ld bytes of data, packet number : %d\n", nbytes, packet.body.data_packet.block_number);

				packet.opcode = ACK;
				packet.body.ack_packet.block_number = packno;

				sendto(sockfd,&packet,sizeof(packet),0,(struct sockaddr *)&client_addr,client_len);

				packno++;
			}

		} while (nbytes == 1);
		close(fd);
		printf("File downloaded successfully from the server.\n");
	}
}
