/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:22:40 by laoubaid          #+#    #+#             */
/*   Updated: 2025/04/17 12:22:57 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include.hpp"

void	set_nonblocking_mode(int fd)
{
	int flags = 0;

	flags = fcntl(fd, F_GETFL, 0); // Get current file descriptor flags 
	if (flags == -1)
		socket_related_err("fcntl F_GETFL failed!", fd, 1); 

	flags |= O_NONBLOCK; // Add non-blocking flag

	if (fcntl(fd, F_SETFL, flags) == -1) // Set new file descriptor flags
		socket_related_err("fcntl F_SETFL failed!", fd, 1);
	std::cout << "DEGUB: Socket set to non-blocking mode" << std::endl;
}

int main()
{
	int		 server_fd, client_fd;
	sockaddr_in	addr;
	socklen_t	slen;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
		socket_related_err("socket creation failed!", server_fd, 1);
	
	//// instraction //// the SO_REUSEADDR flag use in order to avoid socket being in TIME_WAIT
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse)) < 0) {
		perror("ERROR SO_REUSEADDR:");
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(8080);


	// //////////////////////////////////////////////////////// Set socket to non-blocking mode
	set_nonblocking_mode(server_fd);


	if (bind(server_fd, (const sockaddr *)&addr, sizeof(addr)))
		socket_related_err("bind process failed!", server_fd, 1);
	
	if (listen(server_fd, SOMAXCONN))
		socket_related_err("listen process failed!", server_fd, 1);

	
    int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		socket_related_err("epoll_create1 failed", epoll_fd, 1);

	epoll_event svr_event {}; // init all struct member to 0
	epoll_event eventQueue[10]; // change to [MAX_EVENTS] later

	svr_event.data.fd = server_fd;
	svr_event.events = EPOLLIN;  // We’re interested in readable events (e.g., when a client connects or sends data).
    
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &svr_event) == -1) {
		close(epoll_fd);
		socket_related_err("an error occurs within epoll_ctl()", server_fd, 1);
	}
	
	

		// socklen_t addrlen = sizeof(addr);
	char buf[1024] = {0};
	std::string html = "<!DOCTYPE html><html><body><h1>Hello from the WebServer!</h1></body></html>";
	std::string response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(html.size()) + "\r\n"
		"Connection: keep-alive\r\n"  // Add this header to keep the connection alive
		"\r\n" + html;
	
	int id = 0;

	while (true) {
		int n = epoll_wait(epoll_fd, eventQueue, 10, -1); // chang to max_events
		if (n == -1) {
			close(epoll_fd);
			socket_related_err("epoll_wait", server_fd, 1);
		}
		for (int i = 0; i < n; i++) {
			if (eventQueue[i].data.fd == server_fd) {
				int client_fd = accept(server_fd, NULL, NULL);
				if (client_fd == -1)
					continue;
				set_nonblocking_mode(client_fd);
				epoll_event clt_event {};
				clt_event.data.fd = client_fd;
				clt_event.events = EPOLLIN;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &clt_event);
				std::cout << "New client connected!" << std::endl;
			} else {
				int client_fd = eventQueue[i].data.fd;
				std::cout << "the client socket filed: " << client_fd << std::endl;
				int nread = recv(client_fd, buf, 1023, 0);
				if (nread == -1) {
					socket_related_err("couldn't recive from client, terminating connection...", -1, 0);
					continue;
				}
				buf[nread] = '\0';
				// std::cout << "request received!" << std::endl;
				// buf[std::string(buf).find('\n')] = '\0';
				std::cout << "Received \t\t\t(" << id << "):\n\n" << buf << "\n-----------------" << std::endl;
				if (send(client_fd, response.c_str(), response.size(), MSG_NOSIGNAL) == -1){
					socket_related_err("can't send to client, terminating connection...", -1, 0);
				}

				// close(client_fd);
				id++;
			}

		}
	}
	close(server_fd);

	return 0;
}

