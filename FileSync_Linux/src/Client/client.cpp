#include "client.h"

int main(int argc, char* argv[])
{
	int status, clientfd, result;
	struct sockaddr_in servr_address;

	clientfd = socket(AF_INET, SOCK_STREAM, 0);
	if(clientfd < 0)
	{
		printf("Socket creation error\n");
		return -1;
	}

	servr_address.sin_family = AF_INET;
	servr_address.sin_port = htons(PORT);

	// Convert IPV4 to binary
	result = inet_pton(AF_INET, "127.0.0.1", &servr_address.sin_addr);
	
	if(result != 1) // I have done it this way so that it only checks one thing if successful.
	{
		if (result == 0)
		{
			printf("ip address not valid\n");
			return -1;
		}
		else if(result == -1)
		{
			perror("ip address not supported");
			return -1;
		}
	}

	status = connect(clientfd, (sockaddr * )&servr_address, sizeof(servr_address));
	if(status == -1)
	{
		perror("Could not connect");
		return -1;
	}

	const char* msg = "Hello";
	size_t sent_bytes = send(clientfd, msg, strlen(msg), 0);
	if(sent_bytes > 0)
	{
		printf("msg sent successfully.\n");
	}
	else if(sent_bytes == 0)
	{
		printf("Error: No bytes sent.\n");
	}
	else if(sent_bytes == -1)
	{
		perror("Error on sending message");
	}


	close(clientfd);
	return 0;
}
