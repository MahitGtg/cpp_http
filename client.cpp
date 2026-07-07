#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>


using namespace std;

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    // a client connects to a SPECIFIC address; 127.0.0.1 = this machine (loopback)
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("connect");
        return 1;
    }

    const char* message = "Hello, server!";
    send(clientSocket, message, strlen(message), 0);

    close(clientSocket);
    return 0;
}
