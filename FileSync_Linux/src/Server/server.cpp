#include "utils.h"


int main(int argc, char* argv[])
{
    int opt = 1;
    int conn_fd;
    struct sockaddr_in address;
    const int bufferLen = 1024;
    char buffer[bufferLen];
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

    conn_fd = accept(listenSocket, NULL, NULL);
    if(conn_fd == -1)
    {
        perror("Error accepting connection");
        return -1;
    }

    size_t recv_bytes = recv(conn_fd, &buffer, bufferLen-1, 0); // Len-1 to allow null-termination
    
    if(recv_bytes > 0)
    {
        buffer[recv_bytes] = '\0';  // null-terminate
        printf("message received: %s\n", buffer);
    }
    else if(recv_bytes == 0)
    {
        printf("Connection ended by client.\n");
    }
    else if(recv_bytes == -1)
    {
        perror("Error occured on recv");
    }
    return 0;
}
