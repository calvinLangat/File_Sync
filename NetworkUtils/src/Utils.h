#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <filesystem>
#include <fstream>
#include <string>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_IP	 "127.0.0.1"
#define DEFAULT_BUFFLEN 512

int sendFile(SOCKET ConnectSocket, const std::string& filePath);