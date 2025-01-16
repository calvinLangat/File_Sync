#include "Utils.h"

size_t sendFile(SOCKET ConnectSocket, const std::string& filePath)
{
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
	size_t iSendResult = send(ConnectSocket, metadata.c_str(), metadata.size(), 0);

	return iSendResult;
}