/* Common file for server & client */

#include "tftp.h"
extern int mode;
extern int packno;
void send_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename) 
{
	// sending the file from server side and to client
	tftp_packet packet;
	int fd=open(filename,O_RDONLY);
	if(fd>0)
	{
		// fill and ack rrq
		packet.opcode=ACK;
		packet.body.ack_packet.block_number=ACK_RRG;
		sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&client_addr, client_len);
		if(mode==1)
		{
			ssize_t nbytes;

			do {
				memset (&packet, 0, sizeof (packet));
				nbytes = read(fd,packet.body.data_packet.data,512);
				packet.opcode = DATA;
				packet.body.data_packet.block_number = packno;
				// send the data to server
				sendto(sockfd, &packet,sizeof(packet), 0,(struct sockaddr *)&client_addr, client_len);
				// receive the ack from client
				printf("Sending %ld bytes with packet number :%d\n", nbytes, packet.body.data_packet.block_number);
				recvfrom(sockfd,&packet,sizeof(packet), 0, (struct sockaddr *)&client_addr,&client_len);

				if (packet.opcode == ACK&&
						packet.body.ack_packet.block_number == packno) {
					packno++;
				}

			} while (nbytes == 512);
			close(fd);
			printf("File '%s' copied to Client successfully.\n", filename);
		}
		if (mode == 3)
		{
			ssize_t nbytes;
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

				printf("Sending %ld bytes with packet number : %d\n",
						nbytes, packet.body.data_packet.block_number);

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

		if(mode==2)
		{
			ssize_t nbytes;

			do {
				memset (&packet, 0, sizeof (packet));
				nbytes = read(fd,packet.body.data_packet.data,1);
				packet.opcode = DATA;
				packet.body.data_packet.block_number = packno;
				// send the data to server
				sendto(sockfd, &packet,sizeof(packet), 0,(struct sockaddr *)&client_addr, client_len);
				printf("Sending %ld bytes with packet number :%d\n", nbytes, packet.body.data_packet.block_number);
				// receive the ack from client
				recvfrom(sockfd,&packet,sizeof(packet), 0, (struct sockaddr *)&client_addr,&client_len);

				if (packet.opcode == ACK&&
						packet.body.ack_packet.block_number == packno) {
					packno++;
				}

			} while (nbytes == 1);
			close(fd);
			printf("File '%s' copied to Client successfully.\n", filename);
		}
	}
	else
	{
		// fill and error // send to
		packet.opcode=ERROR;
		strcpy(packet.body.error_packet.error_msg,"Failed to open the file in server side");
		sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&client_addr, client_len);
		printf("File transfer from server to client failed.\n");
		close(fd);
	}

}

void receive_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename) 
{
	// uploading the file client to server side
	tftp_packet packet;
	int fd=open(filename,O_WRONLY|O_EXCL);

	if(fd==-1)
	{
		if(errno==ENOENT)
		{
			printf("File '%s' not found in Server CWD. Created successfully.\n", filename);
			fd=open(filename,O_CREAT|O_WRONLY,0644);
		}
	}
	else
	{
		printf("INFO: File '%s' exists in Server CWD. File contents cleared.\n", filename);
		fd=open(filename,O_WRONLY|O_TRUNC);
	}
	// send the acknowledgement to client
	if(fd>0)
	{
		packet.opcode=ACK;
		packet.body.ack_packet.block_number=ACK_WRG;
		sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&client_addr, client_len);
		if(mode == 1 || mode == 3)
		{
			ssize_t nbytes;
			do {
				memset (&packet, 0, sizeof (packet));
				recvfrom(sockfd, &packet, sizeof(packet), 0,(struct sockaddr *)&client_addr,&client_len);

				if (packet.opcode == DATA &&
						packet.body.data_packet.block_number == packno) {

					nbytes =write(fd,packet.body.data_packet.data,strlen(packet.body.data_packet.data));
					printf("Currently receiving %ld bytes of data, packet number %d\n", nbytes, packet.body.data_packet.block_number);


					packet.opcode = ACK;
					packet.body.ack_packet.block_number = packno;

					sendto(sockfd,&packet,sizeof(packet),0,(struct sockaddr *)&client_addr,client_len);

					packno++;
				}

			} while (nbytes == 512);
		}
		if(mode ==2)
		{
			ssize_t nbytes;
			do {
				memset (&packet, 0, sizeof (packet));
				recvfrom(sockfd, &packet, sizeof(packet), 0,(struct sockaddr *)&client_addr,&client_len);

				if (packet.opcode == DATA &&
						packet.body.data_packet.block_number == packno) {

					nbytes=write(fd,packet.body.data_packet.data,strlen(packet.body.data_packet.data));
					printf("Currently receiving %ld bytes of data, packet number %d\n", nbytes, packet.body.data_packet.block_number);

					packet.opcode = ACK;
					packet.body.ack_packet.block_number = packno;

					sendto(sockfd,&packet,sizeof(packet),0,(struct sockaddr *)&client_addr,client_len);

					packno++;
				}

			} while (nbytes == 1);
		}
		close(fd);

	}
	// send the error to client
	else
	{
		packet.opcode=ERROR;
		strcpy(packet.body.error_packet.error_msg,"Failed to open the file in server side");
		sendto(sockfd, &packet, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, client_len);

	}

	close(fd);
	printf("File successfully received from client to server.\n");
}
