#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int connect_to_server(bool send_request) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);

    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));

    std::cout << "Connected to server." << std::endl;

    if (send_request) {
        // sleep(1);
        const char *request = "test request";
        ssize_t sent = send(sock, request, strlen(request), 0);
    }
    else
    {
        sleep(3);
    }
    // close(sock);
    return 0;
}

int main() {
    // First connection: send request
    connect_to_server(true);

    // Second connection: just connect, no send
    connect_to_server(false);

    return 0;
}

