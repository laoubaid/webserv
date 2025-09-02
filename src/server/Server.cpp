/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 19:01:04 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/02 05:52:41 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const serverConf& cfg, sockaddr_in addr) : Socket(addr), conf(cfg)
{
    std::cout << "Server constracteur called!" << std::endl;
	int reuse = 1;
	if (setsockopt(get_fd(), SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse)) < 0) {
		throw std::runtime_error("failed to set socket option to reuse address!");
	}
}

Server::Server(const serverConf& cfg, int fd) : Socket(fd), conf(cfg)
{
    std::cout << "Server constracteur called!" << std::endl;
}

Server::~Server()
{
}

int Server::accept_connections(int epoll_fd) {
	int count = 0;
	int client_fd = accept(get_fd(), NULL, NULL);
	while (client_fd != -1) {
		count++;
		struct epoll_event clt_event;
		memset(&clt_event, 0, sizeof(clt_event));
		clt_event.data.fd = client_fd;
		clt_event.events = EPOLLIN | EPOLLOUT;
		// clt_event.events = EPOLLIN;
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &clt_event);
		// std::cout << CONN_CLR <<"\n$ New client connected! fd: " << client_fd << DEF_CLR << std::endl;

		client_sockets[client_fd] = new Client(client_fd, conf);

		client_fd = accept(get_fd(), NULL, NULL);
	}
	return count;
}

int	Server::add_to_epoll(int epoll_fd) {
	struct epoll_event svr_event;
	memset(&svr_event, 0, sizeof(svr_event)); // init all struct member to 0

	svr_event.data.fd = get_fd();
	svr_event.events = EPOLLIN | EPOLLET; // We’re interested in readable events (e.g., when a client connects)
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, get_fd(), &svr_event) == -1) {
		throw std::runtime_error("Server: epoll_ctl() failed!");
	}
	return 0;
}

int Server::launch() {
	if (bind() == 0 && listen(SOMAXCONN) == 0) {
		std::cout << GRN_CLR << "Server is listening on port " << ntohs(get_sockeaddr().sin_port) << DEF_CLR << std::endl;
		return 0;
	}
	return 1;
}
