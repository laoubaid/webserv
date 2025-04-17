
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>

/// error handling 
#define socket_related_err(msg, fd) do { perror(msg); exit(1); close(fd);} while (0)

int main()
{
	int		 server_fd, client_fd;
	sockaddr_in	addr;
	socklen_t	slen;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
		socket_related_err("socket creation failed!", server_fd);
	
	//// instraction //// the SO_REUSEADDR flag use in order to avoid socket being in TIME_WAIT
	int reuse = 1;
	int result = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse));
	if ( result < 0 ) {
		perror("ERROR SO_REUSEADDR:");
	}

	memset(&addr, 0, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(8080);

	if (bind(server_fd, (const sockaddr *)&addr, sizeof(addr)))
		socket_related_err("bind process failed!", server_fd);
	
	if (listen(server_fd, 1))
		socket_related_err("listen process failed!", server_fd);



	// socklen_t addrlen = sizeof(addr);
	char buf[1024] = {0};
	while (1) {
		// client_fd = accept(server_fd, (sockaddr*)&addr, &addrlen);
		client_fd = accept(server_fd, NULL, NULL);
		if (client_fd == -1)
			socket_related_err("accept function failed!", server_fd);
		std::cout << "connection accepted!" << std::endl;
		recv(client_fd, buf, 1023, 0);
		std::cout << "Received:\n" << buf << "\n-------------" << std::endl;
		close(client_fd);
	}
	close(server_fd);

    return 0;
}


// #include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <cstring>

// #include <fcntl.h>

// int error_handler(const char *msg, int fd) {
//     perror(msg);
//     if (fd != -1) {
//         close(fd);
//     }
//     return -1;
// }

// int main() {
//     int server_fd;
//     int new_socket;
//     sockaddr_in serv_addr;

//     server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd == -1) {
//         std::cerr << "Socket creation failed" << std::endl;
//         return -1;
//     }
//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_addr.s_addr = INADDR_ANY;
//     serv_addr.sin_port = htons(8080);

// //////////////////////////////////////////////////////// Set socket to non-blocking mode
    
//     int flags = fcntl(server_fd, F_GETFL, 0); // Get current flags
//     if (flags == -1)
//         return (error_handler("fcntl F_GETFL failed!", server_fd)); 

//     flags |= O_NONBLOCK; // Add non-blocking flag

//     if (fcntl(server_fd, F_SETFL, flags) == -1)
//         return (error_handler("fcntl F_SETFL failed!", server_fd)); // Set new flags
//     std::cout << "DEGUB: Socket set to non-blocking mode" << std::endl;

// ////////////////////////////////////////////////////////

//     if (bind(server_fd, (const sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
//         std::cerr << "Bind failed" << std::endl;
//         close(server_fd);
//         return -1;
//     }

//     if (listen(server_fd, 1) == -1) {
//         std::cerr << "Listen failed" << std::endl;
//         close(server_fd);
//         return -1;
//     }

//     std::cout << "DEGUB: Server is listening on port 8080..." << std::endl;
//     // Accept a connection
//     std::cout << "DEGUB: Waiting for a connection..." << std::endl;
//     while (true) {
//         // Accept a connection
//         new_socket = accept(server_fd, NULL, NULL);
//         // Check if accept was successful
//         if (new_socket == -1) {
//             if (errno == EAGAIN || errno == EWOULDBLOCK) {
//                 // No connections available, continue waiting
//                 // std::cout << "DEGUB: No connections available, continuing to wait..." << std::endl;
//                 continue;
//             } else {
//                 return error_handler("Accept failed!", server_fd);
//             }
//         }
//         // std::cout << "DEGUB: Connection accepted!" << std::endl;
//         // Handle the connection
//         char buffer[1024] = {0};
//         while (true) {
//             int bytes = recv(new_socket, buffer, 1023, 0);
//             if (bytes == -1) {
//                 if (errno == EAGAIN || errno == EWOULDBLOCK) {
//                     // No data available, continue waiting
//                     // std::cout << "DEGUB: No data available, continuing to wait..." << std::endl;
//                     break;
//                 } else {
//                     return error_handler("Receive failed!", new_socket);
//                 }
//             } else if (bytes == 0) {
//                 // Connection closed by client
//                 // std::cout << "DEGUB: Connection closed by client" << std::endl;
//                 break;
//             }
//             buffer[bytes] = '\0'; // Null-terminate the received data
//             // std::cout << "DEGUB: Received message: ";
//             std::cout << buffer << std::endl;
//         }
//     }
//     // close sockets
//     close(new_socket);
//     close(server_fd);
//     std::cout << "DEGUB: Server closed" << std::endl;
//     return 0;

// }
