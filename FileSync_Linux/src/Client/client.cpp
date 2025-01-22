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

	close(clientfd);
	return 0;
}
