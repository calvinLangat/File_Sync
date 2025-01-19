#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <Windows.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_IP	 "127.0.0.1"
#define DEFAULT_BUFFLEN 512

// Thread function
DWORD WINAPI workerthread(void* socketParam);