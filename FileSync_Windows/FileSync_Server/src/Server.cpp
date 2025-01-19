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

	// Accept sockets and spawn new threads to handle
	while (true)
	{
		// Accepting a Connection
		SOCKET ClientSocket = INVALID_SOCKET;
		ClientSocket = accept(ListenSocket, NULL, NULL);	// This is blocking
		if (ClientSocket == INVALID_SOCKET)
		{
			printf("Failed to accept socket: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return -1;
		}

		// Copying because the ClientSocket is overwritten
		// in the while loop before the new thread can use it.
		SOCKET ThreadSocket = ClientSocket;

		HANDLE hThread = CreateThread(
			NULL,
			8192,					// 8kb stack size
			workerthread,			// Function to be executed
			(void*)&ThreadSocket,	// Parameter to be passed to the func.
			0,
			NULL
		);
		if (hThread == NULL)
		{
			printf("Thread creation failed with error %d\n", GetLastError());
		}
		else
		{
			printf("New Client connected\n");
		}
	}


	// cleanup
	WSACleanup();

	getchar();
	return 0;
}

DWORD WINAPI workerthread(void* socketParam)
{
	// To receive and send data on a socket
	char recvbuff[DEFAULT_BUFFLEN];
	int iSendResult;
	int recvbufflen = DEFAULT_BUFFLEN;
	int iResult;

	// Dereference and copy the socket before socket is overwritten
	SOCKET ClientSocket = *(SOCKET*)socketParam;

	// This while true loop will live in a thread
	while (true)
	{
		iResult = recv(ClientSocket, recvbuff, recvbufflen - 1, 0);	// Leave space for null-termination

		if (iResult > 0)
		{
			printf("Bytes received: %d.\n", iResult);

			recvbuff[iResult] = '\0';
			printf_s("Message: %s\n", recvbuff);

			// If we got thr Upload Command, we start the procedure.
			if (strcmp(recvbuff, "Upload") == 0)
			{
				const char* reply = "ACK Upload";

				// Send the ACK
				iSendResult = send(ClientSocket, reply, strlen(reply), 0);
				if (iSendResult == SOCKET_ERROR)
				{
					printf("send failed: %d.\n", WSAGetLastError());

					// shutdown the send half of the connection since no more data will be sent
					iResult = shutdown(ClientSocket, SD_SEND);
					if (iResult == SOCKET_ERROR) {
						printf("shutdown failed: %d\n", WSAGetLastError());
						closesocket(ClientSocket);
						WSACleanup();
						return -1;
					}
					closesocket(ClientSocket);
					WSACleanup();
					return -1;
				}
				printf("Bytes sent: %d\n", iSendResult);

				// Await the file metadata
				iResult = recv(ClientSocket, recvbuff, recvbufflen - 1, 0);

				std::string metadata = recvbuff;
				std::string fileName;
				std::string fileSize_str;
				int splitPOS = metadata.find('|');

				if (splitPOS == std::string::npos) {
					printf("Invalid metadata format.\n");
					return -1;  // Handle error appropriately
				}

				// Safely extract fileName and fileSize_str
				fileName = metadata.substr(0, splitPOS);
				fileSize_str = metadata.substr(splitPOS + 1);

				// Convert file size to size_t
				size_t fileSize = strtoull(fileSize_str.c_str(), NULL, 10);

				printf("File name: %s\nFile size: %zu bytes\n", fileName.c_str(), fileSize);

				// Create the file in the directory
				size_t receivedBytes = 0;
				char filebuff[4096];
				std::string filePath = "D:\\MEGA\\Documents\\Code\\playground\\" + fileName;
				std::ofstream file(filePath, std::ios::binary);
				if (!file.is_open())
				{
					printf("Failed to create file");
					break;
				}

				// Start receiving the file
				printf("Receiving file...\n");
				while (receivedBytes < fileSize)
				{
					int bytes = recv(ClientSocket, filebuff, sizeof(filebuff), 0);
					file.write(filebuff, bytes);
					receivedBytes += bytes;
				}
				file.close();

				if (receivedBytes != fileSize)
				{
					printf("File size and bytes received mismatch.\n");
				}
				else
				{
					printf("File received successfully.\n");
				}
			}
		}
		else if (iResult == 0)
		{
			printf("Thread %d: Connection closing...\n", GetCurrentThreadId());
			closesocket(ClientSocket);
			break;

		}
		else
		{
			printf("recv failed: %d.\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return -1;
		}
	}

	return 0;
}