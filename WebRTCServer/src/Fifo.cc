#include "Fifo.h"

#include <sys/socket.h> // For socket functions
#include <arpa/inet.h>  // For sockaddr_in and inet_pton
#include <unistd.h>     // For close

Fifo::Fifo()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Failed to create socket." << std::endl;
    }

    // 2. Define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8081); // Port 8081

    // Convert IP addresses from text to binary form (127.0.0.1)
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported." << std::endl;
    }

    // 3. Connect to the server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Connection failed." << std::endl;
    }

    std::cout << "Connected to server." << std::endl;
}

Fifo::~Fifo(){
    // 6. Close the socket
    close(sock);
}

std::string Fifo::Read(){
    char buffer[20480] = {0};
    read(sock, buffer, 20480);
    std::string serverResponse(buffer);
    return serverResponse;
}

void Fifo::Write(std::string data){
    send(sock, data.c_str(), data.length(), 0);
}