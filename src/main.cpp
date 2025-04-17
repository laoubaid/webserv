/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:22:40 by laoubaid          #+#    #+#             */
/*   Updated: 2025/04/17 19:03:22 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include.hpp"

#define RED_CLR "\033[1;4;31m"
#define YLW_CLR "\033[1;4;93m"
#define DEF_CLR "\033[0m"


//////////////// delete this later //////////////////
std::string def_response() {
	std::string html = "<!DOCTYPE html><html><body><h1>Hello from the WebServer!</h1></body></html>";
	std::string response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(html.size()) + "\r\n"
		"Connection: keep-alive\r\n"  // Add this header to keep the connection alive
		"\r\n" + html;
	return response;
}
//////////////// delete this later //////////////////

void print_request(const char *buf, int id) {
	std::cout << "---------------------------------------------------------------------------------------" << std::endl;
	std::cout << "Recevie counter \t\t\t\t\t\t\t\t   (" << id << "):" << std::endl;
	std::cout << "---------------------------------------------------------------------------------------" << std::endl;
	std::cout << "-------------------------------- THE START OF REQUEST! --------------------------------" << std::endl;
	std::cout << buf << std::endl;
	std::cout << "--------------------------------- THE END OF REQUEST! ---------------------------------" << std::endl;
	std::cout << "\n\n" <<std::endl;
}

void socket_related_err(const char *msg, int fd, int should_exit) {
    std::cerr << RED_CLR << "ERROR :" << DEF_CLR;
	fflush(stderr);
	perror(msg);
    if (fd != -1) {
        close(fd);
    }
    if (should_exit) {
        exit(EXIT_FAILURE); // Use EXIT_FAILURE for standard error exit
    }
}

void	set_nonblocking_mode(int fd)
{
	int flags = 0;

	flags = fcntl(fd, F_GETFL, 0); // Get current file descriptor flags 
	if (flags == -1)
		socket_related_err(" fcntl F_GETFL failed!", fd, 1); 

	flags |= O_NONBLOCK; // Add non-blocking flag

	if (fcntl(fd, F_SETFL, flags) == -1) // Set new file descriptor flags
		socket_related_err(" fcntl F_SETFL failed!", fd, 1);
	std::cout << YLW_CLR << "DEGUB :"<< DEF_CLR << " Socket set to non-blocking mode" << std::endl;
}

int main(int ac, char **av)
{
	int		 server_fd, client_fd;
	sockaddr_in	addr;
	socklen_t	slen;

	// if (ac != 2) {
	// 	std::cerr << "Usage: " << av[0] << " [configuration file]" << std::endl;
	// 	return 1;
	// }
	
	// config parsing
	
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
		socket_related_err(" socket() failed! ", server_fd, 1);
	
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
		socket_related_err(" bind() failed! ", server_fd, 1);
	
	if (listen(server_fd, SOMAXCONN))
		socket_related_err(" listen() failed! ", server_fd, 1);

	
    int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		socket_related_err(" epoll_create1() failed! ", epoll_fd, 1);

	epoll_event svr_event {}; // init all struct member to 0
	epoll_event eventQueue[10]; // change to [MAX_EVENTS] later

	svr_event.data.fd = server_fd;
	svr_event.events = EPOLLIN;  // We’re interested in readable events (e.g., when a client connects or sends data).
    
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &svr_event) == -1) {
		close(epoll_fd);
		socket_related_err(" epoll_ctl() failed! ", server_fd, 1);
	}


	// socklen_t addrlen = sizeof(addr);
	std::string response = def_response();

	int recv_count = 0;
	while (true) {
		char buf[1024] {};

		int n = epoll_wait(epoll_fd, eventQueue, 10, -1); // chang to max_events
		if (n == -1) {
			close(epoll_fd);
			socket_related_err(" epoll_wait() failed! ", server_fd, 1);
		}
		for (int i = 0; i < n; i++) {
			if (eventQueue[i].data.fd == server_fd) {
				int client_fd = accept(server_fd, NULL, NULL);
				if (client_fd == -1)
					continue;
				set_nonblocking_mode(client_fd);
				epoll_event clt_event {};
				clt_event.data.fd = client_fd;
				clt_event.events = EPOLLIN | EPOLLOUT;// | EPOLLET;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &clt_event);
				std::cout << "\n-------------------------------- New client connected! --------------------------------\n" << std::endl;
			} else {
				int client_fd = eventQueue[i].data.fd;

				if (eventQueue[i].events & EPOLLIN) {
					// std::cout << ">>>>>>>> the client socket file descriptor: " << client_fd << std::endl;
					int nread = recv(client_fd, buf, 1023, 0);
					if (nread == -1) {
						if (errno != EAGAIN && errno != EWOULDBLOCK) {
							std::cout << "\n--------------------------------- Client disconnected (reset) ---------------------------------\n";
							epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
							close(client_fd);
						}
						continue;
					}
					if (nread == 0) {
						close(client_fd);
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
						std::cout << "\n--------------------------------- Client disconnected ---------------------------------\n" << std::endl;
						continue;
					}

					buf[nread] = '\0';
					print_request(buf, recv_count);
					recv_count++;
				}
				if (eventQueue[i].events & EPOLLOUT) {
					if (send(client_fd, response.c_str(), response.size(), MSG_NOSIGNAL) == -1) {
						if (errno == EPIPE || errno == ECONNRESET) {
							std::cout << "\n--------------------------------- Client disconnected ---------------------------------" << std::endl;
							close(client_fd);
							epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
						} else if (errno == EAGAIN || errno == EWOULDBLOCK) {
							std::cout << "\n--------------------------------- Client disconnected ---------------------------------" << std::endl;
							socket_related_err(" send() failed! , connection closed! ", client_fd, 0);
							epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
						}
					}
				}
			}
		}
	}
	close(server_fd);

	return 0;
}



// // Red foreground
// printf("\033[31mRed text\033[0m\n");

// // Green foreground, bold
// printf("\033[1;32mBold green text\033[0m\n");

// // Blue foreground, underlined
// printf("\033[4;34mUnderlined blue text\033[0m\n");

// // Yellow foreground, cyan background
// printf("\033[33;46mYellow text on cyan background\033[0m\n");

// // Reset to default colors and styles
// printf("\033[0mBack to normal\n");