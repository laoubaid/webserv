
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

int main()
{
    sockaddr_in serv_addr;
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = 8080;

    if (connect(client_fd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Connection failed");
        close(client_fd);
        return -1;
    }
    std::cout << "Connection succeeded!" << std::endl;
    return 0;
}

// #include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <cstring>
// #include <arpa/inet.h>

// int main(int argc, char *argv[]) {

//     if (argc != 2) {
//         std::cerr << "Usage: " << argv[0] << " <name>" << std::endl;
//         return -1;
//     }
    
//     std::string name = argv[1];
//     int client_fd;
//     sockaddr_in serv_addr;
//     char message[1024] = {0};

//     // Create socket
//     client_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (client_fd == -1) {
//         std::cerr << "Socket creation failed" << std::endl;
//         return -1;
//     }

//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_port = htons(8080);

//     // Convert IPv4 and IPv6 addresses from text to binary form
//     if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
//         std::cerr << "Invalid address/ Address not supported" << std::endl;
//         close(client_fd);
//         return -1;
//     }

//     // Connect to server
//     if (connect(client_fd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
//         std::cerr << "Connection failed" << std::endl;
//         close(client_fd);
//         return -1;
//     }

//     std::cout << "Connected to server. Enter messages (type 'exit' to quit):" << std::endl;
//     std::string msg;
//     while (true) {
//         std::cout << "> "; // Prompt for input
//         std::cin.getline(message, 1024);
//         msg = name + " : " + std::string(message);
//         if (strcmp(message, "exit") == 0) {
//             break; // Exit the loop if the user types "exit"
//         }

//         // Send message to server
//         send(client_fd, msg.c_str(), msg.length(), 0);

//         std::cout << "Message sent." << std::endl;
//     }

//     close(client_fd);
//     return 0;
// }