/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 19:01:04 by laoubaid          #+#    #+#             */
/*   Updated: 2025/09/23 16:18:28 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const serverConf& cfg, sockaddr_in addr) : Socket(addr), conf(cfg)
{
	std::cout << "Server constracteur called!" << std::endl;
	int reuse = 1;
	if (setsockopt(get_fd(), SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse)) < 0)
		throw std::runtime_error("failed to set socket option to reuse address!");
}

Server::Server(const serverConf& cfg, int fd) : Socket(fd), conf(cfg)
{
	std::cout << "Server constracteur called!" << std::endl;
}

Server::~Server() {
	for (std::map<int, Client*>::iterator it = client_sockets.begin(); it != client_sockets.end(); ++it)
		delete it->second;
	client_sockets.clear();
}

int	Server::accept_connections(int epoll_fd) {
	int	count = 0;
	int	client_fd;
	
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);

	client_fd = accept(get_fd(), (struct sockaddr *)&client_addr, &addr_len);
	while (client_fd != -1) {

		//! delete this sh!t
		char ip_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, sizeof(ip_str));
		int port = ntohs(client_addr.sin_port);


		count++;
		struct epoll_event clt_event;
		memset(&clt_event, 0, sizeof(clt_event));
		clt_event.data.fd = client_fd;
		clt_event.events = EPOLLIN | EPOLLOUT;  //* must check for both in and out at same time (subject requirements).
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &clt_event);
		std::cout << CONN_CLR <<"\n$ New client connected! fd: " << client_fd << ", ip:port > " << ip_str << ":" << port << DEF_CLR << std::endl;

		client_sockets[client_fd] = new Client(client_fd, conf, epoll_fd, client_addr);
		client_fd = accept(get_fd(), (struct sockaddr *)&client_addr, &addr_len);
	}
	return count;
}

int	Server::add_to_epoll(int epoll_fd) {
	struct epoll_event	svr_event;

	//* init all struct member 
	memset(&svr_event, 0, sizeof(svr_event));

	svr_event.data.fd = get_fd();
	svr_event.events = EPOLLIN | EPOLLOUT; //* We’re interested in readable events only (e.g., when a client connects)
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, get_fd(), &svr_event) == -1) {
		throw std::runtime_error("Server: epoll_ctl() failed!");
	}
	return 0;
}

int	Server::launch() {
	if (bind() == 0 && listen(SOMAXCONN) == 0) {
		std::cout << GRN_CLR << "Server is listening on port " << ntohs(get_sockeaddr().sin_port) << DEF_CLR << std::endl;
		return 0;
	}
	return 1;
}

void Server::check_timeout() {
	std::map<int, Client*>::iterator it = client_sockets.begin();

	while (it != client_sockets.end()) {
		if (it->second->check_timeout()) {
			delete it->second;
			it = client_sockets.erase(it);
		} else {
			++it;
		}
	}
}
