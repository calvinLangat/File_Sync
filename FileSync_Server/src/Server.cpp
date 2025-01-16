#include "Utils.h"

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	int iResult;
	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));

	// Initialize winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult)
	{
		printf("WSAStartup failed with error: %d.\n", iResult);
		return -1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	// AI_PASSIVE flag and NULL nodename sets the IP portion to INADDR_ANY
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult)
	{
		printf("getaddrinfo failed: %d.\n", iResult);
		WSACleanup();
		return -1;
	}

	// Create socket
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		printf("Error at socket(): %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	// Binding
	iResult = bind(ListenSocket, result->ai_addr, result->ai_addrlen);
	if (iResult)
	{
		printf("Failed to bind: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return -1;
	}

	// Listening on the socket
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("listen failed with error: %d.\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return -1;
	}



	// If only need to connect to one client, close the listenig socket
	//closesocket(ListenSocket);

	// To receive and send data on a socket
	char recvbuff[DEFAULT_BUFFLEN];
	int iSendResult;
	int recvbufflen = DEFAULT_BUFFLEN;

	// Receive until the peer shuts down the connection
	//do
	//{
	//	iResult = recv(ClientSocket, recvbuff, recvbufflen, 0);

	//	if (iResult > 0)
	//	{
	//		printf("Bytes received: %d.\n", iResult);
	//		if (iResult < recvbufflen)
	//		{
	//			recvbuff[iResult] = '\0';
	//			printf_s("Message: %s\n", recvbuff);
	//		}
	//		// Echo the buffer back to the sender
	//		iSendResult = send(ClientSocket, recvbuff, iResult, 0);
	//		if (iSendResult == SOCKET_ERROR)
	//		{
	//			printf("send failed: %d.\n", WSAGetLastError());
	//			closesocket(ClientSocket);
	//			WSACleanup();
	//			return -1;
	//		}

	//		printf("Bytes sent: %d\n", iSendResult);
	//	}
	//	else if (iResult == 0)
	//	{
	//		printf("Connection closing...\n");
	//	}
	//	else
	//	{
	//		printf("recv failed: %d.\n", WSAGetLastError());
	//		closesocket(ClientSocket);
	//		WSACleanup();
	//		return -1;
	//	}

	//} while (iResult > 0);
	// 
	
	// Accepting a Connection
	SOCKET ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET)
	{
		printf("Failed to accept socket: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return -1;
	}

	while (true)
	{
		iResult = recv(ClientSocket, recvbuff, recvbufflen - 1, 0);

		if (iResult > 0)
		{
			printf("Bytes received: %d.\n", iResult);

			recvbuff[iResult] = '\0';
			printf_s("Message: %s\n", recvbuff);

			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuff, iResult, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				printf("send failed: %d.\n", WSAGetLastError());

				// shutdown the send half of the connection since no more data will be sent
				iResult = shutdown(ClientSocket, SD_SEND);
				if (iResult == SOCKET_ERROR) {
					printf("shutdown failed: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				closesocket(ClientSocket);
				WSACleanup();
				return -1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
		{
			printf("Connection closing...\n");
		}
		else
		{
			printf("recv failed: %d.\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return -1;
		}
	}

	// cleanup
	WSACleanup();

	getchar();
	return 0;
}