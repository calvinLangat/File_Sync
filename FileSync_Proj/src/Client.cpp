#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_IP	 "127.0.0.1"
#define DEFAULT_BUFFLEN 512

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
					*ptr	= NULL,
					hints	= {};

	const char* sendBuff = "this is a test.";
	char recvbuf[DEFAULT_BUFFLEN];
	int iResult;
	int recvbufflen = DEFAULT_BUFFLEN;

	// initialize winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return -1;
	}

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//Resolve the server address and port
	iResult = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);
	if (iResult)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return -1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr->ai_next)
	{
		// Create a socket for connecting to a server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("failed to get a valid socket.\n");
			WSACleanup();
			return -1;
		}

		// Connect to server
		iResult = connect(ConnectSocket, ptr->ai_addr, ptr->ai_addrlen);
		if(iResult == SOCKET_ERROR)
		{
			printf("Error connecting to server. Trying again.... Error: %d.\n", WSAGetLastError());
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server.\n");
		WSACleanup();
		return -1;
	}

	// Send the initial buffer
	iResult = send(ConnectSocket, sendBuff, strlen(sendBuff), 0);
	if (iResult == SOCKET_ERROR)
	{
		printf("Error sending buffer. Error: %d.\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}

	printf("Bytes sent: %d.\n", iResult);

	// Shutdown the connection since no more data woll be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if(iResult == SOCKET_ERROR)
	{
		printf("Error shutting down. Error: %d.\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}

	// Receive until the peer closes the connection
	do
	{
		iResult = recv(ConnectSocket, recvbuf, recvbufflen, 0);
		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);
		}
		else if (iResult == 0)
		{
			printf("Connection closed\n");
		}
		else
		{
			printf("Receive failed with error: %d\n", WSAGetLastError());
		}
	} while (iResult > 0);

	//Clean up
	closesocket(ConnectSocket);
	WSACleanup();

	getchar();
	return 0;
}