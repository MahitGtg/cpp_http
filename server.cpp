#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main() {
    // AF_INET = ipv4, SOCK_STREAM: TCP
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET; // ipv4
    serverAddress.sin_port = htons(8080); // htons = host to network short
    serverAddress.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY = any available ip

    // NOTE: must be ::bind (the POSIX socket call). Plain bind() resolves to
    // std::bind because of `using namespace std` and silently does nothing.
    if (::bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(serverSocket, 5) < 0) {
        perror("listen");
        return 1;
    }

    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket < 0) {
        perror("accept");
        return 1;
    }

    char buffer[1024] = {0};
    recv(clientSocket, buffer, sizeof(buffer), 0);
    cout << "Message from client: " << buffer << endl;

    close(clientSocket);
    close(serverSocket);
    return 0;
}
