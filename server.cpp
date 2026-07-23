#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <tuple>
#include <unistd.h>
#include <sstream>

using namespace std;

struct Response {
    string body;
    string status;
    int code;
};

struct Socket{
    int fd;

    Socket(int f) : fd(f) {}

    ~Socket() {
        if (fd >= 0) close(fd);
    }
};

string parse_path(const string& request) {
    istringstream iss(request);
    string method, path, version;
    iss >> method >> path >> version;
    return path;
}

Response route_request(const string& path) {
    string body;
    string status = "OK";
    int code = 200;

    if (path == "/") {
        body = "<h1>Home</h1>";
    } else if (path == "/hello") {
        body = "<h1>Hello there</h1>";
    } else {
        body = "<h1>Not found</h1>";
        code = 404;
        status = "Not Found";
    }

    return Response{body, status, code};
}

int main() {
    // AF_INET = ipv4, SOCK_STREAM: TCP
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    Socket server(serverSocket);

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

    // client connection
    while (true) {
        int fd = accept(serverSocket, nullptr, nullptr);
        Socket client(fd);
        if (client.fd < 0) {
            perror("accept");
            continue;
        }
        

        char buffer[1024] = {0};
        // ssize_t = signed size_t, since recv returns >0, 0 or -1
        ssize_t bytesRead = recv(client.fd, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            continue;
        }
        string path = parse_path(buffer);
        auto r = route_request(path);

        string response =
                    "HTTP/1.1 " + to_string(r.code) + " " + r.status + "\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + to_string(r.body.size()) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n" +
                    r.body;

        send(client.fd, response.c_str(), response.size(), 0);

    }

    // close(serverSocket);
    return 0;
}
