#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>


int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(9999);

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, SOMAXCONN);

    int epoll_fd = epoll_create1(0);

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

    std::cout << "Listening on port 9999..." << std::endl;

    epoll_event events[1];
	int nfds = epoll_wait(epoll_fd, events, 1, -1);

	int fd = events[0].data.fd;
	int client_fd = -1;
	if (fd == server_fd) {
		// Accept a new client
		sockaddr_in client_addr{};
		socklen_t client_len = sizeof(client_addr);
		client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);

		std::cout << "Accepted a connection!" << std::endl;

		char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        std::cout << "[ACCEPT] Client: " << client_ip << ":" << ntohs(client_addr.sin_port) << "\n";
		// Add client to epoll for reading
		epoll_event client_ev{};
		client_ev.events = EPOLLIN;
		client_ev.data.fd = client_fd;
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);
		sleep(3); // Simulate some delay before processing
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
		close(client_fd);
	}
	client_fd = -1;
	epoll_wait(epoll_fd, events, 1, -1);
	fd = events[0].data.fd;

	if (fd == server_fd) {
		// Accept a new client
		sockaddr_in client_addr{};
		socklen_t client_len = sizeof(client_addr);
		client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
		std::cout << "Accepted a connection!" << std::endl;
		char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        std::cout << "[ACCEPT] Client: " << client_ip << ":" << ntohs(client_addr.sin_port) << "\n";
		// Add client to epoll for reading
		epoll_event client_ev{};
		client_ev.events = EPOLLIN;
		client_ev.data.fd = client_fd;
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);
	}

	epoll_wait(epoll_fd, events, 1, -1);
	fd = events[0].data.fd;
	if (fd == client_fd) {
		char buffer[1024];
		ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
		if (bytes_read > 0) {
			buffer[bytes_read] = '\0';
			std::cout << "Received: " << buffer << std::endl;
		} else if (bytes_read < 0) {
			std::cerr << "Error reading from client." << std::endl;
		} else {
			std::cout << "Client disconnected." << std::endl;
		}
		std::cout << ">>> RECIVED from client !" << std::endl;
	}
    close(server_fd);
    close(epoll_fd);
    return 0;
}
