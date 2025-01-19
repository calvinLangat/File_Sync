#include "Utils.h"


int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
					*ptr	= NULL,
					hints;

	ZeroMemory(&hints, sizeof(hints));
	
	
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


	// Start Communtication by sending the Upload command
	size_t SendResult;
	const char* msg = "Upload";
	SendResult = send(ConnectSocket, msg, strlen(msg), 0);
	if (SendResult == SOCKET_ERROR)
	{
		printf("send failed: %d.\n", WSAGetLastError());

		// shutdown the send half of the connection since no more data will be sent
		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}

	// Prepare the contents of the file
	std::string filePath = argv[0];
	std::string fileName = std::filesystem::path(filePath).filename().string();
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		printf("Failed to oopen file %s\n", fileName.c_str());
		return -1;
	}

	// Since we passed in the std::ios::ate flag,
	// we are at the end of the file
	size_t fileSize = file.tellg();
	file.seekg(0);				// Return to the start of the file

	// Send the file name and size
	std::string metadata = fileName + "|" + std::to_string(fileSize);


	// Await confirmation from Server
	size_t recvResult = recv(ConnectSocket, recvbuf, sizeof(recvbuf) - 1, 0);
	recvbuf[recvResult] = '\0';

	const char* serverReply = "ACK Upload";
	if (strcmp(recvbuf, serverReply) == 0)
	{
		printf("Server Upload ACK recvd\n");
		printf("Sending file metadata\n");

		// Send metadata if server ACK
		SendResult = send(ConnectSocket, metadata.c_str(), metadata.size(), 0);
		if (SendResult == SOCKET_ERROR) {
			printf("shutdown failed: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}

		// Start streaming bytes with 4KB chunks
		const int optimalSendBytes = 4096;
		char buff[optimalSendBytes];	//4KB chunks
		if (file.is_open())
		{
			size_t sentFileBytes = 0;
			printf("Starting file transmit...\n");
			while (file)
			{
				file.read(buff, sizeof(buff));
				int bytesRead = (int)file.gcount();
				sentFileBytes += send(ConnectSocket, buff, bytesRead, 0);
			}

			// Confirm we sent the right amount of bytes
			if (sentFileBytes != fileSize)
			{
				printf("Mismatch in bytes sent");
			}
			else
			{
				printf("Transmit finished.\n");
			}
			file.close();
		}
	}

	//Clean up
	closesocket(ConnectSocket);
	WSACleanup();

	getchar();
	return 0;
}