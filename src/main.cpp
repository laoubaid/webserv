/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:22:40 by laoubaid          #+#    #+#             */
/*   Updated: 2025/05/12 04:50:36 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include.hpp"
#include "server/ServerSkt.hpp"
#include "server/ClientSkt.hpp"


//////////////// delete this later //////////////////
std::string def_response() {
	std::string html = "<!DOCTYPE html><html><body><h1>Hello from the WebServer!</h1></body></html>";
	std::string response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(html.size()) + "\r\n"
		// "Content-Length: " + std::to_string(html.size()) + "\r\n"
		"Connection: keep-alive\r\n"  // Add this header to keep the connection alive
		"\r\n" + html;// + "\n";
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

// if (ac != 2) {
// 	std::cerr << "Usage: " << av[0] << " [configuration file]" << std::endl;
// 	return 1;
// }

// config parsing

/*
*/

void	init_conf(t_conf *conf) {
	conf->_domain = AF_INET;
	conf->_type = SOCK_STREAM | SOCK_NONBLOCK;
	conf->_protocol = 0;
	conf->_address.sin_family = conf->_domain;
	conf->_address.sin_addr.s_addr = INADDR_ANY;
	conf->_address.sin_port = htons(8080);
}

int	new_connection(int server_fd, int epoll_fd) {
	int client_fd = accept(server_fd, NULL, NULL);
	if (client_fd == -1)
		return -1;
	epoll_event clt_event {};
	clt_event.data.fd = client_fd;
	clt_event.events = EPOLLIN | EPOLLOUT;// | EPOLLET;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &clt_event);
	std::cout << CONN_CLR <<"\n$ New client connected! fd: " << client_fd << DEF_CLR << std::endl;

	return client_fd;
}

// char *recv_data(int client_fd, int epoll_fd) {
//     char	*buf;

// 	memset(buf, 0, buf_size - 1);
// 	buf = new
// 	std::cout << "EPOLLIN event detected!" << std::endl;
// 	int nread = recv(client_fd, buf, buf_size - 1, 0);
// 	if (nread <= 0) {
// 		if (nread == 0)
// 			std::cout << "Client disconnected! (recv() == 0)" << std::endl;
// 		else
// 			perror("recv() failed");
// 		close(client_fd);
// 		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
// 		std::cout << DISC_CLR << "\n$ Client disconnected! (epoll IN) fd: " << client_fd << DEF_CLR << std::endl;
// 		return NULL;
// 	}
// 	buf[nread] = 0;
// 	return buf;
// }

int main(int ac, char **av)
{
	int		 server_fd;

	t_conf	conf;

	
	init_conf(&conf);
	ServerSkt	svr_skt(conf);

	server_fd = svr_skt.get_fd();
	svr_skt.bind();
	svr_skt.listen(SOMAXCONN);

	
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
		socket_related_err(" epoll_create1() failed! ", 1);

	epoll_event svr_event {}; // init all struct member to 0
	epoll_event eventQueue[100]; // change to [MAX_EVENTS] later

	svr_event.data.fd = server_fd;
	svr_event.events = EPOLLIN;  // We’re interested in readable events (e.g., when a client connects or sends data).
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &svr_event) == -1) {
		close(epoll_fd);
		socket_related_err(" epoll_ctl() failed! ", 1);
	}

	std::string response = def_response();

	int recv_count = 0;
	int flag = 0;
    int buf_size = 32;
    char buf[buf_size];
    std::map <int, ClientSkt*> client_sockets;


	while (true) {
        memset(buf, 0, buf_size); // Clear the buffer before each recv
		int n = epoll_wait(epoll_fd, eventQueue, 100, -1); // chang to max_events
		if (n == -1) {
			close(epoll_fd);
			socket_related_err(" epoll_wait() failed! ", 1);
		}
		for (int i = 0; i < n; i++) {
			
            // first the server socket check for new connections
            ////////////////////////////////////////////////////
            
            if (eventQueue[i].data.fd == server_fd) {
				int client_fd = new_connection(server_fd, epoll_fd);
				if (client_fd == -1)
					continue;
                client_sockets[client_fd] = new ClientSkt(client_fd);
			}
            
            
            // the client socket check for data receiving and sending
            /////////////////////////////////////////////////////////

            else {
				int client_fd = eventQueue[i].data.fd;

				if (eventQueue[i].events & EPOLLIN) {
					// recv_data(client_fd, epoll_fd);
                    std::cout << "EPOLLIN event detected!" << std::endl;
					int nread = recv(client_fd, buf, buf_size - 1, 0);
					if (nread <= 0) {
                        if (nread == 0)
                            std::cout << "Client disconnected! (recv() == 0)" << std::endl;
                        else
                            perror("recv() failed");
						close(client_fd);
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
						std::cout << DISC_CLR << "\n$ Client disconnected! (epoll IN) fd: " << client_fd << DEF_CLR << std::endl;
						continue;
					}
					buf[nread] = 0;
                    flag = client_sockets[client_fd]->handle_request(buf);
					recv_count++;
				}

				if (eventQueue[i].events & EPOLLOUT && client_sockets[client_fd]->is_recv_done()) {
                    std::cout << "EPOLLOUT event detected!" << std::endl;
                    if (send(client_fd, response.c_str(), response.size(), MSG_NOSIGNAL) == -1) {
                            close(client_fd);
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                            std::cout << DISC_CLR <<"\n$ Client disconnected! (epoll OUT) fd: " << client_fd << DEF_CLR << std::endl;
                            socket_related_err(" send() failed! , connection closed! ", 0);
                    }
                    client_sockets[client_fd]->reset_recv_done();
				}
			}
		}
	}
	close(server_fd);

	return 0;
}
