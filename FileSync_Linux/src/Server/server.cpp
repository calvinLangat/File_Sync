#include "utils.h"


int main(int argc, char* argv[])
{
    int opt = 1;
    struct sockaddr_in address;

    // Create socket
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(listenSocket == -1)
    {
        perror("Could not create socket");
        return -1;
    }

    // Forcefully attaching socket to port
    if(setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt error");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding socket
    if(bind(listenSocket, (sockaddr* )&address, sizeof(address)) == -1)
    {
        perror("Failed to bind");
        return -1;
    }

    // Listening
    if(listen(listenSocket, 5) == -1) //Accept a maximum queue of 5
    {
        perror("Failed to listen");
        return -1;
    }

    return 0;
}
